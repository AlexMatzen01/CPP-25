# Student Grades Management System

A comprehensive C++ program for managing student grades with advanced sorting, searching, and analysis capabilities.

## Project Overview

This is a menu-driven console application that allows users to:
- Input and store student grades
- Save and load data from files
- Analyze grade statistics
- Search for students by name
- Sort data by name or average grade

## Features

### Core Functionalities

#### 1. Data Input and Storage
- Input student names and multiple grades per student
- Store data in arrays with support for up to 100 students
- Each student can have up to 10 grades
- Automatic grade validation (0-100 range)

#### 2. File I/O
- **Save**: Persist student data to text files
- **Load**: Restore data from previously saved files
- Error handling for file operations (file not found, invalid data, etc.)
- Files stored in `data/` directory

#### 3. Data Analysis
- Calculate each student's average grade automatically
- Identify students with highest and lowest averages
- Display data in formatted tabular output
- Color-coded highlighting for top and bottom performers

#### 4. Search Functionality
- Binary search implementation for efficient student lookup
- Search by student name
- Display complete grade information when found
- Automatic sorting before search to ensure binary search works correctly

#### 5. Sorting Functionality
- **Quick Sort**: Sort students alphabetically by name
- **Merge Sort**: Sort students by average grade in descending order
- User can choose sorting criterion via menu
- Both algorithms preserve all student data during sorting

## Algorithms Implemented

### Quick Sort (Alphabetical Sorting)
- **Time Complexity**: O(n log n) average case
- **Purpose**: Sorts student names alphabetically
- **Implementation**: Recursive partitioning with pivot selection

### Merge Sort (Average Grade Sorting)
- **Time Complexity**: O(n log n) guaranteed
- **Purpose**: Sorts students by average in descending order
- **Implementation**: Divide-and-conquer with merge operation

### Binary Search
- **Time Complexity**: O(log n)
- **Purpose**: Efficiently find students by name
- **Requirement**: Data must be sorted alphabetically first

## Menu Options

```
1. Input grades
2. View grades
3. Save Grades to File
4. Load grades from File
5. Analyze Grades
6. Search for a student by Name
7. Sort Data
8. Exit
```

## Usage Instructions

### Compilation

Using the provided Makefile:
```bash
make student_grades
```

Or manually:
```bash
g++ -std=c++11 -Wall -o student_grades student_grades.cpp
```

### Running the Program

```bash
./student_grades
```

### Sample Workflow

1. **Input Student Data**
   - Choose option 1
   - Enter student name
   - Enter number of grades
   - Input each grade (validated 0-100)

2. **View All Grades**
   - Choose option 2
   - See formatted table of all students

3. **Save Data**
   - Choose option 3
   - Enter filename (e.g., `grades.txt`)
   - Data saved to `data/grades.txt`

4. **Load Data**
   - Choose option 4
   - Enter filename to load
   - Previous data restored

5. **Analyze Grades**
   - Choose option 5
   - View statistics with highest/lowest highlighted

6. **Search for Student**
   - Choose option 6
   - Enter exact student name
   - View individual grades and average

7. **Sort Data**
   - Choose option 7
   - Select sort type:
     - 1: Alphabetical by name
     - 2: By average grade (descending)

## Sample Output

### Analysis Results
```
--- Grade Analysis ---
Student                       Average Grade
---------------------------------------------
Alice                         89.4
Bob                           72.6
Charlie                       95.2
---------------------------------------------
Highest Average: Charlie (95.2)
Lowest Average: Bob (72.6)
```

### Search Results
```
Enter the student's name to search: Charlie
Student found: Charlie
Grades: 90 94 98 92 96
Average: 95.2
```

### Sorted Data (Alphabetical)
```
--- Alphabetical Sort ---
Student                       Average Grade
---------------------------------------------
Alice                         89.4
Bob                           72.6
Charlie                       95.2
```

### Sorted Data (By Average)
```
--- Average Sort ---
Student                       Average Grade
---------------------------------------------
Charlie                       95.2
Alice                         89.4
Bob                           72.6
```

## File Format

The save file format is as follows:
```
<number_of_students>
<student_1_name>
<number_of_grades>
<grade1> <grade2> ... <gradeN>
<average>
<student_2_name>
...
```

Example:
```
3
Alice
5
85 90 88 92 91
89.2
Bob
4
70 75 72 73
72.5
Charlie
5
90 94 98 92 96
95.2
```

## Project Structure

```
chap8/
├── student_grades.cpp    # Main program source code
├── Makefile             # Build configuration
├── data/                # Directory for data files
│   ├── names.txt       # Sample names
│   └── grades.txt      # Saved student grades
└── README.md           # This file
```

## Team Development Workflow

### Phase 1: Planning ✓
- Defined roles and responsibilities
- Established file structure
- Created program flowchart
- Documented in README.md

### Phase 2: Implementation ✓
- Developed input/storage module
- Implemented file I/O with error handling
- Created analysis and search algorithms
- Built sorting algorithms (Quick Sort & Merge Sort)
- Designed menu-driven interface

### Phase 3: Integration ✓
- Combined all modules
- Ensured seamless data flow between components
- Resolved integration issues

### Phase 4: Testing ✓
- Unit tested each module
- System testing with sample data
- Edge case validation
- Error handling verification

### Phase 5: Documentation ✓
- Code comments added
- User guide created
- README documentation complete

## Technical Details

### Data Structures
- **Arrays**: Fixed-size arrays for maximum performance
- **Parallel Arrays**: Maintains data integrity across operations
- Student names: `string studentNames[MAX_STUDENTS]`
- Grades: `double studentGrades[MAX_STUDENTS][MAX_GRADES]`
- Averages: `double studentAverages[MAX_STUDENTS]`

### Input Validation
- Grade range: 0-100
- Maximum students: 100
- Maximum grades per student: 10
- File existence checks
- Error recovery mechanisms

### Memory Management
- Dynamic allocation in merge sort for temporary arrays
- Proper deallocation to prevent memory leaks
- Efficient array swapping in quick sort

## Known Limitations

1. Maximum 100 students (can be increased by changing `MAX_STUDENTS`)
2. Maximum 10 grades per student (can be increased by changing `MAX_GRADES`)
3. Binary search requires exact name match (case-sensitive)
4. Console-based interface only

## Future Enhancements

- Support for weighted grade calculations
- Statistical analysis (median, mode, standard deviation)
- Export to CSV format
- Graphical user interface
- Database integration
- Grade distribution visualization

## Contributors

This project was developed as part of C++ Programming Chapter 8 exercises, demonstrating:
- Array manipulation
- Searching algorithms
- Sorting algorithms
- File I/O operations
- Menu-driven program design

## License

Educational use - Part of C++ course materials

---

**Last Updated**: December 17, 2025
