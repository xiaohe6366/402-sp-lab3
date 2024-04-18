
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    double* data;      // Pointer to the dynamically-allocated array of data points
    int size;          // Number of elements currently stored in the array
    int capacity;      // Total capacity of the array
} DynamicArray;

// Function prototypes with detailed descriptions
DynamicArray* create_dynamic_array(int initial_capacity);
void free_dynamic_array(DynamicArray* array);
int read_data_from_file(const char* filename, DynamicArray* array);
void resize_array_if_needed(DynamicArray* array);
double compute_mean(const DynamicArray* array);
double compute_stddev(const DynamicArray* array, double mean);
double compute_median(DynamicArray* array);
double compute_mode(DynamicArray* array);
double compute_harmonic_mean(const DynamicArray* array);
double babylonian_sqrt(double value);
int compare_double(const void* a, const void* b);
void print_statistics(int size, int capacity, double mean, double median, double mode, double stddev, double harmonic_mean);

// Main function
int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    DynamicArray* data_array = create_dynamic_array(20);
    if (!data_array) {
        fprintf(stderr, "Memory allocation failed for data array.\n");
        return EXIT_FAILURE;
    }

    if (!read_data_from_file(argv[1], data_array)) {
        free_dynamic_array(data_array);
        return EXIT_FAILURE;
    }

    qsort(data_array->data, data_array->size, sizeof(double), compare_double);
    double mean = compute_mean(data_array);
    double median = compute_median(data_array);
    double mode = compute_mode(data_array);
    double stddev = compute_stddev(data_array, mean);
    double harmonic_mean = compute_harmonic_mean(data_array);

    print_statistics(data_array->size, data_array->capacity, mean, median, mode, stddev, harmonic_mean);
    free_dynamic_array(data_array);
    return EXIT_SUCCESS;
}

// Definitions of all functions with comprehensive documentation

/**
 * Creates and initializes a DynamicArray with a specified initial capacity.
 * Returns a pointer to the new DynamicArray or NULL if memory allocation fails.
 */
DynamicArray* create_dynamic_array(int initial_capacity) {
    DynamicArray* array = (DynamicArray*)malloc(sizeof(DynamicArray));
    if (array == NULL) {
        return NULL;
    }
    array->data = (double*)malloc(initial_capacity * sizeof(double));
    if (array->data == NULL) {
        free(array);
        return NULL;
    }
    array->size = 0;
    array->capacity = initial_capacity;
    return array;
}

/**
 * Frees the memory allocated for a DynamicArray.
 */
void free_dynamic_array(DynamicArray* array) {
    if (array != NULL) {
        free(array->data);
        free(array);
    }
}

/**
 * Reads double values from a file into a DynamicArray, resizing as necessary.
 * Returns 1 on success, 0 on failure.
 */
int read_data_from_file(const char* filename, DynamicArray* array) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    double value;
    while (fscanf(file, "%lf", &value) == 1) {
        resize_array_if_needed(array);
        array->data[array->size++] = value;
    }
    fclose(file);
    return 1;
}

/**
 * Ensures the DynamicArray has sufficient capacity to add new elements,
 * doubling its size if necessary.
 */
void resize_array_if_needed(DynamicArray* array) {
    if (array->size == array->capacity) {
        int new_capacity = 2 * array->capacity;
        double* new_data = (double*)realloc(array->data, new_capacity * sizeof(double));
        if (new_data == NULL) {
            fprintf(stderr, "Memory reallocation failed.\n");
            free_dynamic_array(array);
            exit(EXIT_FAILURE);
        }
        array->data = new_data;
        array->capacity = new_capacity;
    }
}

/**
 * Computes the mean (average) of all elements in the DynamicArray.
 */
double compute_mean(const DynamicArray* array) {
    double sum = 0;
    for (int i = 0; i < array->size; i++) {
        sum += array->data[i];
    }
    return sum / array->size;
}

/**
 * Computes the standard deviation of elements in the DynamicArray.
 */
double compute_stddev(const DynamicArray* array, double mean) {
    double sum_sq = 0;
    for (int i = 0; i < array->size; i++) {
        double diff = array->data[i] - mean;
        sum_sq += diff * diff;
    }
    return babylonian_sqrt(sum_sq / array->size);
}

/**
 * Implements the Babylonian method to compute the square root of a value.
 */
double babylonian_sqrt(double value) {
    double x = value, y = 1.0, e = 0.000001;
    while (x - y > e) {
        x = (x + y) / 2;
        y = value / x;
    }
    return x;
}

/**
 * Computes the median of elements in the DynamicArray.
 */
double compute_median(DynamicArray* array) {
    int middle_index = array->size / 2;
    if (array->size % 2 == 0) {
        return (array->data[middle_index - 1] + array->data[middle_index]) / 2.0;
    } else {
        return array->data[middle_index];
    }
}

/**
 * Computes the mode of elements in the DynamicArray.
 */
double compute_mode(DynamicArray* array) {
    double mode = array->data[0];
    int max_count = 1, current_count = 1;
    for (int i = 1; i < array->size; i++) {
        if (array->data[i] == array->data[i - 1]) {
            current_count++;
        } else {
            if (current_count > max_count) {
                max_count = current_count;
                mode = array->data[i - 1];
            }
            current_count = 1;
        }
    }
    return mode;
}

/**
 * Computes the harmonic mean of elements in the DynamicArray.
 */
double compute_harmonic_mean(const DynamicArray* array) {
    double denominator_sum = 0;
    for (int i = 0; i < array->size; i++) {
        denominator_sum += 1 / array->data[i];
    }
    return array->size / denominator_sum;
}

/**
 * A helper function for qsort to compare two doubles.
 */
int compare_double(const void* a, const void* b) {
    double diff = *(double*)a - *(double*)b;
    return (diff > 0) - (diff < 0);
}

/**
 * Prints the calculated statistics.
 */
void print_statistics(int size, int capacity, double mean, double median, double mode, double stddev, double harmonic_mean) {
    printf("Results:\n--------\n");
    printf("Num values: %d\n", size);
    printf("Mean: %.3f\n", mean);
    printf("Median: %.3f\n", median);
    printf("Mode: %.3f\n", mode);
    printf("Standard Deviation: %.3f\n", stddev);
    printf("Harmonic Mean: %.3f\n", harmonic_mean);
    printf("Unused array capacity: %d\n", capacity - size);
}
