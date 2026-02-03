#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>

// ==================== Date Class ====================
class Date {
private:
    int day_;
    int month_;
    int year_;

public:
    Date(int day = 1, int month = 1, int year = 2000)
        : day_(day), month_(month), year_(year) {
        if (!isValid()) {
            throw std::invalid_argument("Invalid date");
        }
    }

    // Getters
    int getDay() const { return day_; }
    int getMonth() const { return month_; }
    int getYear() const { return year_; }

    // Validation
    static bool isLeapYear(int year) {
        return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
    }

    static int daysInMonth(int month, int year) {
        const int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if (month == 2 && isLeapYear(year)) return 29;
        return (month >= 1 && month <= 12) ? days[month - 1] : 0;
    }

    bool isValid() const {
        return year_ >= 1900 &&
            month_ >= 1 && month_ <= 12 &&
            day_ >= 1 && day_ <= daysInMonth(month_, year_);
    }

    // Convertion
    std::string toString() const {
        std::ostringstream oss;
        oss << std::setfill('0')
            << std::setw(4) << year_ << "-"
            << std::setw(2) << month_ << "-"
            << std::setw(2) << day_;
        return oss.str();
    }

    // Operators
    bool operator==(const Date& other) const {
        return day_ == other.day_ && month_ == other.month_ && year_ == other.year_;
    }

    bool operator<(const Date& other) const {
        if (year_ != other.year_) return year_ < other.year_;
        if (month_ != other.month_) return month_ < other.month_;
        return day_ < other.day_;
    }

    bool operator>(const Date& other) const { return other < *this; }
    bool operator<=(const Date& other) const { return !(other < *this); }
    bool operator>=(const Date& other) const { return !(*this < other); }
    bool operator!=(const Date& other) const { return !(*this == other); }

    // Useful methods
    Date addDays(int days) const {
        Date result = *this;
        while (days-- > 0) {
            result.day_++;
            if (result.day_ > daysInMonth(result.month_, result.year_)) {
                result.day_ = 1;
                result.month_++;
                if (result.month_ > 12) {
                    result.month_ = 1;
                    result.year_++;
                }
            }
        }
        return result;
    }

    int daysDifference(const Date& other) const {
        // Siplified version
        int days1 = year_ * 365 + month_ * 30 + day_;
        int days2 = other.year_ * 365 + other.month_ * 30 + other.day_;
        return days1 - days2;
    }

    friend std::ostream& operator<<(std::ostream& os, const Date& date) {
        os << date.toString();
        return os;
    }
};

// ==================== Book Class ====================
class Book {
private:
    std::string title_;
    std::string author_;
    std::string isbn_;
    bool isBorrowed_;
    std::unique_ptr<Date> borrowDate_;
    std::unique_ptr<Date> dueDate_;
    double finePerDay_;

public:
    Book(const std::string& title, const std::string& author,
        const std::string& isbn, double finePerDay = 0.5)
        : title_(title), author_(author), isbn_(isbn),
        isBorrowed_(false), finePerDay_(finePerDay) {
    }

    // Getters
    const std::string& getTitle() const { return title_; }
    const std::string& getAuthor() const { return author_; }
    const std::string& getIsbn() const { return isbn_; }
    bool isBorrowed() const { return isBorrowed_; }
    const Date* getBorrowDate() const { return borrowDate_.get(); }
    const Date* getDueDate() const { return dueDate_.get(); }
    double getFinePerDay() const { return finePerDay_; }

    // Actions
    bool borrow(const Date& borrowDate, int loanDays = 14) {
        if (!isBorrowed_) {
            isBorrowed_ = true;
            borrowDate_ = std::make_unique<Date>(borrowDate);
            dueDate_ = std::make_unique<Date>(borrowDate.addDays(loanDays));
            return true;
        }
        return false;
    }

    bool returnBook(const Date& returnDate) {
        if (isBorrowed_) {
            isBorrowed_ = false;
            borrowDate_.reset();
            dueDate_.reset();
            return true;
        }
        return false;
    }

    // Checking
    bool isOverdue(const Date& currentDate) const {
        return isBorrowed_ && dueDate_ && currentDate > *dueDate_;
    }

    double calculateFine(const Date& currentDate) const {
        if (!isOverdue(currentDate)) return 0.0;
        int overdueDays = currentDate.daysDifference(*dueDate_);
        return overdueDays * finePerDay_;
    }

    // Information
    std::string getInfo() const {
        std::ostringstream oss;
        oss << "  " << title_ << "\n"
            << "   Author: " << author_ << "\n"
            << "   ISBN: " << isbn_ << "\n"
            << "   Status: " << (isBorrowed_ ? "Borrowed" : "Available");

        if (isBorrowed_ && dueDate_) {
            oss << "\n   Due Date: " << *dueDate_;
        }
        return oss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Book& book) {
        os << book.getInfo();
        return os;
    }
};

