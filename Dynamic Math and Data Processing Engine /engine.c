#include <stdio.h>
#include <stdlib.h>
#include "engine.h"

/* ============================
 *   Utility Functions
 * ============================ */

void printDataset(int *data, int size) {
    if (size == 0) {
        printf("Dataset is empty.\n");
        return;
    }
    printf("Dataset: ");
    for (int i = 0; i < size; i++)
        printf("%d ", data[i]);
    printf("\n");
}

/* Dynamically add a value to the dataset */
void addValue(int **data, int *size, int value) {
    int *tmp = realloc(*data, (*size + 1) * sizeof(int));
    if (!tmp) {
        printf("Memory allocation failed. Value not added.\n");
        return;
    }
    *data = tmp;
    (*data)[*size] = value;
    (*size)++;
}

/* Delete a value by index and shrink the dataset */
void deleteValue(int **data, int *size, int index) {
    if (*size == 0 || index < 0 || index >= *size) {
        printf("Invalid index. Cannot delete.\n");
        return;
    }

    /* Shift elements left from index */
    for (int i = index; i < *size - 1; i++)
        (*data)[i] = (*data)[i + 1];

    int newSize = *size - 1;
    int *tmp = NULL;

    if (newSize > 0) {
        tmp = realloc(*data, newSize * sizeof(int));
        if (!tmp) {
            /* Realloc failed, but original pointer is still valid;
               we just keep the old allocation size (logical size is reduced). */
            printf("Memory reallocation error. Logical size updated, physical size unchanged.\n");
            *size = newSize;
            return;
        }
        *data = tmp;
        *size = newSize;
    } else {
        /* New size = 0 -> free and reset */
        free(*data);
        *data = NULL;
        *size = 0;
    }
    printf("Value at index %d deleted successfully.\n", index);
}

/* ============================
 *   File Handling
 * ============================ */

/* Load integers from a file into the dataset */
void loadDataFromFile(const char *filename, int **data, int *size) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("File '%s' not found. No data loaded.\n", filename);
        return;
    }

    /* Clear any existing data */
    free(*data);
    *data = NULL;
    *size = 0;

    int val;
    while (fscanf(fp, "%d", &val) == 1) {
        addValue(data, size, val);
    }

    fclose(fp);
    printf("Data loaded from '%s'. (%d value(s))\n", filename, *size);
}

/* Save current dataset to a file */
void saveDataToFile(const char *filename, int *data, int size) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Could not open '%s' for writing.\n", filename);
        return;
    }

    for (int i = 0; i < size; i++)
        fprintf(fp, "%d\n", data[i]);

    fclose(fp);
    printf("Results saved to '%s'.\n", filename);
}

/* ============================
 *   Operations via Function Pointers
 * ============================ */

void op_sum(int *data, int size) {
    if (size == 0) {
        printf("Dataset is empty. Cannot compute sum.\n");
        return;
    }
    int sum = 0;
    for (int i = 0; i < size; i++)
        sum += data[i];
    printf("Sum = %d\n", sum);
}

void op_average(int *data, int size) {
    if (size == 0) {
        printf("Dataset is empty. Cannot compute average.\n");
        return;
    }
    int sum = 0;
    for (int i = 0; i < size; i++)
        sum += data[i];
    printf("Average = %.2f\n", (float)sum / size);
}

void op_max(int *data, int size) {
    if (size == 0) {
        printf("Dataset is empty. Cannot find maximum.\n");
        return;
    }
    int max = data[0];
    for (int i = 1; i < size; i++)
        if (data[i] > max)
            max = data[i];
    printf("Max = %d\n", max);
}

void op_min(int *data, int size) {
    if (size == 0) {
        printf("Dataset is empty. Cannot find minimum.\n");
        return;
    }
    int min = data[0];
    for (int i = 1; i < size; i++)
        if (data[i] < min)
            min = data[i];
    printf("Min = %d\n", min);
}

