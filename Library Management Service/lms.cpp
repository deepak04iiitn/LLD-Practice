#include <bits/stdc++.h>
using namespace std;


enum class UserRole {
    MEMBER,
    LIBRARIAN
};

enum class BookStatus {
    OVER,
    AVAILABLE
};

enum class BookCopyStatus {
    AVAILABLE,
    ISSUED
};

enum class BookCategory {
    SCIENCE,
    HORROR,
    FICTION,
    AI
};

enum class BorrowStatus {
    BORROWED,
    RETURNED
};

enum class MembershipStatus {
    ACTIVE,
    ENDED
};

enum class FineStatus {
    PAID,
    UNPAID
};

enum class ReservationStatus {
    RESERVED,
    FULFILLED
};


class User {
    private:
        UserRole role;
        int userid;
        string name;
        string email;
        string phone;

    public:
        User(UserRole role, int userid, string name, string email, string phone) : 
            role(role), userid(userid), name(name), email(email), phone(phone) {};

        UserRole getRole() {
            return role;
        }

        int getUserid() {
            return userid;
        }

        string getName() {
            return name;
        }

        string getEmail() {
            return email;
        }

        string getPhone() {
            return phone;
        }
};


class Author {
    private:
        string name;
        int id;

    public:
        Author(int id, string name) : 
            id(id), name(name) {};

        int getAuthorid() {
            return id;
        }

        string getAuthorName() {
            return name;
        }
};


class BookCopy;


class Book {
    private:
        int bookid;
        string title;
        string isbn;
        BookCategory category;
        vector<Author*>authors;
        BookStatus status;
        vector<BookCopy*>bookcopies;
        int numberOfCopies;

    public:
        Book(int id, string title, string isbn, BookCategory category, int copies) : 
            bookid(id), title(title), isbn(isbn), category(category), numberOfCopies(copies), status(BookStatus::AVAILABLE) {};

        void addAuthor(Author* author) {
            authors.push_back(author);
        }

        void addBookCopy(BookCopy* copy) {
            bookcopies.push_back(copy);
        }

        bool removeBookCopy(int copyId); // defined after BookCopy is fully declared

        void decreaseNumberOfCopies() {
            if(numberOfCopies > 0) numberOfCopies--;
        }

        void increaseNumberOfCopies(int num) {
            numberOfCopies += num;
        }

        void updateBookStatus(BookStatus st) {
            status = st;
        }

        int getBookid() {
            return bookid;
        }

        string getTitle() {
            return title;
        }

        string getIsbn() {
            return isbn;
        }

        BookCategory getBookCategory() {
            return category;
        }

        BookStatus getBookStatus() {
            return status;
        }

        vector<Author*> getAuthors() {
            return authors;
        }

        vector<BookCopy*> getBookCopies() {
            return bookcopies;
        }

        int getNumberOfCopies() {
            return numberOfCopies;
        }

};


class BookCopy {
    private:
        int copyId;
        BookCopyStatus status;
        Book* book;

    public:
        BookCopy(int copyId, Book* book) : 
            copyId(copyId), book(book), status(BookCopyStatus::AVAILABLE) {};

        void updateBookCopyStatus(BookCopyStatus st) {
            status = st;
        }

        int getCopyId() {
            return copyId;
        }

        Book* getBook() {
            return book;
        }

        BookCopyStatus getBookCopyStatus() {
            return status;
        }
};


inline bool Book::removeBookCopy(int copyId) {
    for(auto it = bookcopies.begin(); it != bookcopies.end(); ++it) {
        if((*it) -> getCopyId() == copyId) {
            bookcopies.erase(it);
            return true;
        }
    }
    return false;
}


class Record {
    private:
        int recordId;
        User* member;
        BookCopy* bookcopy;
        time_t issueDate;
        time_t dueDate;
        time_t returnDate;
        BorrowStatus status;

