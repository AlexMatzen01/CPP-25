# Student Grades Management System

This is a simple C++ console program to manage student grades. You can add students, enter their grades, view, save, load, analyze, search, and sort student data.

## How to Compile

Open a terminal in this folder and run:

```sh
g++ -std=c++17 -O2 -Wall student_grades.cpp -o student_grades.exe
```

## How to Run

```sh
./student_grades.exe
```
Or on Windows:
```sh
student_grades.exe
```

## Features
- Add students and their grades
- View all students and grades
- Save/load data to/from a file
- Analyze grades (find highest/lowest average)
- Search for a student by name
- Sort students by name or average

## Data Files
Saved files are stored in the `data/` folder. Example: `data/grades.txt`

## Program Flowchart

```mermaid
flowchart TD
    Start([Start])
    Menu{Show Menu\nGet Choice}
    Add[Input Grades]
    AddName[Enter Student Name]
    AddGrades[Enter Number of Grades]
    AddEachGrade[Enter Each Grade]
    AddDone[Student Added]
    View[View Grades]
    ViewCheck{Any Students?}
    ViewNone[Show 'No Data']
    ViewShow[Display Table]
    Save[Save Grades to File]
    SaveCheck{Any Students?}
    SaveNone[Show 'No Data']
    SaveFile[Prompt Filename]
    SaveDone[Write File]
    Load[Load Grades from File]
    LoadFile[Prompt Filename]
    LoadOpen{File Exists?}
    LoadFail[Show Error]
    LoadDone[Read Data]
    Analyze[Analyze Grades]
    AnalyzeCheck{Any Students?}
    AnalyzeNone[Show 'No Data']
    AnalyzeCalc[Calculate Averages]
    AnalyzeShow[Show Analysis]
    Search[Search Student]
    SearchCheck{Any Students?}
    SearchNone[Show 'No Data']
    SearchPrompt[Prompt Name]
    SearchSort[Sort by Name]
    SearchBin[Binary Search]
    SearchFound[Show Student]
    SearchNotFound[Show Not Found]
    Sort[Sort Data]
    SortCheck{Any Students?}
    SortNone[Show 'No Data']
    SortPrompt[Prompt Sort Type]
    SortAlpha[Sort by Name]
    SortAvg[Sort by Average]
    SortShow[Show Sorted]
    Exit([Exit])
    Invalid[Invalid Choice]

    Start --> Menu
    Menu -->|1| Add
    Add --> AddName --> AddGrades --> AddEachGrade --> AddDone --> Menu
    Menu -->|2| View
    View --> ViewCheck
    ViewCheck -->|No| ViewNone --> Menu
    ViewCheck -->|Yes| ViewShow --> Menu
    Menu -->|3| Save
    Save --> SaveCheck
    SaveCheck -->|No| SaveNone --> Menu
    SaveCheck -->|Yes| SaveFile --> SaveDone --> Menu
    Menu -->|4| Load
    Load --> LoadFile --> LoadOpen
    LoadOpen -->|No| LoadFail --> Menu
    LoadOpen -->|Yes| LoadDone --> Menu
    Menu -->|5| Analyze
    Analyze --> AnalyzeCheck
    AnalyzeCheck -->|No| AnalyzeNone --> Menu
    AnalyzeCheck -->|Yes| AnalyzeCalc --> AnalyzeShow --> Menu
    Menu -->|6| Search
    Search --> SearchCheck
    SearchCheck -->|No| SearchNone --> Menu
    SearchCheck -->|Yes| SearchPrompt --> SearchSort --> SearchBin
    SearchBin -->|Found| SearchFound --> Menu
    SearchBin -->|Not Found| SearchNotFound --> Menu
    Menu -->|7| Sort
    Sort --> SortCheck
    SortCheck -->|No| SortNone --> Menu
    SortCheck -->|Yes| SortPrompt
    SortPrompt -->|1| SortAlpha --> SortShow --> Menu
    SortPrompt -->|2| SortAvg --> SortShow --> Menu
    Menu -->|8| Exit
    Menu -->|other| Invalid --> Menu
```