/* Bubble sort ascending */
void op_sortAscending(int *data, int size) {
    if (size == 0) {
        printf("Dataset is empty. Nothing to sort.\n");
        return;
    }
    for (int i = 0; i < size - 1; i++)
        for (int j = 0; j < size - i - 1; j++)
            if (data[j] > data[j + 1]) {
                int t = data[j];
                data[j] = data[j + 1];
                data[j + 1] = t;
            }
    printf("Sorted ascending.\n");
    printDataset(data, size);
}

/* Bubble sort descending */
void op_sortDescending(int *data, int size) {
    if (size == 0) {
        printf("Dataset is empty. Nothing to sort.\n");
        return;
    }
    for (int i = 0; i < size - 1; i++)
        for (int j = 0; j < size - i - 1; j++)
            if (data[j] < data[j + 1]) {
                int t = data[j];
                data[j] = data[j + 1];
                data[j + 1] = t;
            }
    printf("Sorted descending.\n");
    printDataset(data, size);
}

/* Linear search for a value */
void op_search(int *data, int size) {
    if (size == 0) {
        printf("Dataset is empty. Nothing to search.\n");
        return;
    }

    int target;
    printf("Enter value to search: ");
    if (scanf("%d", &target) != 1) {
        printf("Invalid input. Search aborted.\n");
        /* clear stdin */
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        return;
    }

    for (int i = 0; i < size; i++) {
        if (data[i] == target) {
            printf("Value %d found at index %d.\n", target, i);
            return;
        }
    }

    printf("Value %d not found in dataset.\n", target);
}

/* ============================
 *   Menu System
 * ============================ */

void menu(void) {
    int *data = NULL;
    int size  = 0;
    int choice, val;

    /* Array of function pointers for operations */
    Operation operations[] = {
        op_sum,            /* 0 */
        op_average,        /* 1 */
        op_max,            /* 2 */
        op_min,            /* 3 */
        op_sortAscending,  /* 4 */
        op_sortDescending, /* 5 */
        op_search          /* 6 */
    };

    while (1) {
        printf("\n===== Dynamic Math & Data Engine =====\n");
        printf("1. Add value\n");
        printf("2. Delete value by index\n");
        printf("3. Show dataset\n");
        printf("4. Load from file (input.txt)\n");
        printf("5. Save to file (output.txt)\n");
        printf("6. Select & run operation (via function pointer)\n");
        printf("0. Exit\n");
        printf("Choose: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            continue;
        }

        switch (choice) {
        case 1:
            printf("Enter number: ");
            if (scanf("%d", &val) != 1) {
                printf("Invalid input. Value not added.\n");
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                break;
            }
            addValue(&data, &size, val);
            break;

        case 2:
            printf("Enter index to delete: ");
            if (scanf("%d", &val) != 1) {
                printf("Invalid input. Delete aborted.\n");
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                break;
            }
            deleteValue(&data, &size, val);
            break;

        case 3:
            printDataset(data, size);
            break;

        case 4:
            loadDataFromFile("input.txt", &data, &size);
            break;

        case 5:
            saveDataToFile("output.txt", data, size);
            break;

        case 6:
            printf("\nAvailable operations:\n");
            printf("0: Sum\n");
            printf("1: Average\n");
            printf("2: Max\n");
            printf("3: Min\n");
            printf("4: Sort Ascending\n");
            printf("5: Sort Descending\n");
            printf("6: Search Value\n");
            printf("Select operation index: ");

            if (scanf("%d", &val) != 1) {
                printf("Invalid input. Operation aborted.\n");
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                break;
            }

            if (val < 0 || val > 6) {
                printf("Invalid operation index.\n");
            } else {
                Operation op = operations[val];
                op(data, size);
            }
            break;

        case 0:
            free(data);
            printf("Exiting. All dynamically allocated memory freed.\n");
            return;

        default:
            printf("Invalid choice. Please select a valid menu option.\n");
        }
    }
}