# Student Grades Management System

This is a simple C++ console application to manage student grades. It allows you to:

- Input student names and their grades
- View all students and their grades
- Save and load student data from a file
- Analyze grades to find the highest and lowest averages
- Search for a student by name
- Sort students by name or by average grade

## How to Compile

Use the provided Makefile:

```
make student_grades
```

Or compile manually:

```
g++ -std=c++11 -Wall -o student_grades student_grades.cpp
```

## How to Run

```
./student_grades
```

## Menu Options

1. Input grades
2. View grades
3. Save Grades to File
4. Load grades from File
5. Analyze Grades
6. Search for a student by Name
7. Sort Data
8. Exit

## File Structure

- `student_grades.cpp` - Main program source code
- `Makefile` - For building the program
- `data/` - Directory for data files

---
