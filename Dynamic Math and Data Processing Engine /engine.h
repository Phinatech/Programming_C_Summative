#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h>

/*
 * Dynamic Math & Data Processing Engine
 * -------------------------------------
 * Uses function pointers to dynamically dispatch operations
 * on an integer dataset that is managed with dynamic memory.
 */

/* Function pointer type for operations on the dataset */
typedef void (*Operation)(int *data, int size);

/* File I/O */
void loadDataFromFile(const char *filename, int **data, int *size);
void saveDataToFile(const char *filename, int *data, int size);

/* Dataset management helpers */
void printDataset(int *data, int size);
void addValue(int **data, int *size, int value);
void deleteValue(int **data, int *size, int index);

/* Operations executed via function pointers */
void op_sum(int *data, int size);
void op_average(int *data, int size);
void op_max(int *data, int size);
void op_min(int *data, int size);
void op_sortAscending(int *data, int size);
void op_sortDescending(int *data, int size);
void op_search(int *data, int size);

/* Main menu controller */
void menu(void);

#endif /* ENGINE_H */