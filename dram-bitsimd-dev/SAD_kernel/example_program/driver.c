#include <stdio.h>
#include <stdlib.h>

#include "pattern_matching.h"

/**
 * @brief Search for a pattern of bytes within a longer sequence of bytes
 * 
 * This program creates a random array of bytes and selects a random chunk of 
 * the array as the pattern to search for. 
 */
int main() {

    // Problem size
    int vector_size = 16*1024;
    int subvector_size = 1024;

    char *vector = (char*)malloc(vector_size * sizeof(char));
    char *subvector = (char*)malloc(subvector_size * sizeof(char));

    // Initialize vector
    for(int i = 0; i < vector_size; i++)
        vector[i] = rand() % 256;   

    // Initialize subvector
    int correct_idx = rand() % (vector_size - subvector_size);
    for(int i = 0; i < subvector_size; i++)
        subvector[i] = vector[i + correct_idx];

    // Search vector for subvector
    int min_idx = pattern_match(vector, subvector, vector_size, subvector_size);

    // Check correctness
    if(min_idx == correct_idx)
        printf("Best match found!\n");
    else
        printf("Failed to find best match\n");

    return 0;
}