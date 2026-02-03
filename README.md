# Library-Management-System-CPP
A C++ library management system demonstrating OOP principles
# Library Management System - C++

A console-based library management system demonstrating Object-Oriented Programming principles in C++.

## ✨ Features
- 📚 **Book Management**: Add, remove, search books by ISBN/title/author
- 👥 **Member Management**: Register members, track borrowed books
- 📅 **Date Handling**: Full date validation with leap year support
- 💰 **Fine Calculation**: Automatic fine calculation for overdue books
- 💾 **File Persistence**: Save/Load library data to/from file
- 🎯 **Object-Oriented Design**: Clean class hierarchy and encapsulation

## 🛠️ Technologies Used
- C++11/14
- Standard Template Library (STL)
- Object-Oriented Programming (OOP)
- RAII (Resource Acquisition Is Initialization) pattern
- Smart Pointers (`std::unique_ptr`)
- Exception Handling
- File I/O operations

## 📁 Project Structure
LibraryManagementSystem/
├── library.cpp # Main source code
├── README.md # Project documentation
├── .gitignore # Git ignore rules
└── library_data.txt # Generated data file

## 🚀 How to Compile & Run

### Linux/macOS
```bash
# Compile
g++ -std=c++14 -o library library.cpp

# Run
./library
# Compile
g++ -std=c++14 -o library.exe library.cpp

# Run
library.exe
📊 Sample Output

🏛️  LIBRARY MANAGEMENT SYSTEM 🏛️
================================

📊 LIBRARY STATUS 📊
=====================
Total books: 3
Total members: 3
Borrowed books: 2
Total fines due: $0.00