    public:
        Record(int id, User* member, BookCopy* bookcopy, time_t issueDate, time_t dueDate) : 
            recordId(id), member(member), bookcopy(bookcopy), issueDate(issueDate), dueDate(dueDate), returnDate(0), status(BorrowStatus::BORROWED) {}

        int getRecordId() {
            return recordId;
        }

        User* getMember() {
            return member;
        }

        BookCopy* getBookCopy() {
            return bookcopy;
        }

        time_t getIssueDate() {
            return issueDate;
        }

        time_t getDueDate() {
            return dueDate;
        }

        time_t getReturnDate() {
            return returnDate;
        }

        BorrowStatus getStatus() {
            return status;
        }

        void returnBook(time_t date) {
            returnDate = date;
            status = BorrowStatus::RETURNED;
        }
};


class Fine {
    private:
        int fineid;
        Record* record;
        double amount;
        FineStatus status;

    public:
        Fine(int id, Record* record, double amt)
            : fineid(id), record(record), amount(amt), status(FineStatus::UNPAID) {};

        void updateFineStatus(FineStatus st) {
            status = st;
        }

        int getFineid() {
            return fineid;
        }

        Record* getRecord() {
            return record;
        }

        double getAmount() {
            return amount;
        }

        FineStatus getFineStatus() {
            return status;
        }
};


class Reservation {
    private:
        int reservationid;
        User* member;
        Book* book;
        ReservationStatus status;

    public:
        Reservation(int id, User* member, Book* book) : 
            reservationid(id), member(member), book(book), status(ReservationStatus::RESERVED) {};

        void updateReservationStatus(ReservationStatus st) {
            status = st;
        }
        
        int getReservationid() {
            return reservationid;
        }

        User* getReservationMember() {
            return member;
        }

        Book* getReservedBook() {
            return book;
        }

        ReservationStatus getReservationStatus() {
            return status;
        }
        
};  


class FineManagementService {
    private:
        static const int FINE_PER_DAY = 10;

        int fineCounter = 1;
        unordered_map<int, Fine*> fineById;
        unordered_map<int, vector<Fine*>> finesByMemberId;

        static long calcOverdueDays(time_t dueDate, time_t returnDate) {
            if(returnDate <= dueDate) return 0;
            long secondsLate = (long)(returnDate - dueDate);
            return (secondsLate + 24 * 60 * 60 - 1) / (24 * 60 * 60);
        }

    public:
        FineManagementService() {};

        // Called right after a Record is marked returned. Returns nullptr if not overdue.
        Fine* createFineIfOverdue(Record* record) {
            long overdueDays = calcOverdueDays(record -> getDueDate(), record -> getReturnDate());
            if(overdueDays == 0) return nullptr;

            double amount = overdueDays * FINE_PER_DAY;
            Fine* fine = new Fine(fineCounter++, record, amount);

            fineById[fine -> getFineid()] = fine;
            finesByMemberId[record -> getMember() -> getUserid()].push_back(fine);

            return fine;
        }

        double getOutstandingAmount(int memberId) {
            double total = 0;
            for(auto f : finesByMemberId[memberId]) {
                if(f -> getFineStatus() == FineStatus::UNPAID) total += f -> getAmount();
            }
            return total;
        }

        void viewOutstandingFine(int memberId) {
            cout << "Outstanding fine for member " << memberId << ": Rs. " << getOutstandingAmount(memberId) << endl;
        }

        void payFine(int fineId) {
            auto it = fineById.find(fineId);
            if(it == fineById.end()) {
                cout << "Fine " << fineId << " not found." << endl;
                return;
            }

            if(it -> second -> getFineStatus() == FineStatus::PAID) {
                cout << "Fine " << fineId << " is already paid." << endl;
                return;
            }

            it -> second -> updateFineStatus(FineStatus::PAID);
            cout << "Fine " << fineId << " of Rs. " << it -> second -> getAmount() << " paid. Thank you!" << endl;
        }
};


