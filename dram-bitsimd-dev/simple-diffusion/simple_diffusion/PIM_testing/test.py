import ctypes
import random
import numpy as np

# Load the shared library
pim_lib = ctypes.CDLL("/u/dmh9ewu/pim/dram-bitsimd-dev/pim-func-sim/apps/convolution/libpim.so")

PIM_FUNCTIONAL = 1
PIM_OK = 1
PIM_ALLOC_V1 = 0
PIM_COPY_V = 0


def get_decomposed_matrix(matrix_row, matrix_column, filter_row, filter_column, stride, input_matrix):
    decomp_matrix = [[] for _ in range(filter_row * filter_column)]

    for i in range(0, matrix_row - filter_row + 1, stride):
        for j in range(0, matrix_column - filter_column + 1, stride):
            row_idx = 0
            for k in range(i, i + filter_row):
                for l in range(j, j + filter_column):
                    decomp_matrix[row_idx].append(input_matrix[k][l])
                    row_idx += 1

    return decomp_matrix

def get_matrix(row, column, padding):
    np.random.seed(None)  # Seed with system time
    input_matrix = []
    for i in range(row + (padding * 2)):
        if i < padding or i >= row + padding:
            input_matrix.append([0] * (column + (padding * 2)))
        else:
            input_matrix.append(np.random.randint(0, i * j + 1) for j in range(column + (padding * 2)))
    return input_matrix

# Flag to track if PIM device is created
device_created = False

def create_device():
    global device_created
    if not device_created:
        num_cores = 16
        num_rows = 8192
        num_cols = 65536
        status = pim_lib.pimCreateDevice(PIM_FUNCTIONAL, num_cores, num_rows, num_cols)
        if status != PIM_OK:
            print("Abort (create device)")
            return
        device_created = True

def perform_conv(filter_matrix, input_matrix, num_required_pim_rows, num_required_pim_col):
    bits_per_element = 32
    filter_objects = []

    obj1 = pim_lib.pimAlloc(PIM_ALLOC_V1, num_required_pim_col, bits_per_element, PIM_INT32)
    if obj1 == -1:
        print("Abort conv1")
        return
    filter_objects.append(obj1)
    for i in range(1, num_required_pim_rows):
        obj = pim_lib.pimAllocAssociated(PIM_ALLOC_V1, num_required_pim_col, bits_per_element, filter_objects[0], PIM_INT32)
        if obj == -1:
            print("Abort conv2")
            return
        filter_objects.append(obj)

    idx = 0
    for i in range(len(filter_matrix)):
        for j in range(len(filter_matrix[i])):
            status = pim_lib.pimBroadCast(PIM_COPY_V, filter_objects[idx], filter_matrix[i][j])
            if status != PIM_OK:
                print("Abort conv3")
                return
            idx += 1

    matrix_objects = []
    for i in range(num_required_pim_rows):
        obj = pim_lib.pimAllocAssociated(PIM_ALLOC_V1, len(input_matrix[i]), bits_per_element, filter_objects[0], PIM_INT32)
        if obj == -1:
            print("Abort conv4")
            return
        matrix_objects.append(obj)

    for i in range(len(input_matrix)):
        status = pim_lib.pimCopyHostToDevice(PIM_COPY_V, (ctypes.c_void_p * len(input_matrix[i]))(*input_matrix[i]), matrix_objects[i])
        if status != PIM_OK:
            print("Abort conv5")
            return

    for i in range(len(input_matrix)):
        status = pim_lib.pimMul(matrix_objects[i], filter_objects[i], filter_objects[i])
        if status != PIM_OK:
            print("Abort conv6")
            return

    for i in range(1, num_required_pim_rows):
        status = pim_lib.pimAdd(filter_objects[0], filter_objects[i], filter_objects[0])
        if status != PIM_OK:
            print("Abortconv 7")
            return

    output_matrix = np.empty(num_required_pim_col, dtype=np.int32)
    status = pim_lib.pimCopyDeviceToHost(PIM_COPY_V, filter_objects[0], output_matrix.ctypes.data)
    if status != PIM_OK:
        print("Abort conv8")
        return

    for elem in filter_objects:
        pim_lib.pimFree(elem)

    for elem in matrix_objects:
        pim_lib.pimFree(elem)

    return output_matrix.tolist()

def main():
    num_cols = 65536

    matrix_row, matrix_column, matrix_z = 224, 224, 3
    filter_row, filter_column, filter_z = 3, 3, 64
    stride, padding = 1, 1
    num_of_mat_per_row = min(int(num_cols / (matrix_row * matrix_column)), matrix_z)
    num_of_pim_row = filter_row * filter_column
    num_of_pim_column = num_of_mat_per_row * matrix_row * matrix_column

    print("Matrix per core:", num_of_mat_per_row)

    create_device()
    for i in range(filter_z):
        filter_mat = get_matrix(filter_row, filter_column, 0)
        print("Filter Matrix is done")
        for j in range(0, matrix_z, num_of_mat_per_row):
            input_mat = []
            for k in range(num_of_mat_per_row):
                image_mat = get_matrix(matrix_row, matrix_column, padding)
                temp_column = matrix_column + (padding * 2)
                temp_row = matrix_row + (padding * 2)
                decomp_vec = get_decomposed_matrix(temp_row, temp_column, filter_row, filter_column, stride, image_mat)
                input_mat.extend(decomp_vec)
            out_vector = perform_conv(filter_mat, input_mat, num_of_pim_row, num_of_pim_column)
            for idx, row in enumerate(out_vector):
                print("row:", i, "column size:", len(row))

    pim_lib.pimShowStats()

if __name__ == "__main__":
    main()