// ==================== LibraryMember Class ====================
class LibraryMember {
private:
    std::string name_;
    int memberId_;
    std::vector<Book*> borrowedBooks_;
    double totalFines_;

public:
    LibraryMember(const std::string& name, int id)
        : name_(name), memberId_(id), totalFines_(0.0) {
    }

    // Getters
    const std::string& getName() const { return name_; }
    int getMemberId() const { return memberId_; }
    double getTotalFines() const { return totalFines_; }
    size_t getBorrowedCount() const { return borrowedBooks_.size(); }

    std::vector<Book*> getBorrowedBooks() const {
        return borrowedBooks_;
    }

    // Actions
    bool borrowBook(Book* book, const Date& borrowDate, int loanDays = 14) {
        if (!book || borrowedBooks_.size() >= 5) return false; // Maximum 5 books

        if (book->borrow(borrowDate, loanDays)) {
            borrowedBooks_.push_back(book);
            return true;
        }
        return false;
    }

    bool returnBook(Book* book, const Date& returnDate) {
        auto it = std::find(borrowedBooks_.begin(), borrowedBooks_.end(), book);
        if (it != borrowedBooks_.end() && book->returnBook(returnDate)) {
            borrowedBooks_.erase(it);

            // Count fines
            if (book->isOverdue(returnDate)) {
                totalFines_ += book->calculateFine(returnDate);
            }
            return true;
        }
        return false;
    }

    void payFine(double amount) {
        if (amount > 0 && amount <= totalFines_) {
            totalFines_ -= amount;
        }
    }

    // Information
    std::string getInfo() const {
        std::ostringstream oss;
        oss << "Member: " << name_ << " (ID: " << memberId_ << ")\n"
            << "   Borrowed books: " << borrowedBooks_.size() << "\n"
            << "   Total fines: $" << std::fixed << std::setprecision(2) << totalFines_;
        return oss.str();
    }
};

// ==================== Library Class ====================
class Library {
private:
    std::vector<std::unique_ptr<Book>> books_;
    std::vector<std::unique_ptr<LibraryMember>> members_;
    std::string dataFile_;
    Date currentDate_;

    // Assistant functions
    bool isIsbnUnique(const std::string& isbn) const {
        return std::none_of(books_.begin(), books_.end(),
            [&](const auto& book) { return book->getIsbn() == isbn; });
    }

    bool isMemberIdUnique(int id) const {
        return std::none_of(members_.begin(), members_.end(),
            [&](const auto& member) { return member->getMemberId() == id; });
    }

public:
    Library(const std::string& dataFile = "library_data.txt")
        : dataFile_(dataFile), currentDate_(Date(28, 1, 2024)) {
    }

    // Controlling books
    bool addBook(const std::string& title, const std::string& author,
        const std::string& isbn) {
        if (!isIsbnUnique(isbn)) {
            std::cerr << " ISBN already exists: " << isbn << std::endl;
            return false;
        }

        books_.push_back(std::make_unique<Book>(title, author, isbn));
        std::cout << " Book added: " << title << std::endl;
        return true;
    }

    bool removeBook(const std::string& isbn) {
        auto it = std::remove_if(books_.begin(), books_.end(),
            [&](const auto& book) { return book->getIsbn() == isbn; });

        if (it != books_.end()) {
            books_.erase(it, books_.end());
            std::cout << " Book removed: " << isbn << std::endl;
            return true;
        }
        return false;
    }

    // Member control
    bool addMember(const std::string& name, int id) {
        if (!isMemberIdUnique(id)) {
            std::cerr << " Member ID already exists: " << id << std::endl;
            return false;
        }

        members_.push_back(std::make_unique<LibraryMember>(name, id));
        std::cout << " Member added: " << name << std::endl;
        return true;
    }

    // Searching
    Book* findBookByIsbn(const std::string& isbn) const {
        auto it = std::find_if(books_.begin(), books_.end(),
            [&](const auto& book) { return book->getIsbn() == isbn; });
        return (it != books_.end()) ? it->get() : nullptr;
    }

    LibraryMember* findMemberById(int id) const {
        auto it = std::find_if(members_.begin(), members_.end(),
            [&](const auto& member) { return member->getMemberId() == id; });
        return (it != members_.end()) ? it->get() : nullptr;
    }

    std::vector<Book*> findBooksByTitle(const std::string& title) const {
        std::vector<Book*> results;
        for (const auto& book : books_) {
            if (book->getTitle().find(title) != std::string::npos) {
                results.push_back(book.get());
            }
        }
        return results;
    }

