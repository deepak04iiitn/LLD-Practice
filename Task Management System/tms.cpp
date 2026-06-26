#include <bits/stdc++.h>
using namespace std;

enum class Priority {
    HIGH,
    MEDIUM,
    LOW
};

enum class Status {
    PENDING,
    IN_PROGRESS,
    COMPLETED
};


string priorityToString(Priority p) {
    switch(p) {
        case Priority::HIGH: return "High";
        case Priority::MEDIUM: return "Medium";
        case Priority::LOW: return "Low";
    }

    return "UNKNOWN";
}


string statusToString(Status s) {
    switch(s) {
        case Status::PENDING: return "Pending";
        case Status::IN_PROGRESS: return "In Progress";
        case Status::COMPLETED: return "Completed";
    }

    return "UNKNOWN";
}


class Task;
class Editor;

class IdGenerator {
    private:
        static int counter;

    public:
        static string generate(string prefix) {
            return prefix + to_string(++counter);
        }
};

int IdGenerator::counter = 0;

class User {
    private:
        string userid;
        string name;
        vector<Task*>tasks;

    public:
        User(string name , vector<Task*>tasks) {
            userid = IdGenerator::generate("USR"),
            this -> name = name;
            this -> tasks = tasks;
        }

        string getUserId() { 
            return userid; 
        }

        string getName() { 
            return name; 
        }

        void addTask(Task* task) {
            tasks.push_back(task);
        }

        void printTasks();

        void deleteTask(string taskid);
        void updateTask(string taskid , Editor* editor , string newValue);
        void markCompleted(Task* task , Status s);
        void assignToUser(Task* task , User* assignedUser);
};


class Task {
    private:
        string taskid;
        string title;
        string description;
        string duedate;
        Priority priority;
        Status status;
        User* user;

    public:
        Task(string title , string desc , string duedate , Priority p , Status s , User* user) {
            taskid = IdGenerator::generate("TSK");
            this -> title = title;
            this -> description = desc;
            this -> duedate = duedate;
            this -> priority = p;
            this -> status = s;
            this -> user = user;
        }

        string getTaskId() {
            return taskid;
        }

        string getTitle() {
            return title;
        }

        void setTitle(string newTitle) {
            title = newTitle;
        }

        string getDescription() {
            return description;
        }

        void setDescription(string newDesc) {
            description = newDesc;
        }

        string getTDuedate() {
            return duedate;
        }

        Priority getPriority() {
            return priority;
        }

        Status getStatus() {
            return status;
        }

        void setStatus(Status newStatus) {
            status = newStatus;
        }

        void print() {
            cout << "  [" << taskid << "] " << title
                 << " | " << priorityToString(priority)
                 << " | " << statusToString(status)
                 << " | Due: " << duedate
                 << " | " << description << "\n";
        }
};


class Editor {
    public:
        virtual void editProperty(string newValue) = 0;
};

class EditTitle : public Editor {
    private:
        Task* task;

    public:
        EditTitle(Task* task) : task(task) {}

        void editProperty(string newValue) override {
            task -> setTitle(newValue);
        }
};

class EditDescription : public Editor {
    private:
        Task* task;

    public:
        EditDescription(Task* task) : task(task) {}

        void editProperty(string newValue) override {
            task -> setDescription(newValue);
        }
};


void User::printTasks() {
    cout << "Tasks for " << name << " (" << userid << "):\n";
    if (tasks.empty()) {
        cout << "  (no tasks)\n";
        return;
    }
    for (auto task : tasks) task->print();
}


void User::deleteTask(string taskid)
{
    auto it = remove_if(
        tasks.begin(),
        tasks.end(),
        [&](Task* task)
        {
            return task -> getTaskId() == taskid;
        }
    );

    if (it != tasks.end())
    {
        tasks.erase(it, tasks.end());
    }
}


void User::updateTask(string taskid , Editor* editor , string newValue) {
    for(auto task : tasks) {
        if(task -> getTaskId() == taskid) {
            editor -> editProperty(newValue);
            return;
        }
    }
}


void User::markCompleted(Task* task , Status s) {
    task -> setStatus(s);
}


void User::assignToUser(Task* task , User* assignedUser) {
    this -> deleteTask(task -> getTaskId());
    assignedUser -> addTask(task);
}
 

int main() {
    // --- create users ---
    User* alice = new User("Alice", {});
    User* bob = new User("Bob", {});

    cout << "=== Task Management System ===\n\n";

    // --- create tasks ---
    Task* t1 = new Task("Design DB schema",  "Draft ER diagram",        "2026-07-01", Priority::HIGH,   Status::PENDING,     alice);
    Task* t2 = new Task("Write unit tests",  "Cover all edge cases",    "2026-07-05", Priority::MEDIUM, Status::IN_PROGRESS, alice);
    Task* t3 = new Task("Deploy to staging", "Run smoke tests after",   "2026-07-10", Priority::LOW,    Status::PENDING,     bob);

    alice -> addTask(t1);
    alice -> addTask(t2);
    bob -> addTask(t3);

    // --- print initial state ---
    cout << "--- Initial Tasks ---\n";
    alice -> markCompleted(t1 , Status::COMPLETED);
    alice -> printTasks();
    bob -> printTasks();

    // Assign task t2 to Bob
    alice -> assignToUser(t2, bob);
    alice -> printTasks();
    bob -> printTasks();

    // --- update title of t1 ---
    cout << "\n--- Updating title of " << t1 -> getTaskId() << " ---\n";
    EditTitle titleEditor(t1);
    alice -> updateTask(t1 -> getTaskId(), &titleEditor, "Design DB schema (v2)");
    alice -> printTasks();

    // --- update description of t2 ---
    cout << "\n--- Updating description of " << t2 -> getTaskId() << " ---\n";
    EditDescription descEditor(t2);
    alice -> updateTask(t2 -> getTaskId(), &descEditor, "Cover edge cases + integration tests");
    alice -> printTasks();

    // --- delete t1 ---
    cout << "\n--- Deleting " << t1 -> getTaskId() << " from Alice ---\n";
    alice -> deleteTask(t1 -> getTaskId());
    alice -> printTasks();

    // --- cleanup ---
    delete t1;
    delete t2;
    delete t3;
    delete alice;
    delete bob;

    return 0;
}