class MemberManagementService {
    private:
        int memberIdCounter = 1;
        unordered_map<int, User*>members;

    public:
        MemberManagementService() {};

        User* registerMember(string name, string email, string phone) {
            User* member = new User(UserRole::MEMBER, memberIdCounter++, name, email, phone);
            members[member -> getUserid()] = member;

            cout << "Member \"" << name << "\" registered with id " << member -> getUserid() << "." << endl;
            return member;
        }

        void addMember(int id, User* mem) {
            members[id] = mem;
        }

        void removeMember(int id) {
            members.erase(id);
        }

};


class SearchCriteria {
    public:
        string title;
        string isbn;
        string authorName;
        int authorId;
};


class RecordManagementService {
    private:
        unordered_map<int, vector<Record*>>recordsByMember;

        static string formatDate(time_t t) {
            char buf[20];
            strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));
            return string(buf);
        }

    public:
        RecordManagementService() {};

        void addRecord(int memberId, Record* record) {
            recordsByMember[memberId].push_back(record);
        }

        void displayRecordOfMember(int memberId) {
            auto it = recordsByMember.find(memberId);

            if(it == recordsByMember.end() || it -> second.empty()) {
                cout << "No borrowing history found for member " << memberId << "." << endl;
                return;
            }

            cout << "Borrowing history for member " << memberId << ":" << endl;
            for(auto record : it -> second) {
                Book* book = record -> getBookCopy() -> getBook();

                cout << "  Record #" << record -> getRecordId()
                     << " | Book: " << book -> getTitle()
                     << " | Copy: " << record -> getBookCopy() -> getCopyId()
                     << " | Issued: " << formatDate(record -> getIssueDate())
                     << " | Due: " << formatDate(record -> getDueDate());

                if(record -> getStatus() == BorrowStatus::RETURNED) {
                    cout << " | Returned: " << formatDate(record -> getReturnDate());
                } else {
                    cout << " | Status: BORROWED";
                }
                cout << endl;
            }
        }
};


class ReservationManagementService {
    private:
        int counter = 1;
        unordered_map<int, queue<User*>> reserveQueueByBookId;    // bookId -> members waiting, FIFO
        unordered_map<int, Book*> reservedBookByMember;           // memberId -> book they're waiting on
        unordered_map<int, Reservation*> reservationByMember;     // memberId -> their active Reservation

    public:
        ReservationManagementService() {};

        void reserveBook(Book* book, User* member) {
            if(book -> getNumberOfCopies() > 0) {
                cout << "Book \"" << book -> getTitle() << "\" has copies available, no need to reserve." << endl;
                return;
            }

            int memberId = member -> getUserid();
            if(reservedBookByMember.find(memberId) != reservedBookByMember.end()) {
                cout << "Member " << memberId << " already has an active reservation." << endl;
                return;
            }

            reserveQueueByBookId[book -> getBookid()].push(member);
            reservedBookByMember[memberId] = book;

            Reservation* reservation = new Reservation(counter++, member, book);
            reservationByMember[memberId] = reservation;

            cout << "Member " << memberId << " reserved book \"" << book -> getTitle() << "\"." << endl;
        }

        // Call when a copy of `book` frees up (e.g. after a return) to hand it to the next member in line.
        // Returns the member to issue the freed copy to, or nullptr if no one is waiting.
        User* assignBook(Book* book) {
            auto it = reserveQueueByBookId.find(book -> getBookid());
            if(it == reserveQueueByBookId.end() || it -> second.empty()) {
                return nullptr;
            }

            User* nextMember = it -> second.front();
            it -> second.pop();

            int memberId = nextMember -> getUserid();
            reservedBookByMember.erase(memberId);

            auto resIt = reservationByMember.find(memberId);
            if(resIt != reservationByMember.end()) {
                resIt -> second -> updateReservationStatus(ReservationStatus::FULFILLED);
                reservationByMember.erase(resIt);
            }

            cout << "Reservation fulfilled for member " << memberId << " on book \"" << book -> getTitle() << "\"." << endl;
            return nextMember;
        }
};