    // Borrow/Return
    bool borrowBook(const std::string& isbn, int memberId, int loanDays = 14) {
        Book* book = findBookByIsbn(isbn);
        LibraryMember* member = findMemberById(memberId);

        if (!book || !member) {
            std::cerr << " Book or member not found" << std::endl;
            return false;
        }

        if (member->borrowBook(book, currentDate_, loanDays)) {
            std::cout << " " << member->getName()
                << " borrowed: " << book->getTitle() << std::endl;
            return true;
        }

        std::cerr << " Cannot borrow book" << std::endl;
        return false;
    }

    bool returnBook(const std::string& isbn, int memberId) {
        Book* book = findBookByIsbn(isbn);
        LibraryMember* member = findMemberById(memberId);

        if (!book || !member) {
            std::cerr << " Book or member not found" << std::endl;
            return false;
        }

        if (member->returnBook(book, currentDate_)) {
            std::cout << " " << member->getName()
                << " returned: " << book->getTitle() << std::endl;
            return true;
        }

        std::cerr << " Cannot return book" << std::endl;
        return false;
    }

    // Status
    void printStatus() const {
        std::cout << "\n LIBRARY STATUS \n";
        std::cout << "=====================\n";
        std::cout << "Total books: " << books_.size() << "\n";
        std::cout << "Total members: " << members_.size() << "\n";

        int borrowedCount = 0;
        double totalFines = 0.0;

        for (const auto& book : books_) {
            if (book->isBorrowed()) borrowedCount++;
        }

        for (const auto& member : members_) {
            totalFines += member->getTotalFines();
        }

        std::cout << "Borrowed books: " << borrowedCount << "\n";
        std::cout << "Total fines due: $"
            << std::fixed << std::setprecision(2) << totalFines << "\n";
    }

    void listAllBooks() const {
        std::cout << "\n ALL BOOKS \n";
        std::cout << "================\n";
        for (const auto& book : books_) {
            std::cout << *book << "\n---\n";
        }
    }

    void listAllMembers() const {
        std::cout << "\n ALL MEMBERS \n";
        std::cout << "==================\n";
        for (const auto& member : members_) {
            std::cout << member->getInfo() << "\n---\n";
        }
    }

    // File actions
    void saveToFile() const {
        std::ofstream out(dataFile_);
        if (!out) {
            std::cerr << " Cannot open file for writing: " << dataFile_ << std::endl;
            return;
        }

        // Books
        out << "[BOOKS]\n";
        for (const auto& book : books_) {
            out << book->getTitle() << "|"
                << book->getAuthor() << "|"
                << book->getIsbn() << "|"
                << (book->isBorrowed() ? "1" : "0") << "\n";
        }

        // Members
        out << "[MEMBERS]\n";
        for (const auto& member : members_) {
            out << member->getName() << "|"
                << member->getMemberId() << "|"
                << member->getTotalFines() << "\n";
        }

        std::cout << " Data saved to: " << dataFile_ << std::endl;
    }

    // Change date(for test)
    void setCurrentDate(const Date& date) { currentDate_ = date; }
    const Date& getCurrentDate() const { return currentDate_; }
};

// ==================== Main Program ====================
void runLibraryDemo() {
    std::cout << "  LIBRARY MANAGEMENT SYSTEM \n";
    std::cout << "================================\n\n";

    Library library;

    // Add books
    library.addBook("The C++ Programming Language", "Bjarne Stroustrup", "9780321563842");
    library.addBook("Effective Modern C++", "Scott Meyers", "9781491903995");
    library.addBook("Clean Code", "Robert C. Martin", "9780132350884");

    // Add members
    library.addMember("Anna", 1001);
    library.addMember("David", 1002);
    library.addMember("Michael", 1003);

    // Show status
    library.printStatus();

    // Borrow books
    std::cout << "\n BORROWING BOOKS \n";
    library.borrowBook("9780321563842", 1001, 7); // short time
    library.borrowBook("9781491903995", 1002);

    // Show books
    library.listAllBooks();

    // Show members
    library.listAllMembers();

    // Return book
    std::cout << "\n RETURNING BOOKS \n";

    // Change date(for test) - after 10 days
    Date futureDate = library.getCurrentDate().addDays(10);
    library.setCurrentDate(futureDate);

    library.returnBook("9780321563842", 1001); 

    // Final status
    std::cout << "\n FINAL STATUS \n";
    library.printStatus();

    // Save to file
    library.saveToFile();

    std::cout << "\n DEMO COMPLETED SUCCESSFULLY! \n";
}

int main() {
    try {
        runLibraryDemo();
    }
    catch (const std::exception& e) {
        std::cerr << "\n ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
