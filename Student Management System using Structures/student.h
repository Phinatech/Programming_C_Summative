#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>

#define MAX_NAME     50
#define MAX_COURSE   50
#define MAX_SUBJECTS 10
#define DATA_FILE    "students.txt"

/*
 * Student structure:
 *  - id: unique identifier
 *  - name: student name (no spaces or use underscores)
 *  - age: positive integer
 *  - course: course name
 *  - grades: array of subject grades
 *  - numGrades: how many grades are stored
 *  - gpa: average of grades
 */
typedef struct {
    int   id;
    char  name[MAX_NAME];
    int   age;
    char  course[MAX_COURSE];
    int   numGrades;
    float grades[MAX_SUBJECTS];
    float gpa;
} Student;

/* Initialization and cleanup */
void initSystem(void);     /* load data from file, initialize memory */
void cleanupSystem(void);  /* free allocated memory */

/* CRUD operations */
void addStudent(void);
void displayAllStudents(void);
void updateStudent(void);
void deleteStudent(void);

/* Search / Sort menu and logic */
void searchSortMenu(void);

/* Reports and analytics menu and logic */
void reportsMenu(void);

/* File operations (exposed so main can force save/load) */
void saveToFile(const char *filename);
void loadFromFile(const char *filename);

#endif /* STUDENT_H */