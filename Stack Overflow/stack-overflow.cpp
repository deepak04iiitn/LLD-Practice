#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

class User; class Question; class Answer; class Comment;

// ─── IdGenerator ──────────────────────────────────────────────────────────────

class IdGenerator {
    static int counter;
public:
    static string generate(const string& prefix) {
        return prefix + to_string(++counter);
    }
};

int IdGenerator::counter = 0;

// ─── Votable ──────────────────────────────────────────────────────────────────

class Votable {
protected:
    vector<string> upvoters, downvoters;
    int totalUpvotes = 0, totalDownvotes = 0;

    virtual void onUpvote(const string&)          {}
    virtual void onDownvote(const string&)        {}
    virtual void onUpvoteRemoved(const string&)   {}
    virtual void onDownvoteRemoved(const string&) {}

public:
    void upvote(const string& userId) {
        auto it = find(upvoters.begin(), upvoters.end(), userId);
        if (it == upvoters.end()) {
            upvoters.push_back(userId);
            totalUpvotes++;
            onUpvote(userId);
        } else {
            upvoters.erase(it);
            if (totalUpvotes > 0) totalUpvotes--;
            onUpvoteRemoved(userId);
        }
    }

    void downvote(const string& userId) {
        auto it = find(downvoters.begin(), downvoters.end(), userId);
        if (it == downvoters.end()) {
            downvoters.push_back(userId);
            totalDownvotes++;
            onDownvote(userId);
        } else {
            downvoters.erase(it);
            if (totalDownvotes > 0) totalDownvotes--;
            onDownvoteRemoved(userId);
        }
    }

    int getUpvotes() const {
        return totalUpvotes;
    }
    int getDownvotes() const {
        return totalDownvotes;
    }
};

// ─── Commentable ──────────────────────────────────────────────────────────────

class Commentable {
protected:
    vector<Comment*> comments;
public:
    void addComment(Comment* comment);

    const vector<Comment*>& getComments() const {
        return comments;
    }
};

// ─── Comment ──────────────────────────────────────────────────────────────────

class Comment {
    string commentId, text;
    User*  author;
public:
    Comment(const string& text, User* author) {
        commentId = IdGenerator::generate("CMT");
        this->text = text;
        this->author = author;
    }

    void display() const;

    const string& getCommentId() const {
        return commentId;
    }

    const string& getText() const {
        return text;
    }

    User* getAuthor() const {
        return author;
    }
};

// ─── User ─────────────────────────────────────────────────────────────────────

class User {
    string         userId, name, email;
    int            reputation = 0;

public:
    User(const string& name, const string& email) {
        userId = IdGenerator::generate("USR");
        this->name = name;
        this->email = email;
    }

    Question* askQuestion(const string& title, const string& body, const vector<string>& tags);
    Answer*   answerQuestion(Question* question, const string& text);
    void      commentOn(Commentable* target, const string& text);
    void      upvote(Votable* target);
    void      downvote(Votable* target);

    void addReputation(int delta) {
        reputation += delta;
        cout << "  [REP] " << name << " -> " << reputation << " (" << (delta >= 0 ? "+" : "") << delta << ")\n";
    }

    const string& getUserId() const {
        return userId;
    }

    const string& getName() const {
        return name;
    }

    const string& getEmail() const {
        return email;
    }
    
    int getReputation() const {
        return reputation;
    }
};

// ─── Question ─────────────────────────────────────────────────────────────────

class Question : public Votable, public Commentable {
    string          questionId, title, body;
    User*           author;
    vector<string>  tags;
    vector<Answer*> answers;

protected:
    void onUpvote(const string&) override {
        author->addReputation(5);
        cout << "[UPVOTE] \"" << title << "\" | +" << totalUpvotes << "\n";
    }

    void onDownvote(const string&) override {
        author->addReputation(-2);
        cout << "[DOWNVOTE] \"" << title << "\" | -" << totalDownvotes << "\n";
    }

