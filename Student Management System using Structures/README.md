# Student Management System (C, Structures)

Interactive console app that manages student records using C structures and a dynamic array. Supports CRUD, search/sort, simple analytics, and file persistence to `students.txt`.

- Add, display, update, delete students with GPA tracking.
- Search by ID/name; sort by GPA, name, or ID.
- Reports: class stats (avg/median/min/max), top N overall, top per course, course average GPA.
- Auto-loads/saves data from `students.txt` in the working directory.

## Requirements
- GCC or Clang with C11 support.
- POSIX environment for standard C library and `stdio`.

## Build
From the `Student Management System using Structures` directory:
```sh
gcc -std=c11 -Wall -Wextra -pedantic main.c student.c -o student_mgmt
```

## Run
```sh
./student_mgmt
```

Follow the menu prompts:
- Names/courses are read as single tokens (use underscores instead of spaces, e.g., `John_Doe`).
- Grades are entered as numbers 0–100.
- Data is auto-saved to `students.txt` after changes; you can force save/load via menu options 7/8.

## Data File
- Stored as plain text `students.txt` in the same directory.
- Loaded on startup; if missing, the app starts empty.
- Re-running the program continues from the saved file; delete/rename the file to start fresh.

## Notes
- Sorting uses simple in-place bubble sorts (fine for small datasets).
- GPA is the arithmetic mean of entered grades.
- Basic input validation is present; malformed input can leave stdin in an unexpected state—restart if inputs get out of sync.
