#include <stdio.h>
#include "student.h"

/*
 * Main menu for the Student Management System.
 * Delegates all logic to functions defined in student.c.
 */
int main(void) {
    int choice;

    initSystem();  /* load data from file and init dynamic array */

    do {
        printf("\n===== Student Management System =====\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Update Student\n");
        printf("4. Delete Student\n");
        printf("5. Search / Sort\n");
        printf("6. Reports & Analytics\n");
        printf("7. Save to File\n");
        printf("8. Load from File\n");
        printf("0. Exit\n");
        printf("=====================================\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayAllStudents(); break;
            case 3: updateStudent(); break;
            case 4: deleteStudent(); break;
            case 5: searchSortMenu(); break;
            case 6: reportsMenu(); break;
            case 7: saveToFile(DATA_FILE); break;
            case 8: loadFromFile(DATA_FILE); break;
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
                break;
        }
    } while (choice != 0);

    cleanupSystem(); /* free dynamic memory */
    return 0;
}