    void onUpvoteRemoved(const string&) override {
        author->addReputation(-5);
        cout << "[UPVOTE REMOVED] \"" << title << "\" | +" << totalUpvotes << "\n";
    }

    void onDownvoteRemoved(const string&) override {
        author->addReputation(2);
        cout << "[DOWNVOTE REMOVED] \"" << title << "\" | -" << totalDownvotes << "\n";
    }

public:
    Question(const string& questionTitle, const string& questionBody, User* questionAuthor, const vector<string>& questionTags) {
        questionId = IdGenerator::generate("QST");
        title = questionTitle;
        body = questionBody;
        author = questionAuthor;
        tags = questionTags;
    }

    void addAnswer(Answer* answer);
    void display() const;

    const string& getQuestionId() const {
        return questionId;
    }

    const string& getTitle() const {
        return title;
    }

    const string& getBody() const {
        return body;
    }

    User* getAuthor() const {
        return author;
    }

    const vector<string>& getTags() const {
        return tags;
    }

    const vector<Answer*>& getAnswers() const {
        return answers;
    }

};

// ─── Answer ───────────────────────────────────────────────────────────────────

class Answer : public Votable, public Commentable {
    string    answerId, answerText;
    User*     author;
    Question* question;

protected:
    void onUpvote(const string&) override {
        author->addReputation(10);
        cout << "[UPVOTE] Answer by " << author->getName() << " | +" << totalUpvotes << "\n";
    }

    void onDownvote(const string&) override {
        author->addReputation(-2);
        cout << "[DOWNVOTE] Answer by " << author->getName() << " | -" << totalDownvotes << "\n";
    }

    void onUpvoteRemoved(const string&) override {
        author->addReputation(-10);
        cout << "[UPVOTE REMOVED] Answer by " << author->getName() << " | +" << totalUpvotes << "\n";
    }

    void onDownvoteRemoved(const string&) override {
        author->addReputation(2);
        cout << "[DOWNVOTE REMOVED] Answer by " << author->getName() << " | -" << totalDownvotes << "\n";
    }

public:
    Answer(const string& answerText, Question* answerQuestion, User* answerAuthor) {
        answerId = IdGenerator::generate("ANS");
        this->answerText = answerText;
        author = answerAuthor;
        question = answerQuestion;
    }

    void display() const {
        cout << "  [ANSWER] " << answerText << "\n"
             << "    By: " << author->getName() << " | +" << totalUpvotes << " / -" << totalDownvotes << "\n";
        for (const auto& c : comments) c->display();
    }

    const string& getAnswerId() const {
        return answerId;
    }

    const string& getAnswerText() const {
        return answerText;
    }
    
    User* getAuthor() const {
        return author;
    }
    
};

// ─── Out-of-line definitions ───────────────────────────────────────────────────

void Commentable::addComment(Comment* comment) {
    comments.push_back(comment);
    comment->display();
}

void Comment::display() const {
    cout << "  [COMMENT] \"" << text << "\" — by " << author->getName() << "\n";
}

Question* User::askQuestion(const string& title, const string& body, const vector<string>& tags) {
    Question* q = new Question(title, body, this, tags);
    cout << "[QUESTION POSTED] \"" << title << "\" by " << name << "\n";
    return q;
}

Answer* User::answerQuestion(Question* question, const string& text) {
    Answer* a = new Answer(text, question, this);
    question->addAnswer(a);
    return a;
}

void User::commentOn(Commentable* target, const string& text) {
    target->addComment(new Comment(text, this));
}

void User::upvote(Votable* target) {
    target->upvote(userId);
}

void User::downvote(Votable* target) {
    target->downvote(userId);
}

void Question::addAnswer(Answer* answer) {
    answers.push_back(answer);
    cout << "[ANSWER POSTED] by " << answer->getAuthor()->getName() << " on \"" << title << "\"\n";
}

