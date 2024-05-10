// Test: C++ version of vector arithmetic
// Copyright 2024 LavaLab @ University of Virginia. All rights reserved.

#include "libpimsim.h"
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
extern "C" {
    void getDecomposedMatrix(int matrixRow, int matrixColumn, int filterRow, int filterColumn, int stride, std::vector<std::vector<int>>& inputMatrix, std::vector<std::vector<int>>& decompMatrix) {
    for (int i = 0; i < filterRow * filterColumn; i++)
    {
        std::vector<int> tempVec;
        decompMatrix.push_back(tempVec);
    }

    for (int i = 0; i < (matrixRow - filterRow + 1); i += stride)
    {
        for (int j = 0; j < (matrixColumn - filterColumn + 1); j += stride)
        {
        int rowIDX = 0;
        for (int k = i; k < i + filterRow; k++)
        {
            for (int l = j; l < j + filterColumn; l++)
            {
            decompMatrix[rowIDX++].push_back(inputMatrix[k][l]);
            }
        }
        }
    }
    }
}

extern "C" {
    void getMatrix(int row, int column, int padding, std::vector<std::vector<int>>& inputMatrix) {
    srand((unsigned)time(NULL));
    for (int i = 0; i < row + (padding * 2); ++i)
    {
        std::vector<int> tempVec;
        for (int j = 0; j < column + (padding * 2); ++j)
        {
        if (i < padding || i > row || j < padding || j > column)
        {
            tempVec.push_back(0);
        }
        else
        {
            tempVec.push_back(rand() % (i*j+1));
        }
        }
        inputMatrix.push_back(tempVec);
    }
    }
}

extern "C" {
    void createDevice() {
    unsigned numCores = 16;
    unsigned numRows = 8192;
    unsigned numCols = 65536;
    PimStatus status = pimCreateDevice(PIM_FUNCTIONAL, numCores, numRows, numCols);
    if (status != PIM_OK)
    {
        std::cout << "Abort" << std::endl;
        return ;
    }
    }
}

extern "C" {
    void performConv(std::vector<std::vector<int>>& filterMatrix, std::vector<std::vector<int>>& inputMatrix, std::vector<int>& outputMatrix, int numRequiredPIMRows, int numRequiredPIMCol) {


    unsigned bitsPerElement = 32;
    std::vector<PimObjId> filterObjects;

    PimObjId obj1 = pimAlloc(PIM_ALLOC_V1, numRequiredPIMCol, bitsPerElement, PIM_INT32);
    if (obj1 == -1)
    {
        std::cout << "Abort" << std::endl;
        return ;
    }
    filterObjects.push_back(obj1);
    for (int i = 1; i < numRequiredPIMRows; i++)
    {
        PimObjId obj = pimAllocAssociated(PIM_ALLOC_V1, numRequiredPIMCol, bitsPerElement, filterObjects[0], PIM_INT32);
        if (obj == -1)
        {
        std::cout << "Abort" << std::endl;
        return ;
        }
        filterObjects.push_back(obj);
    }

    int idx = 0;
    for (int i = 0; i < filterMatrix.size(); ++i)
    {
        for (int j = 0; j < filterMatrix[i].size(); ++j)
        {
        PimStatus status = pimBroadCast(PIM_COPY_V, filterObjects[idx], filterMatrix[i][j]);
        if (status != PIM_OK)
        {
            std::cout << "Abort" << std::endl;
            return ;
        }
        ++idx;
        }
    }

    std::vector<PimObjId> matrixObjects;
    for (int i = 0; i < numRequiredPIMRows; i++)
    {
        PimObjId obj = pimAllocAssociated(PIM_ALLOC_V1, inputMatrix[i].size(), bitsPerElement, filterObjects[0], PIM_INT32);
        if (obj == -1)
        {
        std::cout << "Abort" << std::endl;
        return ;
        }
        matrixObjects.push_back(obj);
    }

    for (int i = 0; i < inputMatrix.size(); i++)
    {
        PimStatus status = pimCopyHostToDevice(PIM_COPY_V, (void *)inputMatrix[i].data(), matrixObjects[i]);
        if (status != PIM_OK)
        {
        std::cout << "Abort" << std::endl;
        return ;
        }
    }

    for (int i = 0; i < inputMatrix.size(); i++)
    {
        PimStatus status = pimMul(matrixObjects[i], filterObjects[i], filterObjects[i]);
        if (status != PIM_OK)
        {
        std::cout << "Abort" << std::endl;
        return ;
        }
    }

    for (int i = 1; i < numRequiredPIMRows; i++)
    {
        PimStatus status = pimAdd(filterObjects[0], filterObjects[i], filterObjects[0]);
        if (status != PIM_OK)
        {
        std::cout << "Abort" << std::endl;
        return ;
        }
    }
    outputMatrix.reserve(numRequiredPIMCol);

    PimStatus status = pimCopyDeviceToHost(PIM_COPY_V, filterObjects[0], outputMatrix.data());
    if (status != PIM_OK)
    {
        std::cout << "Abort" << std::endl;
        return ;
    }

    for (auto elem : filterObjects) {
        pimFree(elem);
    }

    for (auto elem : matrixObjects) {
        pimFree(elem);
    }

    }
}

