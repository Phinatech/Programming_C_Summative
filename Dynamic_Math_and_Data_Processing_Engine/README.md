# Dynamic Math & Data Processing Engine

Interactive C console app that demonstrates dynamic memory management and function-pointer dispatch over an integer dataset. You can load/save numbers, mutate the dataset, and run math/utility operations selected at runtime.

- Dynamic array with add/delete and dataset printing.
- Function-pointer menu for sum/average/min/max, ascending/descending sort, and search.
- File I/O helpers to load from `input.txt` and save to `output.txt`.
- Graceful handling of empty datasets and basic input validation.

## Requirements
- GCC or Clang with C11 support.

## Build
From the `Dynamic_Math_and_Data_Processing_Engine` directory:
```sh
gcc -std=c11 -Wall -Wextra -pedantic main.c engine.c -o engine
```

## Run
```sh
./engine
```

Menu options:
- Add/delete values (dynamic realloc).
- Show current dataset.
- Load numbers from `input.txt` (one integer per line).
- Save current dataset to `output.txt`.
- Run an operation by choosing its index (0–6) via the function-pointer table.

## Notes
- Sorting uses in-place bubble sort (fine for small lists).
- `input.txt`/`output.txt` are relative to the working directory; overwrite on save.
- If stdin gets out of sync after bad input, restart the program to reset state.