void Question::display() const {
    cout << "\n[QUESTION " << questionId << "] " << title << "\n"
         << "  " << body << "\n  Tags: ";
    for (const auto& t : tags) cout << "[" << t << "] ";
    cout << "\n  By: " << author->getName() << " | +" << totalUpvotes << " / -" << totalDownvotes << "\n";
    cout << "  Answers (" << answers.size() << "):\n";
    for (const auto& a : answers) a->display();
    cout << "  Comments (" << comments.size() << "):\n";
    for (const auto& c : comments) c->display();
}

// ─── StackOverflow Platform ───────────────────────────────────────────────────

class StackOverflow {
    vector<User*>          users;
    map<string, Question*> questions;

public:
    User* registerUser(const string& name, const string& email) {
        User* user = new User(name, email);
        users.push_back(user);
        cout << "[USER REGISTERED] " << name << " (id: " << user->getUserId() << ")\n";
        return user;
    }

    void addQuestion(Question* question) {
        questions[question->getQuestionId()] = question;
    }

    vector<Question*> searchByTag(const string& tag) const {
        vector<Question*> result;
        for (const auto& [id, q] : questions) {
            const auto& tags = q->getTags();
            if (find(tags.begin(), tags.end(), tag) != tags.end())
                result.push_back(q);
        }
        return result;
    }

    vector<Question*> searchByKeyword(const string& keyword) const {
        vector<Question*> result;
        for (const auto& [id, q] : questions)
            if (q->getTitle().find(keyword) != string::npos || q->getBody().find(keyword) != string::npos)
                result.push_back(q);
        return result;
    }

    void displayAll() const {
        cout << "\n========== ALL QUESTIONS ==========";
        for (const auto& [id, q] : questions) q->display();
    }
};

// ─── main ─────────────────────────────────────────────────────────────────────

int main() {
    StackOverflow platform;

    User* deepak = platform.registerUser("Deepak", "deepak@gmail.com");
    User* harsh  = platform.registerUser("Harsh",  "harsh@gmail.com");
    User* aniket = platform.registerUser("Aniket", "aniket@gmail.com");
    User* aman   = platform.registerUser("Aman",   "aman@gmail.com");
    User* shiv   = platform.registerUser("Shiv",   "shiv@gmail.com");
    User* rahul  = platform.registerUser("Rahul",  "rahul@gmail.com");
    User* priya  = platform.registerUser("Priya",  "priya@gmail.com");

    cout << "\n";

    Question* q1 = deepak->askQuestion(
        "What is prop drilling?",
        "I keep hearing about prop drilling in React. What is it and why is it bad?",
        {"react", "javascript", "frontend"}
    );
    platform.addQuestion(q1);

    Question* q2 = harsh->askQuestion(
        "What is JSX?",
        "I see JSX everywhere in React code. How does it work under the hood?",
        {"react", "jsx", "javascript"}
    );
    platform.addQuestion(q2);

    cout << "\n";

    Answer* a1 = aniket->answerQuestion(q1, "Prop drilling is passing data through many component layers via props.");
    Answer* a2 = aman->answerQuestion(q2,   "JSX is JavaScript XML — it lets you write HTML-like syntax inside JS.");

    cout << "\n";

    shiv->commentOn(q1, "Great question, I had the same doubt!");
    rahul->commentOn(a2, "Clear explanation, thanks!");

    cout << "\n";

    shiv->upvote(q1);
    rahul->upvote(q1);
    priya->downvote(q2);
    deepak->upvote(a1);
    harsh->upvote(a2);
    harsh->downvote(a1);

    cout << "\n";

    cout << "\n[SEARCH] tag=react\n";
    for (Question* q : platform.searchByTag("react"))
        cout << "  -> " << q->getTitle() << "\n";

    cout << "\n[SEARCH] keyword=JSX\n";
    for (Question* q : platform.searchByKeyword("JSX"))
        cout << "  -> " << q->getTitle() << "\n";

    cout << "\n[REPUTATION SUMMARY]\n";
    for (User* u : {deepak, harsh, aniket, aman, shiv, rahul, priya})
        cout << "  " << u->getName() << ": " << u->getReputation() << "\n";

    platform.displayAll();

    return 0;
}