class BookManagementService {
    private:
        static const int MAX_BOOKS_PER_MEMBER = 5;
        static const int LOAN_PERIOD_DAYS = 14;

        int recordCounter = 1;
        int bookIdCounter = 1;
        int copyIdCounter = 1;

        unordered_map<int, Record*> activeRecordByCopyId;
        unordered_map<int, vector<Record*>> activeRecordsByMemberId;
        FineManagementService* fineService;
        RecordManagementService* recordService;
        ReservationManagementService* reservationService;
        vector<Book*>books;

        BookCopy* findCopyById(Book* b, int copyId) {
            for(auto c : b -> getBookCopies()) {
                if(c -> getCopyId() == copyId) return c;
            }
            return nullptr;
        }

        Record* createLoan(Book* b, BookCopy* copy, User* member) {
            copy -> updateBookCopyStatus(BookCopyStatus::ISSUED);
            b -> decreaseNumberOfCopies();
            if(b -> getNumberOfCopies() == 0) b -> updateBookStatus(BookStatus::OVER);

            time_t issueDate = time(nullptr);
            time_t dueDate = issueDate + (time_t)LOAN_PERIOD_DAYS * 24 * 60 * 60;

            int memberId = member -> getUserid();
            Record* record = new Record(recordCounter++, member, copy, issueDate, dueDate);
            activeRecordByCopyId[copy -> getCopyId()] = record;
            activeRecordsByMemberId[memberId].push_back(record);
            recordService -> addRecord(memberId, record);

            return record;
        }

    public:
        BookManagementService(FineManagementService* fineService, RecordManagementService* recordService, ReservationManagementService* reservationService)
            : fineService(fineService), recordService(recordService), reservationService(reservationService) {};

        Book* addBook(string title, string isbn, BookCategory category, vector<Author*> authors) {
            Book* book = new Book(bookIdCounter++, title, isbn, category, 0);
            for(auto author : authors) book -> addAuthor(author);

            books.push_back(book);

            cout << "Book \"" << title << "\" added with id " << book -> getBookid() << "." << endl;
            return book;
        }

        BookCopy* addBookCopy(Book* b) {
            BookCopy* copy = new BookCopy(copyIdCounter++, b);
            b -> addBookCopy(copy);
            b -> increaseNumberOfCopies(1);
            b -> updateBookStatus(BookStatus::AVAILABLE);

            cout << "Copy " << copy -> getCopyId() << " added to book \"" << b -> getTitle() << "\"." << endl;
            return copy;
        }

        bool removeBookCopy(Book* b, int copyId) {
            BookCopy* copy = findCopyById(b, copyId);
            if(!copy) {
                cout << "Copy " << copyId << " not found in book \"" << b -> getTitle() << "\"." << endl;
                return false;
            }

            if(copy -> getBookCopyStatus() == BookCopyStatus::ISSUED) {
                cout << "Cannot remove copy " << copyId << " - it is currently issued." << endl;
                return false;
            }

            b -> removeBookCopy(copyId);
            b -> decreaseNumberOfCopies();
            if(b -> getNumberOfCopies() == 0) b -> updateBookStatus(BookStatus::OVER);

            delete copy;

            cout << "Copy " << copyId << " removed from book \"" << b -> getTitle() << "\"." << endl;
            return true;
        }

