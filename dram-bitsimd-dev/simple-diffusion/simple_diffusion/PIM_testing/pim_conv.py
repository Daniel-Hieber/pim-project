import ctypes
import numpy as np
import torch

# Load the shared library (.so) containing the PIM kernel function
pim_lib = ctypes.CDLL("/u/dmh9ewu/pim/dram-bitsimd-dev/pim-func-sim/apps/convolution/libpim.so")

# Define the argument and return types of the PIM kernel functions
pim_lib.getDecomposedMatrix.argtypes = [
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.POINTER(ctypes.POINTER(ctypes.c_int)), ctypes.POINTER(ctypes.POINTER(ctypes.c_int))]

pim_lib.getDecomposedMatrix.restype = None

# pim_lib._Z19getDecomposedMatrixiiiiiRSt6vectorIS_IiSaIiEESaIS1_EES4_.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)]
# pim_lib._Z19getDecomposedMatrixiiiiiRSt6vectorIS_IiSaIiEESaIS1_EES4_.restype = None

pim_lib.getMatrix.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.POINTER(ctypes.c_int))]
pim_lib.getMatrix.restype = None

pim_lib.createDevice.argtypes = []
pim_lib.createDevice.restype = None

pim_lib.performConv.argtypes = [ctypes.POINTER(ctypes.POINTER(ctypes.c_int)), ctypes.POINTER(ctypes.POINTER(ctypes.c_int)),
                                ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int]
pim_lib.performConv.restype = None

# Define wrapper functions for each PIM kernel function
def get_decomposed_matrix(matrixRow, matrixColumn, filterRow, filterColumn, stride, inputMatrix):
    decompMatrix = (ctypes.POINTER(ctypes.c_int) * (filterRow * filterColumn))()
    for i in range(filterRow * filterColumn):
        decompMatrix[i] = (ctypes.c_int * ((matrixRow - filterRow + 1) * (matrixColumn - filterColumn + 1)))()
    pim_lib.getDecomposedMatrix(matrixRow, matrixColumn, filterRow, filterColumn, stride, inputMatrix, decompMatrix)
    return decompMatrix

def get_matrix(row, column, padding):
    inputMatrix = (ctypes.POINTER(ctypes.c_int) * (row + (padding * 2)))()
    for i in range(row + (padding * 2)):
        inputMatrix[i] = (ctypes.c_int * (column + (padding * 2)))()
        for j in range(column + (padding * 2)):
            if i < padding or i >= row or j < padding or j >= column:
                inputMatrix[i][j] = 0
            else:
                inputMatrix[i][j] = ctypes.c_int(np.random.randint(i * j + 1))
    pim_lib.getMatrix(row, column, padding, inputMatrix)
    return inputMatrix

def create_device():
    pim_lib.createDevice()

def perform_conv(filterMatrix, inputMatrix, numRequiredPIMRows, numRequiredPIMCols):
    filterMatrixPtr = (ctypes.POINTER(ctypes.c_int) * len(filterMatrix))()
    for i in range(len(filterMatrix)):
        filterMatrixPtr[i] = (ctypes.c_int * len(filterMatrix[i]))(*filterMatrix[i])

    inputMatrixPtr = (ctypes.POINTER(ctypes.c_int) * len(inputMatrix))()
    for i in range(len(inputMatrix)):
        inputMatrixPtr[i] = (ctypes.c_int * len(inputMatrix[i]))(*inputMatrix[i])

    outputMatrix = (ctypes.c_int * numRequiredPIMCols)()
    pim_lib.performConv(filterMatrixPtr, inputMatrixPtr, outputMatrix, numRequiredPIMRows, numRequiredPIMCols)
    return outputMatrix

# Example usage of the PIM kernel
def main():
    # Set up parameters
    matrixRow = 224
    matrixColumn = 224
    matrixZ = 3
    filterRow = 3
    filterColumn = 3
    filterZ = 64
    stride = 1
    padding = 1
    numCols = 65536

    numOfMatPerRow = min(int(numCols / (matrixRow * matrixColumn)), matrixZ)
    numOfPIMRow = filterRow * filterColumn
    numOfPIMColumn = numOfMatPerRow * matrixRow * matrixColumn

    print("Matrix per core:", numOfMatPerRow)

    # Create PIM device
    create_device()

    # Loop over filters and input matrices
    for i in range(filterZ):
        filterMatrix = get_matrix(filterRow, filterColumn, 0)
        print("Filter Matrix is done")
        for j in range(0, matrixZ, numOfMatPerRow):
            inputMatrices = []
            for k in range(numOfMatPerRow):
                imageMat = get_matrix(matrixRow, matrixColumn, padding)
                tempColumn = matrixColumn + (padding * 2)
                tempRow = matrixRow + (padding * 2)
                decompVec = get_decomposed_matrix(tempRow, tempColumn, filterRow, filterColumn, stride, imageMat)
                inputMatrices.extend(decompVec)
            outVector = perform_conv(filterMatrix, inputMatrices, numOfPIMRow, numOfPIMColumn)
            print("row:", i, "column size:", len(outVector))

    # Show PIM device stats
    pim_lib.pimShowStats()

if __name__ == "__main__":
    main()
