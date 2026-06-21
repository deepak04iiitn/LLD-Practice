#include <bits/stdc++.h>
using namespace std;

class Question;
class Answer;
class Comment;
class QComment;
class AComment;

class User {
    private:
        string userid;
        string name;
        string email;
        Answer* answer;

    public:
        User(string userid , string name , string email) {
            this -> userid = userid;
            this -> name = name;
            this -> email = email;
        }

        void askQuestion(Question* question);
        void answerQuestion(Answer* answer);
        void commentOnQuestion(Question* question , QComment* comment);
        void commentOnAnswer(Answer* answer , AComment* comment);
        void upvoteQuestion(Question* question , string userid);
        void downvoteQuestion(Question* question , string userid);
        void upvoteAnswer(Answer* answer , string userid);
        void downvoteAnswer(Answer* answer , string userid);

        string getUserid() {
            return userid;
        }

        string getName() {
            return name;
        }

        string getEmail() {
            return email;
        }
};


class Question {
    private:
        User* user;
        string questionId;
        string questionText;
        vector<Answer>answers;
        vector<QComment>qcomments;
        vector<string>upvotes;
        vector<string>downvotes;
        int totalUpvotes = 0;
        int totalDownvotes = 0;

    public:
        Question(string qId , string questionText , User* user) {
            this -> questionId = qId;
            this -> questionText = questionText;
            this -> user = user;
        }

        void postQuestion() {
            cout << "\n[QUESTION POSTED]" << endl;
            cout << "  Asked by : " << user -> getName() << endl;
            cout << "  Question : " << questionText << endl;
        }

        void likeQuestion(string userid) {
            auto it = find(upvotes.begin() , upvotes.end() , userid);

            if(it == upvotes.end()) {
                upvotes.push_back(userid);
                totalUpvotes++;
                cout << "\n[UPVOTE] Question \"" << questionText << "\" upvoted. Total upvotes: " << totalUpvotes << endl;
            } else {
                upvotes.erase(it);
                if(totalUpvotes > 0) totalUpvotes--;
                cout << "\n[UPVOTE REMOVED] Question \"" << questionText << "\". Total upvotes: " << totalUpvotes << endl;
            }
        }

        void dislikeQestion(string userid) {
            auto it = find(downvotes.begin() , downvotes.end() , userid);

            if(it == downvotes.end()) {
                downvotes.push_back(userid);
                totalDownvotes++;
                cout << "\n[DOWNVOTE] Question \"" << questionText << "\" downvoted. Total downvotes: " << totalDownvotes << endl;
            } else {
                downvotes.erase(it);
                if(totalDownvotes > 0) totalDownvotes--;
                cout << "\n[DOWNVOTE REMOVED] Question \"" << questionText << "\". Total downvotes: " << totalDownvotes << endl;
            }
        }

        void addCommentQuestion(QComment* comment);

        string getQuestionid() {
            return questionId;
        }

        string getQuestionText() {
            return questionText;
        }

        int getTotalLikes() {
            return totalUpvotes;
        }

        int getTotalDislikes() {
            return totalDownvotes;
        }
};


class Answer {
    private:
        User* user;
        string answerId;
        string answerText;
        Question* question;
        vector<AComment>acomments;
        vector<string>upvotes;
        vector<string>downvotes;
        int totalUpvotes = 0;
        int totalDownvotes = 0;

    public:
        Answer(string answerId , string answerText , Question* question , User* user) {
            this -> answerId = answerId;
            this -> answerText = answerText;
            this -> question = question;
            this -> user = user;
        }

        void postAnswer() {
            cout << "\n[ANSWER POSTED]" << endl;
            cout << "  Question : " << question -> getQuestionText() << endl;
            cout << "  Answer   : " << answerText << endl;
            cout << "  By       : " << user -> getName() << endl;
        }

        void likeAnswer(string userid) {
            auto it = find(upvotes.begin() , upvotes.end() , userid);

            if(it == upvotes.end()) {
                upvotes.push_back(userid);
                totalUpvotes++;
                cout << "\n[UPVOTE] Answer \"" << answerText << "\" upvoted. Total upvotes: " << totalUpvotes << endl;
            } else {
                upvotes.erase(it);
                if(totalUpvotes > 0) totalUpvotes--;
                cout << "\n[UPVOTE REMOVED] Answer \"" << answerText << "\". Total upvotes: " << totalUpvotes << endl;
            }
        }