        void issueBook(Book* b, User* member) {
            int memberId = member -> getUserid();

            if((int)activeRecordsByMemberId[memberId].size() >= MAX_BOOKS_PER_MEMBER) {
                cout << "Member " << memberId << " already has " << MAX_BOOKS_PER_MEMBER << " books issued." << endl;
                return;
            }

            BookCopy* availableCopy = nullptr;
            for(auto c : b -> getBookCopies()) {
                if(c -> getBookCopyStatus() == BookCopyStatus::AVAILABLE) {
                    availableCopy = c;
                    break;
                }
            }

            if(!availableCopy) {
                cout << "No available copy for book: " << b -> getTitle() << endl;
                return;
            }

            createLoan(b, availableCopy, member);

            cout << "Book \"" << b -> getTitle() << "\" issued to member " << memberId
                 << " (copy " << availableCopy -> getCopyId() << "), due in " << LOAN_PERIOD_DAYS << " days." << endl;
        }

        void returnBook(Book* b, int copyId, User* member) {

            auto it = activeRecordByCopyId.find(copyId);
            if(it == activeRecordByCopyId.end()) {
                cout << "No active loan found for copy " << copyId << endl;
                return;
            }

            Record* record = it -> second;
            BookCopy* copy = record -> getBookCopy();

            time_t returnDate = time(nullptr);
            record -> returnBook(returnDate);

            copy -> updateBookCopyStatus(BookCopyStatus::AVAILABLE);
            b -> increaseNumberOfCopies(1);
            b -> updateBookStatus(BookStatus::AVAILABLE);

            activeRecordByCopyId.erase(it);
            auto& memberRecords = activeRecordsByMemberId[member -> getUserid()];
            memberRecords.erase(remove(memberRecords.begin(), memberRecords.end(), record), memberRecords.end());

            Fine* fine = fineService -> createFineIfOverdue(record);
            if(fine) {
                cout << "Book returned late. Fine: Rs. " << fine -> getAmount() << endl;
            } else {
                cout << "Book returned on time. No fine." << endl;
            }

            User* nextMember = reservationService -> assignBook(b);
            if(nextMember) {
                createLoan(b, copy, nextMember);
                cout << "Copy " << copy -> getCopyId() << " handed directly to reserved member "
                     << nextMember -> getUserid() << "." << endl;
            }
        }

        vector<Book*>searchBooks(SearchCriteria criteria) {
            vector<Book*>result;

            for(auto b : books) {
                if(!criteria.title.empty() && b -> getTitle() != criteria.title) continue;
                if(!criteria.isbn.empty() && b -> getIsbn() != criteria.isbn) continue;
                if(!criteria.authorName.empty()) {
                    vector<Author*>authors = b -> getAuthors();

                    for(auto a : authors) {
                        if(a -> getAuthorid() != criteria.authorId && a -> getAuthorName() != criteria.authorName) continue;
                    }
                } 

                result.push_back(b);
            }

            return result;
        }
};


class AdminService {
    private:
        BookManagementService* bookService;
        MemberManagementService* memberService;

    public:
        AdminService(BookManagementService* bookService, MemberManagementService* memberService)
            : bookService(bookService), memberService(memberService) {};

        Book* addBook(string title, string isbn, BookCategory category, vector<Author*> authors = {}) {
            return bookService -> addBook(title, isbn, category, authors);
        }

        BookCopy* addBookCopy(Book* book) {
            return bookService -> addBookCopy(book);
        }

        bool removeBookCopy(Book* book, int copyId) {
            return bookService -> removeBookCopy(book, copyId);
        }

        User* registerMember(string name, string email, string phone) {
            return memberService -> registerMember(name, email, phone);
        }
};


class Library {
    private:
        FineManagementService fineService;
        RecordManagementService recordService;
        ReservationManagementService reservationService;
        MemberManagementService memberService;
        BookManagementService bookService;
        AdminService adminService;

    public:
        Library()
            : bookService(&fineService, &recordService, &reservationService),
              adminService(&bookService, &memberService) {};

        // ---- Admin operations ----
        User* registerMember(string name, string email, string phone) {
            return adminService.registerMember(name, email, phone);
        }

