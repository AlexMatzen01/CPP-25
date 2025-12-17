# Chapter 8: Student Grades Management System

## Project Overview
A comprehensive menu-driven program to manage student grades using arrays, functions, file I/O, search, and sorting algorithms.

## Features

### 1. Data Input and Storage
- Input student names and grades for multiple assignments
- Store data in arrays
- Validate inputs (grades must be 0-100)

### 2. File I/O
- **Save**: Save student data (names, grades, averages) to a file (`student_data.txt`)
- **Load**: Load student data from a file
- Error handling for file not found and invalid data

### 3. Data Analysis
- Calculate average grade for each student
- Identify highest and lowest averages
- Calculate class average
- Display data in formatted table
- Highlight students with highest/lowest averages

### 4. Search Functionality
- Binary search to find a student by name
- Display grades and average if found
- Automatically sorts data alphabetically before search

### 5. Sorting Functionality
- **Quick Sort**: Sort students by name alphabetically
- **Merge Sort**: Sort students by average grade (descending)
- User selectable sorting criterion

### 6. User Interface
Menu-driven interface with options:
1. Input grades
2. View grades
3. Save data
4. Load data
5. Analyze grades
6. Search student
7. Sort data
0. Exit

## Compilation

```bash
g++ -o student_grades student_grades.cpp
```

Or with warnings:
```bash
g++ -Wall -Wextra -o student_grades student_grades.cpp
```

## Usage

```bash
./student_grades
```

Follow the menu prompts to:
- Add student data
- View formatted grade tables
- Save/load data from files
- Analyze class performance
- Search for specific students
- Sort data by name or average

## Sample Workflow

1. **Input Grades**: Enter student names and assignment grades
2. **View Grades**: See all data in a formatted table
3. **Analyze Grades**: View statistics (highest, lowest, class average)
4. **Save Data**: Persist data to file
5. **Search Student**: Find a specific student's information
6. **Sort Data**: Organize by name or performance

## Implementation Details

### Algorithms Used
- **Binary Search**: O(log n) search for student names
- **Quick Sort**: O(n log n) average case for alphabetical sorting
- **Merge Sort**: O(n log n) guaranteed for grade sorting

### Data Structures
- Parallel arrays for student names, grades, and averages
- Maximum 50 students with up to 5 assignments each

### Error Handling
- Input validation for grades (0-100 range)
- File I/O error handling
- Empty data checks

## Team Collaboration Notes

This project is designed for team collaboration:
- **Main developer**: Implements `main()` and menu system
- **Team members**: Each implements specific functions (search, sort, file I/O, etc.)
- Function parameters and return values are clearly defined
- Modular design allows independent development and testing

## File Format

The data file (`student_data.txt`) uses the following format:
```
<num_students> <num_assignments>
<student_name_1>
<grade1> <grade2> ... <gradeN>
<average1>
<student_name_2>
<grade1> <grade2> ... <gradeN>
<average2>
...
```

## Example Output

```
=== Student Performance Table ===
Name                Average     Status
----------------------------------------
Alice Johnson       95.60       ** HIGHEST **
Bob Smith           87.40       
Carol Davis         92.20       
David Wilson        78.80       ** LOWEST **
```
