#include <limits.h>
#include <stdlib.h>
#include "pattern_matching.h"

/**
 * @brief Find the sum of absolute differences of the subvector at a particular
 * index within the vector
 *  
 * @param[in] vector : vector to look for the pattern in
 * @param[in] subvector : pattern that is being searched for
 * @param[in] idx : index of the vector to look at
 * @param[in] subvec_size : size of the subvector
 *
 * @returns Index where the best match begins
 */
int pattern_match(char *vector, char* subvector, int vector_size, int subvector_size) {
    int min_diff = INT_MAX;
    int min_idx = -1;
    int sum_abs_diff;
    for(int idx = 0; idx < vector_size - subvector_size; idx++){
        sum_abs_diff = 0;
        for(int i = 0; i < subvector_size; i++)
            sum_abs_diff += abs(vector[idx + i] - subvector[i]);

        if(sum_abs_diff < min_diff){
            min_idx = idx;
            min_diff = sum_abs_diff;
        }
    }
    return min_idx;
}