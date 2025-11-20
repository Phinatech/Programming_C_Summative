#include <stdlib.h>
#include <string.h>
#include "student.h"

/* Dynamic storage for students (only visible inside this file) */
static Student *students      = NULL;
static int      studentCount  = 0;
static int      capacity      = 0;

/* ===================== Internal Helpers ===================== */

/* Ensure we have enough capacity in the dynamic array */
static void ensureCapacity(void) {
    if (studentCount >= capacity) {
        int newCap = (capacity == 0) ? 2 : capacity * 2;
        Student *tmp = realloc(students, newCap * sizeof(Student));
        if (!tmp) {
            printf("Error: memory allocation failed!\n");
            exit(EXIT_FAILURE);
        }
        students = tmp;
        capacity = newCap;
    }
}

/* Calculate GPA based on grades */
static void calcGPA(Student *s) {
    if (s->numGrades == 0) {
        s->gpa = 0.0f;
        return;
    }
    float sum = 0.0f;
    for (int i = 0; i < s->numGrades; i++) {
        sum += s->grades[i];
    }
    s->gpa = sum / s->numGrades;
}

/* Find student index by ID (linear search) */
static int findStudentIndexById(int id) {
    for (int i = 0; i < studentCount; i++) {
        if (students[i].id == id)
            return i;
    }
    return -1;
}

/* Check if ID is unique in the current array */
static int idIsUnique(int id) {
    return findStudentIndexById(id) == -1;
}

/* ===================== File Handling ===================== */

