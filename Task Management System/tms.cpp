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

        string getDuedate() {
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

        User* getUser() {
            return user;
        }

        void setUser(User* u) {
            user = u;
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
    task -> setUser(assignedUser);
    assignedUser -> addTask(task);
}
 

// ── Specification / Filter hierarchy ─────────────────────────────────────────

class TaskFilter {
    public:
        virtual bool matches(Task* task) const = 0;
        virtual ~TaskFilter() = default;
};

class PriorityFilter : public TaskFilter {
    Priority priority;
    public:
        PriorityFilter(Priority p) : priority(p) {}
        bool matches(Task* task) const override {
            return task->getPriority() == priority;
        }
};

class StatusFilter : public TaskFilter {
    Status status;
    public:
        StatusFilter(Status s) : status(s) {}
        bool matches(Task* task) const override {
            return task->getStatus() == status;
        }
};

enum class DateOp { BEFORE, ON, AFTER };

// Date strings must be in "YYYY-MM-DD" format; lexicographic comparison is correct.
class DueDateFilter : public TaskFilter {
    string date;
    DateOp op;
    public:
        DueDateFilter(string date, DateOp op) : date(date), op(op) {}
        bool matches(Task* task) const override {
            string d = task->getDuedate();
            switch (op) {
                case DateOp::BEFORE: return d < date;
                case DateOp::ON:     return d == date;
                case DateOp::AFTER:  return d > date;
            }
            return false;
        }
};

class AssignedUserFilter : public TaskFilter {
    string userid;
    public:
        AssignedUserFilter(User* user) : userid(user->getUserId()) {}
        bool matches(Task* task) const override {
            User* u = task->getUser();
            return u && u->getUserId() == userid;
        }
};

// Case-insensitive substring match on title
class TitleKeywordFilter : public TaskFilter {
    string keyword;
    public:
        TitleKeywordFilter(string kw) {
            transform(kw.begin(), kw.end(), back_inserter(keyword), ::tolower);
        }
        bool matches(Task* task) const override {
            string title = task->getTitle();
            string lower;
            transform(title.begin(), title.end(), back_inserter(lower), ::tolower);
            return lower.find(keyword) != string::npos;
        }
};

// Combines multiple filters with AND semantics
class CompositeFilter : public TaskFilter {
    vector<TaskFilter*> filters;
    public:
        CompositeFilter(vector<TaskFilter*> filters) : filters(filters) {}
        bool matches(Task* task) const override {
            for (auto f : filters)
                if (!f->matches(task)) return false;
            return true;
        }
};

// ── TaskManager ───────────────────────────────────────────────────────────────

class TaskManager {
    vector<Task*> tasks;

    public:
        void addTask(Task* task) {
            tasks.push_back(task);
        }

        void removeTask(string taskid) {
            auto it = remove_if(tasks.begin(), tasks.end(),
                [&](Task* t) { return t->getTaskId() == taskid; });
            tasks.erase(it, tasks.end());
        }

        // Returns all tasks matching every provided filter (AND logic).
        vector<Task*> search(vector<TaskFilter*> filters) const {
            CompositeFilter composite(filters);
            vector<Task*> result;
            for (auto t : tasks)
                if (composite.matches(t)) result.push_back(t);
            return result;
        }

        void printResults(const vector<Task*>& results, const string& label) const {
            cout << "--- " << label << " (" << results.size() << " found) ---\n";
            if (results.empty()) cout << "  (none)\n";
            else for (auto t : results) t->print();
        }
};

// ─────────────────────────────────────────────────────────────────────────────

int main() {
    // --- create users ---
    User* alice = new User("Alice", {});
    User* bob   = new User("Bob",   {});

    cout << "=== Task Management System ===\n\n";

    // --- create tasks ---
    Task* t1 = new Task("Design DB schema",    "Draft ER diagram",             "2026-07-01", Priority::HIGH,   Status::PENDING,     alice);
    Task* t2 = new Task("Write unit tests",    "Cover all edge cases",         "2026-07-05", Priority::MEDIUM, Status::IN_PROGRESS, alice);
    Task* t3 = new Task("Deploy to staging",   "Run smoke tests after",        "2026-07-10", Priority::LOW,    Status::PENDING,     bob);
    Task* t4 = new Task("Fix login bug",       "Null pointer in auth service", "2026-07-03", Priority::HIGH,   Status::IN_PROGRESS, bob);
    Task* t5 = new Task("Write DB migration",  "Migrate users table",          "2026-07-08", Priority::MEDIUM, Status::PENDING,     alice);

    alice->addTask(t1); alice->addTask(t2); alice->addTask(t5);
    bob->addTask(t3);   bob->addTask(t4);

    // Register all tasks with the global manager
    TaskManager tm;
    for (Task* t : {t1, t2, t3, t4, t5}) tm.addTask(t);

    // --- print initial state ---
    cout << "--- Initial Tasks ---\n";
    alice->markCompleted(t1, Status::COMPLETED);
    alice->printTasks();
    bob->printTasks();

    // Assign t2 to Bob
    alice->assignToUser(t2, bob);
    alice->printTasks();
    bob->printTasks();

    // --- update title of t1 ---
    cout << "\n--- Updating title of " << t1->getTaskId() << " ---\n";
    EditTitle titleEditor(t1);
    alice->updateTask(t1->getTaskId(), &titleEditor, "Design DB schema (v2)");
    alice->printTasks();

    // --- update description of t2 (now owned by Bob) ---
    cout << "\n--- Updating description of " << t2->getTaskId() << " ---\n";
    EditDescription descEditor(t2);
    bob->updateTask(t2->getTaskId(), &descEditor, "Cover edge cases + integration tests");
    bob->printTasks();

    // --- delete t1 ---
    cout << "\n--- Deleting " << t1->getTaskId() << " from Alice ---\n";
    alice->deleteTask(t1->getTaskId());
    tm.removeTask(t1->getTaskId());
    alice->printTasks();

    // ── Search & Filter demos ─────────────────────────────────────────────────
    cout << "\n=== Search & Filter ===\n\n";

    // 1. All HIGH priority tasks
    {
        PriorityFilter pf(Priority::HIGH);
        tm.printResults(tm.search({&pf}), "HIGH priority tasks");
    }

    // 2. All PENDING tasks
    {
        StatusFilter sf(Status::PENDING);
        tm.printResults(tm.search({&sf}), "PENDING tasks");
    }

    // 3. Tasks due on or before 2026-07-05
    {
        DueDateFilter df("2026-07-06", DateOp::BEFORE);
        tm.printResults(tm.search({&df}), "Tasks due before 2026-07-06");
    }

    // 4. Tasks assigned to Bob
    {
        AssignedUserFilter uf(bob);
        tm.printResults(tm.search({&uf}), "Tasks assigned to Bob");
    }

    // 5. Keyword search: tasks with "db" in the title
    {
        TitleKeywordFilter kf("db");
        tm.printResults(tm.search({&kf}), "Tasks with 'db' in title");
    }

    // 6. Composite: HIGH priority + IN_PROGRESS
    {
        PriorityFilter pf(Priority::HIGH);
        StatusFilter   sf(Status::IN_PROGRESS);
        tm.printResults(tm.search({&pf, &sf}), "HIGH priority + IN_PROGRESS");
    }

    // 7. Composite: tasks assigned to Bob, due before 2026-07-08
    {
        AssignedUserFilter uf(bob);
        DueDateFilter      df("2026-07-08", DateOp::BEFORE);
        tm.printResults(tm.search({&uf, &df}), "Bob's tasks due before 2026-07-08");
    }

    // --- cleanup ---
    delete t1; delete t2; delete t3; delete t4; delete t5;
    delete alice; delete bob;

    return 0;
}