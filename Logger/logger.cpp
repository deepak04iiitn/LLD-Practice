#include <bits/stdc++.h>
using namespace std;

enum LogLevel {
    INFO = 1,
    DEBUG = 2,
    ERROR = 3
};

class LoggerHandler {
protected:
    int level;
    LoggerHandler* nextHandler;

public:
    LoggerHandler(int level) {
        this -> level = level;
        this -> nextHandler = nullptr;
    }

    void setNextHandler(LoggerHandler* next) {
        nextHandler = next;
    }

    virtual void log(int logLevel, string message) = 0;
};

class ConsoleHandler : public LoggerHandler {
public:
    ConsoleHandler(int level) : LoggerHandler(level) {}

    void log(int logLevel, string message) override {

        if(logLevel >= level) {
            cout << "[Console] " << message << endl;
        }

        if(nextHandler != nullptr) {
            nextHandler -> log(logLevel, message);
        }
    }
};

class FileHandler : public LoggerHandler {
public:
    FileHandler(int level) : LoggerHandler(level) {}

    void log(int logLevel, string message) override {

        if (logLevel >= level) {
            cout << "[File] " << message << endl;
        }

        if (nextHandler != nullptr) {
            nextHandler->log(logLevel, message);
        }
    }
};

class DatabaseHandler : public LoggerHandler {
public:
    DatabaseHandler(int level) : LoggerHandler(level) {}

    void log(int logLevel, string message) override {

        if (logLevel >= level) {
            cout << "[Database] " << message << endl;
        }

        if (nextHandler != nullptr) {
            nextHandler->log(logLevel, message);
        }
    }
};

int main() {

    LoggerHandler* console = new ConsoleHandler(INFO);
    LoggerHandler* file = new FileHandler(DEBUG);
    LoggerHandler* database = new DatabaseHandler(ERROR);

    // Build the chain
    console -> setNextHandler(file);
    file -> setNextHandler(database);

    cout << "INFO Log\n";
    console -> log(INFO, "Application Started");

    cout << "\nDEBUG Log\n";
    console -> log(DEBUG, "Fetching User Details");

    cout << "\nERROR Log\n";
    console -> log(ERROR, "Database Connection Failed");

    delete console;
    delete file;
    delete database;

    return 0;
}