        Book* addBook(string title, string isbn, BookCategory category, vector<Author*> authors = {}) {
            return adminService.addBook(title, isbn, category, authors);
        }

        BookCopy* addBookCopy(Book* book) {
            return adminService.addBookCopy(book);
        }

        bool removeBookCopy(Book* book, int copyId) {
            return adminService.removeBookCopy(book, copyId);
        }

        // ---- Member operations ----
        vector<Book*> searchBooks(SearchCriteria criteria) {
            return bookService.searchBooks(criteria);
        }

        void issueBook(Book* book, User* member) {
            bookService.issueBook(book, member);
        }

        void returnBook(Book* book, int copyId, User* member) {
            bookService.returnBook(book, copyId, member);
        }

        void reserveBook(Book* book, User* member) {
            reservationService.reserveBook(book, member);
        }

        void viewOutstandingFine(int memberId) {
            fineService.viewOutstandingFine(memberId);
        }

        void payFine(int fineId) {
            fineService.payFine(fineId);
        }

        void displayRecordOfMember(int memberId) {
            recordService.displayRecordOfMember(memberId);
        }
};


int main() {
    Library library;

    cout << "--- Registering members ---" << endl;
    User* alice = library.registerMember("Alice", "alice@example.com", "9999900001");
    User* bob   = library.registerMember("Bob", "bob@example.com", "9999900002");
    User* carol = library.registerMember("Carol", "carol@example.com", "9999900003");

    cout << "\n--- Adding books and copies ---" << endl;
    Author* orwell = new Author(1, "George Orwell");
    Book* nineteen84 = library.addBook("1984", "ISBN-1984", BookCategory::FICTION, {orwell});
    BookCopy* copyA = library.addBookCopy(nineteen84);
    BookCopy* copyB = library.addBookCopy(nineteen84);

    Book* dune = library.addBook("Dune", "ISBN-DUNE", BookCategory::SCIENCE);
    BookCopy* duneCopy = library.addBookCopy(dune);

    cout << "\n--- Searching for \"1984\" ---" << endl;
    SearchCriteria criteria;
    criteria.title = "1984";
    for(auto b : library.searchBooks(criteria)) {
        cout << "Found: " << b -> getTitle() << " (" << b -> getNumberOfCopies() << " copies available)" << endl;
    }

    cout << "\n--- Issuing books ---" << endl;
    library.issueBook(nineteen84, alice);   // takes copyA
    library.issueBook(dune, bob);           // takes the only Dune copy

    cout << "\n--- Carol wants Dune, but it's fully checked out ---" << endl;
    library.issueBook(dune, carol);         // rejected: no copy available
    library.reserveBook(dune, carol);       // joins the wait list

    cout << "\n--- Bob returns Dune; Carol should get it automatically ---" << endl;
    library.returnBook(dune, duneCopy -> getCopyId(), bob);

    cout << "\n--- Enforcing the 5-books-per-member limit ---" << endl;
    Book* comics = library.addBook("Comics Vol.1", "ISBN-COMICS", BookCategory::FICTION);
    for(int i = 0; i < 6; i++) library.addBookCopy(comics);
    for(int i = 0; i < 5; i++) library.issueBook(comics, alice); // 5th call fails (alice already has 1984 + 4 comics = 5)

    cout << "\n--- Admin removing a copy ---" << endl;
    library.removeBookCopy(nineteen84, copyA -> getCopyId());  // fails: currently issued to alice
    library.removeBookCopy(nineteen84, copyB -> getCopyId());  // succeeds: still available

    cout << "\n--- Borrowing history ---" << endl;
    library.displayRecordOfMember(alice -> getUserid());
    library.displayRecordOfMember(bob -> getUserid());
    library.displayRecordOfMember(carol -> getUserid());

    cout << "\n--- Outstanding fines (none yet, loans are still within the 14-day window) ---" << endl;
    library.viewOutstandingFine(bob -> getUserid());

    return 0;
}