void saveToFile(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: could not open file '%s' for writing.\n", filename);
        return;
    }

    /* First line: number of students */
    fprintf(fp, "%d\n", studentCount);

    for (int i = 0; i < studentCount; i++) {
        Student *s = &students[i];
        fprintf(fp, "%d %s %d %s %d",
                s->id, s->name, s->age, s->course, s->numGrades);
        for (int j = 0; j < s->numGrades; j++) {
            fprintf(fp, " %.2f", s->grades[j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    printf("Data saved to '%s'.\n", filename);
}

void loadFromFile(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("No existing data file found ('%s'). Starting fresh.\n", filename);
        return;
    }

    int count;
    if (fscanf(fp, "%d", &count) != 1 || count < 0) {
        printf("Error: invalid data format in file.\n");
        fclose(fp);
        return;
    }

    /* Clear current data */
    free(students);
    students = NULL;
    studentCount = 0;
    capacity = 0;

    for (int i = 0; i < count; i++) {
        Student s;
        if (fscanf(fp, "%d %49s %d %49s %d",
                   &s.id, s.name, &s.age, s.course, &s.numGrades) != 5) {
            printf("Error: invalid student record format.\n");
            break;
        }

        if (s.numGrades < 0) s.numGrades = 0;
        if (s.numGrades > MAX_SUBJECTS) s.numGrades = MAX_SUBJECTS;

        for (int j = 0; j < s.numGrades; j++) {
            if (fscanf(fp, "%f", &s.grades[j]) != 1) {
                s.grades[j] = 0.0f;
            }
        }

        calcGPA(&s);
        ensureCapacity();
        students[studentCount++] = s;
    }

    fclose(fp);
    printf("Loaded %d student(s) from '%s'.\n", studentCount, filename);
}

/* ===================== Init / Cleanup ===================== */

void initSystem(void) {
    students     = NULL;
    studentCount = 0;
    capacity     = 0;
    loadFromFile(DATA_FILE);
}

void cleanupSystem(void) {
    free(students);
    students     = NULL;
    studentCount = 0;
    capacity     = 0;
}

/* ===================== CRUD Operations ===================== */

static void displayStudent(const Student *s) {
    printf("ID: %d | Name: %s | Age: %d | Course: %s | GPA: %.2f\n",
           s->id, s->name, s->age, s->course, s->gpa);
    printf("  Grades: ");
    for (int i = 0; i < s->numGrades; i++) {
        printf("%.1f ", s->grades[i]);
    }
    printf("\n");
}

void addStudent(void) {
    Student s;

    printf("Enter student ID: ");
    if (scanf("%d", &s.id) != 1) {
        printf("Invalid input.\n");
        return;
    }

    if (!idIsUnique(s.id)) {
        printf("Error: student ID must be unique.\n");
        return;
    }

    printf("Enter name (no spaces, e.g. John_Doe): ");
    scanf("%s", s.name);

    printf("Enter age: ");
    scanf("%d", &s.age);
    if (s.age <= 0) {
        printf("Error: invalid age.\n");
        return;
    }

    printf("Enter course (no spaces, e.g. Computer_Science): ");
    scanf("%s", s.course);

    printf("Enter number of subjects (1-%d): ", MAX_SUBJECTS);
    scanf("%d", &s.numGrades);
    if (s.numGrades <= 0 || s.numGrades > MAX_SUBJECTS) {
        printf("Error: invalid number of subjects.\n");
        return;
    }

    for (int i = 0; i < s.numGrades; i++) {
        float g;
        printf("Enter grade %d (0-100): ", i + 1);
        scanf("%f", &g);
        if (g < 0.0f || g > 100.0f) {
            printf("Error: invalid grade.\n");
            return;
        }
        s.grades[i] = g;
    }

    calcGPA(&s);
    ensureCapacity();
    students[studentCount++] = s;
    printf("Student added successfully.\n");
    saveToFile(DATA_FILE);
}

void displayAllStudents(void) {
    if (studentCount == 0) {
        printf("No student records.\n");
        return;
    }
    for (int i = 0; i < studentCount; i++) {
        printf("---- Student %d ----\n", i + 1);
        displayStudent(&students[i]);
    }
}

void updateStudent(void) {
    int id;
    printf("Enter ID of student to update: ");
    scanf("%d", &id);

    int idx = findStudentIndexById(id);
    if (idx == -1) {
        printf("Student not found.\n");
        return;
    }

    Student *s = &students[idx];
    printf("Updating student:\n");
    displayStudent(s);

    printf("Enter new name (or . to keep '%s'): ", s->name);
    char newName[MAX_NAME];
    scanf("%s", newName);
    if (strcmp(newName, ".") != 0) {
        strcpy(s->name, newName);
    }

    printf("Enter new age (or -1 to keep %d): ", s->age);
    int newAge;
    scanf("%d", &newAge);
    if (newAge > 0) {
        s->age = newAge;
    }

    printf("Enter new course (or . to keep '%s'): ", s->course);
    char newCourse[MAX_COURSE];
    scanf("%s", newCourse);
    if (strcmp(newCourse, ".") != 0) {
        strcpy(s->course, newCourse);
    }

    printf("Change grades? (1 = yes, 0 = no): ");
    int changeGrades;
    scanf("%d", &changeGrades);
    if (changeGrades == 1) {
        printf("Enter number of subjects (1-%d): ", MAX_SUBJECTS);
        scanf("%d", &s->numGrades);
        if (s->numGrades <= 0 || s->numGrades > MAX_SUBJECTS) {
            printf("Invalid number of subjects. Keeping old grades.\n");
        } else {
            for (int i = 0; i < s->numGrades; i++) {
                float g;
                printf("Enter grade %d (0-100): ", i + 1);
                scanf("%f", &g);
                if (g < 0.0f || g > 100.0f) {
                    printf("Invalid grade. Aborting update.\n");
                    return;
                }
                s->grades[i] = g;
            }
        }
    }

    calcGPA(s);
    printf("Student updated.\n");
    saveToFile(DATA_FILE);
}

void deleteStudent(void) {
    int id;
    printf("Enter ID of student to delete: ");
    scanf("%d", &id);

    int idx = findStudentIndexById(id);
    if (idx == -1) {
        printf("Student not found.\n");
        return;
    }

    for (int i = idx; i < studentCount - 1; i++) {
        students[i] = students[i + 1];
    }
    studentCount--;
    printf("Student deleted.\n");
    saveToFile(DATA_FILE);
}

/* ===================== Search & Sort ===================== */

static void searchById(void) {
    int id;
    printf("Enter ID to search: ");
    scanf("%d", &id);

    int idx = findStudentIndexById(id);
    if (idx == -1) {
        printf("Student not found.\n");
    } else {
        displayStudent(&students[idx]);
    }
}

static void searchByName(void) {
    char name[MAX_NAME];
    printf("Enter name to search (exact match): ");
    scanf("%s", name);

    int found = 0;
    for (int i = 0; i < studentCount; i++) {
        if (strcmp(students[i].name, name) == 0) {
            displayStudent(&students[i]);
            found = 1;
        }
    }
    if (!found) {
        printf("No students found with that name.\n");
    }
}

/* Simple Bubble Sorts for different keys */

static void sortByGPA(void) {
    for (int i = 0; i < studentCount - 1; i++) {
        for (int j = 0; j < studentCount - i - 1; j++) {
            if (students[j].gpa < students[j + 1].gpa) { /* descending */
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
    printf("Sorted by GPA (descending).\n");
}

static void sortByName(void) {
    for (int i = 0; i < studentCount - 1; i++) {
        for (int j = 0; j < studentCount - i - 1; j++) {
            if (strcmp(students[j].name, students[j + 1].name) > 0) {
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
    printf("Sorted by Name (A-Z).\n");
}

static void sortById(void) {
    for (int i = 0; i < studentCount - 1; i++) {
        for (int j = 0; j < studentCount - i - 1; j++) {
            if (students[j].id > students[j + 1].id) {
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
    printf("Sorted by ID (ascending).\n");
}

void searchSortMenu(void) {
    int choice;
    printf("\n--- Search & Sort Menu ---\n");
    printf("1. Search by ID\n");
    printf("2. Search by Name\n");
    printf("3. Sort by GPA (desc)\n");
    printf("4. Sort by Name (A-Z)\n");
    printf("5. Sort by ID (asc)\n");
    printf("0. Back\n");
    printf("Choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: searchById(); break;
        case 2: searchByName(); break;
        case 3: sortByGPA(); break;
        case 4: sortByName(); break;
        case 5: sortById(); break;
        case 0:
        default: break;
    }
}

/* ===================== Statistics & Reports ===================== */

static void classStatistics(void) {
    if (studentCount == 0) {
        printf("No students to analyze.\n");
        return;
    }

    float sum = 0.0f;
    float *gpas = malloc(studentCount * sizeof(float));
    if (!gpas) {
        printf("Memory error.\n");
        return;
    }

    for (int i = 0; i < studentCount; i++) {
        gpas[i] = students[i].gpa;
        sum += students[i].gpa;
    }

    float average = sum / studentCount;

    /* sort gpas for median (Bubble sort) */
    for (int i = 0; i < studentCount - 1; i++) {
        for (int j = 0; j < studentCount - i - 1; j++) {
            if (gpas[j] > gpas[j + 1]) {
                float tmp = gpas[j];
                gpas[j] = gpas[j + 1];
                gpas[j + 1] = tmp;
            }
        }
    }

    float median;
    if (studentCount % 2 == 1) {
        median = gpas[studentCount / 2];
    } else {
        median = (gpas[studentCount / 2 - 1] + gpas[studentCount / 2]) / 2.0f;
    }

    float minGPA = gpas[0];
    float maxGPA = gpas[studentCount - 1];

    printf("Class Average GPA: %.2f\n", average);
    printf("Median GPA:        %.2f\n", median);
    printf("Lowest GPA:        %.2f\n", minGPA);
    printf("Highest GPA:       %.2f\n", maxGPA);

    free(gpas);
}

static void topNStudents(void) {
    if (studentCount == 0) {
        printf("No students.\n");
        return;
    }

    int N;
    printf("Enter N (top N students): ");
    scanf("%d", &N);
    if (N <= 0) {
        printf("Invalid N.\n");
        return;
    }
    if (N > studentCount) N = studentCount;

    sortByGPA(); /* sorts in place, descending */
    printf("Top %d student(s):\n", N);
    for (int i = 0; i < N; i++) {
        displayStudent(&students[i]);
    }
}

static void topStudentPerCourse(void) {
    char course[MAX_COURSE];
    printf("Enter course name to find top student (exact): ");
    scanf("%s", course);

    int found = 0;
    Student *top = NULL;
    for (int i = 0; i < studentCount; i++) {
        if (strcmp(students[i].course, course) == 0) {
            if (!found || students[i].gpa > top->gpa) {
                top = &students[i];
                found = 1;
            }
        }
    }

    if (!found) {
        printf("No students found for that course.\n");
    } else {
        printf("Top student in %s:\n", course);
        displayStudent(top);
    }
}

static void courseAverageGPA(void) {
    char course[MAX_COURSE];
    printf("Enter course name for average GPA: ");
    scanf("%s", course);

    float sum = 0.0f;
    int count = 0;
    for (int i = 0; i < studentCount; i++) {
        if (strcmp(students[i].course, course) == 0) {
            sum += students[i].gpa;
            count++;
        }
    }

    if (count == 0) {
        printf("No students found for that course.\n");
    } else {
        printf("Average GPA for course %s: %.2f\n",
               course, sum / count);
    }
}

void reportsMenu(void) {
    int choice;
    printf("\n--- Reports & Analytics ---\n");
    printf("1. Class statistics (avg, median, min, max GPA)\n");
    printf("2. Top N students overall\n");
    printf("3. Top student per course\n");
    printf("4. Course-wise average GPA\n");
    printf("0. Back\n");
    printf("Choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: classStatistics(); break;
        case 2: topNStudents(); break;
        case 3: topStudentPerCourse(); break;
        case 4: courseAverageGPA(); break;
        case 0:
        default: break;
    }
}