        void dislikeAnswer(string userid) {
            auto it = find(downvotes.begin() , downvotes.end() , userid);

            if(it == downvotes.end()) {
                downvotes.push_back(userid);
                totalDownvotes++;
                cout << "\n[DOWNVOTE] Answer \"" << answerText << "\" downvoted. Total downvotes: " << totalDownvotes << endl;
            } else {
                downvotes.erase(it);
                if(totalDownvotes > 0) totalDownvotes--;
                cout << "\n[DOWNVOTE REMOVED] Answer \"" << answerText << "\". Total downvotes: " << totalDownvotes << endl;
            }
        }

        void addCommentAnswer(AComment* comment);

        string getAnswerId() {
            return answerId;
        }

        string getAnswerText() {
            return answerText;
        }

        int getTotalLikes() {
            return totalUpvotes;
        }

        int getTotaldislikes() {
            return totalDownvotes;
        }
};


class Comment {
    virtual void addComment() = 0;
};

class QComment : public Comment {
    private:
        Question* question;
        User* user;
        string commentText;

    public:
        QComment(string commentText , Question* question , User* user) {
            this -> commentText = commentText;
            this -> question = question;
            this -> user = user;
        }

        void addComment() override {
            cout << "\n[COMMENT on QUESTION]" << endl;
            cout << "  Question : " << question -> getQuestionText() << endl;
            cout << "  Comment  : " << commentText << endl;
            cout << "  By       : " << user -> getName() << endl;
        }

};


class AComment : public Comment {
    private:
        Answer* answer;
        User* user;
        string commentText;

    public:
        AComment(string commentText , Answer* answer , User* user) {
            this -> commentText = commentText;
            this -> answer = answer;
            this -> user = user;
        }

        void addComment() override {
            cout << "\n[COMMENT on ANSWER]" << endl;
            cout << "  Answer   : " << answer -> getAnswerText() << endl;
            cout << "  Comment  : " << commentText << endl;
            cout << "  By       : " << user -> getName() << endl;
        }

};


// Out-of-line definitions — all classes fully defined here

void User::askQuestion(Question* question) {
    question -> postQuestion();
}

void User::answerQuestion(Answer* answer) {
    answer -> postAnswer();
}

void User::commentOnQuestion(Question* question , QComment* comment) {
    question -> addCommentQuestion(comment);
}

void User::commentOnAnswer(Answer* answer , AComment* comment) {
    answer -> addCommentAnswer(comment);
}

void User::upvoteQuestion(Question* question , string userid) {
    question -> likeQuestion(userid);
}

void User::downvoteQuestion(Question* question , string userid) {
    question -> dislikeQestion(userid);
}

void User::upvoteAnswer(Answer* answer , string userid) {
    answer -> likeAnswer(userid);
}

void User::downvoteAnswer(Answer* answer , string userid) {
    answer -> dislikeAnswer(userid);
}

void Question::addCommentQuestion(QComment* comment) {
    comment -> addComment();
}

void Answer::addCommentAnswer(AComment* comment) {
    comment -> addComment();
}


int main() {

    // Users
    User* user1 = new User("123" , "Deepak" , "abc@gmail.com");
    User* user2 = new User("223" , "Harsh" , "bcd@gmail.com");
    User* user3 = new User("323" , "Aniket" , "efg@gmail.com");
    User* user4 = new User("423" , "Aman" , "ghi@gmail.com");
    User* user5 = new User("523" , "Shiv" , "jkl@gmail.com");

    User* user6 = new User("623", "Rahul", "rahul@gmail.com");
    User* user7 = new User("723", "Priya", "priya@gmail.com");
    User* user8 = new User("823", "Neha", "neha@gmail.com");
    User* user9 = new User("923", "Rohan", "rohan@gmail.com");

    // Questions
    Question* q1 = new Question("001" , "What is prop drilling?" , user1);
    Question* q2 = new Question("002" , "What is JSX?" , user2);

    // Ask Question
    user1 -> askQuestion(q1);
    user2 -> askQuestion(q2);

    // Answers
    Answer* a1 = new Answer("100" , "Prop drilling is a type of drilling." , q1 , user3);
    Answer* a2 = new Answer("200" , "JSX is Javascript XML." , q2 , user4);

    // Answer questions
    user3 -> answerQuestion(a1);
    user4 -> answerQuestion(a2);

    // Comments
    QComment* c1 = new QComment("I also had doubt in this question" , q1 , user5);
    AComment* c2 = new AComment("Nice explanation" , a2 , user5);

    // Do comment
    user5 -> commentOnQuestion(q1 , c1);
    user5 -> commentOnAnswer(a2 , c2);

    // Like Question
    user6 -> upvoteQuestion(q1 , "623");

    // Dislike Question
    user7 -> downvoteQuestion(q2 , "723");

    // Like Answer
    user8 -> upvoteAnswer(a1 , "823");

    // Dislike Answer
    user9 -> downvoteAnswer(a2 , "923");
}