int main(int argc, char *argv[])
{
    // this is a global of the max memory in the PIM simulator, not really sure of a good way to benchmark this against the given
  unsigned numCols = 65536;

    // matrixRow and matrix col is 128 (because the 32x32 is upsampled to this for conv)
    // matrixZ is 3 (RGB values)
    // filterRow and col are 3 (that's how the conv scans through, in a 3x3)
    // filterZ is the output features, which is 10
    // I think stride is 1 and padding is also 1

  int matrixRow = 128, matrixColumn = 128, matrixZ = 3, filterRow = 3, filterColumn = 3, filterZ = 10, stride = 1, padding = 1;
  int numOfMatPerRow = floor((1.0*numCols)/(matrixRow*matrixColumn)) < matrixZ ? floor((1.0*numCols)/(matrixRow*matrixColumn)) : matrixZ;
  int numOfPIMRow = filterRow*filterColumn, numOfPIMColumn = numOfMatPerRow*matrixRow*matrixColumn;

  cout << "Matrix per core: " << numOfMatPerRow << endl;

  createDevice();
  for (int i = 0; i < filterZ; i++) {
    std::vector<std::vector<int>> filterMat;
    getMatrix(filterRow, filterColumn, 0, filterMat);
    std::cout << "Filter Matrix is done\n";
    for (int j = 0; j < matrixZ; j+=numOfMatPerRow) {
      std::vector<std::vector<int>> inputMat(numOfPIMRow, vector<int>());
      for (int k = 0; k < numOfMatPerRow; k++) {
        std::vector<std::vector<int>> imageMat;
        getMatrix(matrixRow, matrixColumn, padding, imageMat);
        int tempColumn = matrixColumn + (padding * 2);
        int tempRow = matrixRow + (padding * 2);
        std::vector<std::vector<int>> dicompVec;
        getDecomposedMatrix(tempRow, tempColumn, filterRow, filterColumn, stride, imageMat, dicompVec);
        for (int idx = 0; idx < inputMat.size(); idx++) {
          inputMat[idx].reserve(inputMat[idx].size() + dicompVec[idx].size());
          inputMat[idx].insert(inputMat[idx].end(), make_move_iterator(dicompVec[idx].begin()), make_move_iterator(dicompVec[idx].end()));
        }
      }
      std::vector<int> outVector;
      for (int idx = 0; idx < inputMat.size(); idx++)
      {
        std::cout << "row: " << i << " column size: " << inputMat[idx].size() << "\n";
      }
      performConv(filterMat, inputMat, outVector, numOfPIMRow, numOfPIMColumn);
    }
  }

  pimShowStats();

  return 0;
}