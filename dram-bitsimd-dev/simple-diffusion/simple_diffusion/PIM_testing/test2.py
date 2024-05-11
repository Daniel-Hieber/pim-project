import ctypes
import numpy as np
import random
import math

# Load the shared library
lib = ctypes.CDLL("/u/dmh9ewu/pim/dram-bitsimd-dev/pim-func-sim/apps/convolution/libpim.so")

# Define function prototypes
lib.getDecomposedMatrix.argtypes = [
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_int, ctypes.POINTER(ctypes.POINTER(ctypes.c_int)),
    ctypes.POINTER(ctypes.POINTER(ctypes.c_int))
]
lib.getDecomposedMatrix.restype = None

lib.getMatrix.argtypes = [
    ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.POINTER(ctypes.POINTER(ctypes.c_int))
]
lib.getMatrix.restype = None

lib.createDevice.argtypes = []
lib.createDevice.restype = None

lib.performConv.argtypes = [
    ctypes.POINTER(ctypes.POINTER(ctypes.c_int)),
    ctypes.POINTER(ctypes.POINTER(ctypes.c_int)),
    ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int
]
lib.performConv.restype = None

# Define Python wrapper functions
def get_decomposed_matrix(matrix_row, matrix_column, filter_row, filter_column, stride, input_matrix):
    decomp_matrix = (ctypes.POINTER(ctypes.c_int) * (filter_row * filter_column))()
    for i in range(filter_row * filter_column):
        decomp_matrix[i] = (ctypes.c_int * matrix_row * matrix_column)()
    input_matrix_ptr = (ctypes.POINTER(ctypes.c_int) * len(input_matrix))()
    for i, row in enumerate(input_matrix):
        input_matrix_ptr[i] = (ctypes.c_int * len(row))(*row)
    lib.getDecomposedMatrix(matrix_row, matrix_column, filter_row, filter_column, stride, input_matrix_ptr, decomp_matrix)
    return [[decomp_matrix[i][j] for j in range(matrix_row * matrix_column)] for i in range(filter_row * filter_column)]

def get_matrix(row, column, padding):
    input_matrix = (ctypes.POINTER(ctypes.c_int) * (row + 2 * padding))()
    for i in range(row + 2 * padding):
        input_matrix[i] = (ctypes.c_int * (column + 2 * padding))()
        for j in range(column + 2 * padding):
            if i < padding or i >= row + padding or j < padding or j >= column + padding:
                input_matrix[i][j] = 0
            else:
                input_matrix[i][j] = random.randint(0, i * j)
    lib.getMatrix(row, column, padding, input_matrix)
    return [[input_matrix[i][j] for j in range(column + 2 * padding)] for i in range(row + 2 * padding)]

def create_device():
    lib.createDevice()

def perform_conv(filter_matrix, input_matrix, num_required_pim_rows, num_required_pim_col):
    filter_matrix_ptr = (ctypes.POINTER(ctypes.c_int) * len(filter_matrix))()
    for i, row in enumerate(filter_matrix):
        filter_matrix_ptr[i] = (ctypes.c_int * len(row))(*row)
    input_matrix_ptr = (ctypes.POINTER(ctypes.c_int) * len(input_matrix))()
    for i, row in enumerate(input_matrix):
        input_matrix_ptr[i] = (ctypes.c_int * len(row))(*row)
    output_matrix = (ctypes.c_int * num_required_pim_col)()
    lib.performConv(filter_matrix_ptr, input_matrix_ptr, output_matrix, num_required_pim_rows, num_required_pim_col)
    return list(output_matrix)

def main():
    num_cols = 65536

    matrix_row, matrix_column, matrix_z = 224, 224, 3
    filter_row, filter_column, filter_z = 3, 3, 64
    stride, padding = 1, 1

    num_of_mat_per_row = min(math.floor(num_cols / (matrix_row * matrix_column)), matrix_z)
    num_of_pim_row = filter_row * filter_column
    num_of_pim_column = num_of_mat_per_row * matrix_row * matrix_column

    print("Matrix per core:", num_of_mat_per_row)

    create_device()

    for i in range(filter_z):
        filter_matrix = get_matrix(filter_row, filter_column, 0)
        print("Filter Matrix is done")

        for j in range(0, matrix_z, num_of_mat_per_row):
            input_matrices = [get_matrix(matrix_row, matrix_column, padding) for _ in range(num_of_mat_per_row)]
            decomp_matrices = [get_decomposed_matrix(matrix_row + 2 * padding, matrix_column + 2 * padding, filter_row, filter_column, stride, input_mat) for input_mat in input_matrices]
            input_matrices_flattened = [np.concatenate(decomp_mat) for decomp_mat in decomp_matrices]

            out_vector = []
            for idx, input_mat in enumerate(input_matrices_flattened):
                print("row:", i, "column size:", len(input_mat))
            
            perform_conv(filter_matrix, input_matrices_flattened, num_of_pim_row, num_of_pim_column)

    # Simulating stats
    print("PIM Stats")

if __name__ == "__main__":
    main()
