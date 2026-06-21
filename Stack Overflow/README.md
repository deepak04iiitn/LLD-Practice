# Stack Overflow — LLD Walkthrough

A beginner-friendly guide to what this code does, how it's structured, and why each design decision was made.

---

## What are we building?

A simplified clone of Stack Overflow — a Q&A platform where users can:

- Register and build a reputation score
- Post questions with tags
- Answer questions
- Comment on questions or answers
- Upvote / downvote questions and answers
- Search questions by tag or keyword

---

## High-Level Flow

```
main()
  |
  ├── StackOverflow (the platform)
  |     ├── registerUser()  →  creates User objects
  |     ├── addQuestion()   →  stores questions in a map
  |     ├── searchByTag()   →  filters questions by tag
  |     └── searchByKeyword() → filters by title/body text
  |
  ├── User
  |     ├── askQuestion()   →  creates a Question, returns it
  |     ├── answerQuestion()→  creates an Answer, attaches to Question
  |     ├── commentOn()     →  creates a Comment, attaches to target
  |     ├── upvote()        →  delegates to Votable
  |     └── downvote()      →  delegates to Votable
  |
  ├── Question  (extends Votable + Commentable)
  ├── Answer    (extends Votable + Commentable)
  └── Comment
```

---

## Class-by-Class Breakdown

### `IdGenerator`

```cpp
class IdGenerator {
    static int counter;
public:
    static string generate(const string& prefix) {
        return prefix + to_string(++counter);
    }
};
```

**What it does:** Every object (User, Question, Answer, Comment) needs a unique ID. `IdGenerator` keeps a single global counter and pre-pends a prefix so you get IDs like `USR1`, `QST2`, `ANS3`, `CMT4`.

**Why a static counter?** There's only one counter shared across the whole program — no matter which class calls it, each ID is unique. A `static` member lives at the class level, not per-object.

---

### `Votable` (abstract base class)

```
Votable
  ├── upvoters[]   — list of user IDs who upvoted
  ├── downvoters[] — list of user IDs who downvoted
  ├── upvote(userId)
  └── downvote(userId)
```

**What it does:** Any content that can be voted on inherits from `Votable`. It handles the toggle-vote logic: if you upvote something you already upvoted, it removes your vote instead.

**Key design decision — toggle behaviour:**
```cpp
void upvote(const string& userId) {
    auto it = find(upvoters.begin(), upvoters.end(), userId);
    if (it == upvoters.end()) {   // not voted yet → add vote
        upvoters.push_back(userId);
        totalUpvotes++;
        onUpvote(userId);         // hook for subclasses
    } else {                      // already voted → remove it
        upvoters.erase(it);
        onUpvoteRemoved(userId);
    }
}
```

**Why virtual hooks (`onUpvote`, `onDownvote`…)?**
`Votable` doesn't know *what* is being voted on. The reputation change for a question (+5) is different from an answer (+10). So `Votable` calls `onUpvote()` (which does nothing by default), and `Question`/`Answer` override it to adjust reputation. This is the **Template Method pattern** — define the skeleton of an algorithm and let subclasses fill in the steps.

---

### `Commentable` (abstract base class)

```cpp
class Commentable {
    vector<Comment*> comments;
public:
    void addComment(Comment* comment);
    const vector<Comment*>& getComments() const;
};
```

**What it does:** Anything that can receive comments (Questions and Answers) inherits this. It stores a list of comments and provides `addComment()`.

**Why a separate base class?** Both `Question` and `Answer` support comments. Rather than duplicating the comment list and logic in both, we extract it into `Commentable` and inherit it. This is the **DRY principle** (Don't Repeat Yourself).

---

### `Comment`

```
Comment
  ├── commentId  (e.g. "CMT3")
  ├── text
  └── author (User*)
```

Simple value object — just holds the comment text and a pointer to its author.

---

### `User`

```
User
  ├── userId, name, email
  ├── reputation (starts at 0)
  └── methods:
        askQuestion()   → new Question(...)
        answerQuestion()→ new Answer(...) + question->addAnswer()
        commentOn()     → target->addComment(new Comment(...))
        upvote()        → target->upvote(userId)
        downvote()      → target->downvote(userId)
        addReputation() → reputation += delta
```

**Why does `User::upvote()` take a `Votable*`?**
A user can upvote both questions and answers. By accepting a `Votable*` (a pointer to the base class), one method handles both. This is **polymorphism** — the same call works regardless of the concrete type underneath.

**Reputation changes** are driven by vote events:

| Event                    | Author's reputation |
|--------------------------|---------------------|
| Question upvoted         | +5                  |
| Question downvoted       | -2                  |
| Question upvote removed  | -5                  |
| Question downvote removed| +2                  |
| Answer upvoted           | +10                 |
| Answer downvoted         | -2                  |
| Answer upvote removed    | -10                 |
| Answer downvote removed  | +2                  |

---

### `Question`

```
Question  (inherits Votable + Commentable)
  ├── questionId, title, body
  ├── author (User*)
  ├── tags   (vector<string>)
  └── answers (vector<Answer*>)
```

Overrides `onUpvote`, `onDownvote`, etc. to adjust the *question author's* reputation whenever a vote happens.

**Multiple inheritance** — `Question` gets voting behaviour from `Votable` and comment behaviour from `Commentable`. In C++ this is fine as long as both base classes have separate, non-overlapping responsibilities (which they do here).

---

### `Answer`

```
Answer  (inherits Votable + Commentable)
  ├── answerId, answerText
  ├── author   (User*)
  └── question (Question*)  — back-reference to the question
```

Same shape as `Question`. Reputation deltas are higher for answers (+10/-10) because helpful answers deserve more reward than questions.

---

### `StackOverflow` (the platform / facade)

```
StackOverflow
  ├── users[]              — all registered users
  ├── questions{}          — map<questionId, Question*>
  ├── registerUser()
  ├── addQuestion()
  ├── searchByTag()
  └── searchByKeyword()
```

**Why a `map` for questions?** A `map<string, Question*>` lets us look up any question by its ID in O(log n) time. The key is the question ID string (e.g., `"QST1"`).

**Why a separate `StackOverflow` class?** It acts as a **Facade** — a single entry point that owns all top-level data. Without it, users and questions would be loose global variables.

---

## Out-of-Line Definitions — Why Are Some Methods Defined Late?

You'll notice that some method bodies appear *after* all classes are declared:

```cpp
// ─── Out-of-line definitions ──────────────────────────────────────────────────

void Commentable::addComment(Comment* comment) { ... }
void Comment::display() const { ... }
Question* User::askQuestion(...) { ... }
```

**Why?** Because of forward declarations at the top:

```cpp
class User; class Question; class Answer; class Comment;
```

`User::askQuestion()` needs to construct a `Question` — but `Question` is defined *after* `User`. The forward declarations tell the compiler "these classes exist, trust me," allowing pointers (`Question*`) to be used. The full method bodies are then defined once all classes are complete.

---

## `main()` — The Full Story

```
1. Register 7 users (Deepak, Harsh, Aniket, Aman, Shiv, Rahul, Priya)

2. Deepak asks  → "What is prop drilling?"   [tags: react, javascript, frontend]
   Harsh asks   → "What is JSX?"             [tags: react, jsx, javascript]
   Both questions added to the platform.

3. Aniket answers q1 ("Prop drilling is passing data through many layers…")
   Aman   answers q2 ("JSX is JavaScript XML…")

4. Shiv  comments on q1  ("Great question!")
   Rahul comments on a2  ("Clear explanation!")

5. Voting:
   Shiv  upvotes  q1  → Deepak  +5 rep
   Rahul upvotes  q1  → Deepak  +5 rep  (total +10)
   Priya downvotes q2 → Harsh   -2 rep
   Deepak upvotes a1  → Aniket +10 rep
   Harsh  upvotes a2  → Aman   +10 rep
   Harsh  downvotes a1→ Aniket  -2 rep

6. Search by tag "react"     → returns both questions
   Search by keyword "JSX"   → returns q2

7. Reputation summary printed for all users.

8. Full platform display (all questions + answers + comments).
```

---

## Class Relationship Diagram

```
                    ┌─────────────┐
                    │StackOverflow│
                    │  (platform) │
                    └──────┬──────┘
                           │ owns
              ┌────────────┼────────────┐
              ▼                         ▼
           User*                   Question*  (in map)
              │                         │
     ┌────────┼────────┐         ┌──────┴──────┐
     │        │        │         │             │
  asks     answers  comments   Votable    Commentable
     │        │        │
     ▼        ▼        ▼
  Question  Answer  Comment
              │
         ┌────┴────┐
         │         │
      Votable  Commentable
```

---

## Key Design Patterns Used

| Pattern           | Where                              | Why                                              |
|-------------------|------------------------------------|--------------------------------------------------|
| Template Method   | `Votable::upvote()` + `onUpvote()` | Skeleton algorithm, subclasses fill in the steps |
| Polymorphism      | `User::upvote(Votable*)`           | One method works for Question and Answer both    |
| Facade            | `StackOverflow` class              | Single entry point to the whole system           |
| DRY / Mixin       | `Votable`, `Commentable` base classes | Shared behaviour extracted once, reused many times |
| Static Singleton  | `IdGenerator::counter`             | One shared counter for unique IDs across all objects |

---

## Things to Note for Interviews

1. **Why not use `shared_ptr`?** This is a demo — raw pointers keep the code readable. In production code always prefer smart pointers to avoid memory leaks.
2. **Toggle-vote is intentional** — upvoting twice cancels the first vote (like real Stack Overflow).
3. **`map` vs `unordered_map`** — `map` is used for questions; `unordered_map` would give O(1) lookup but `map` keeps questions in sorted order, which is fine for a demo.
4. **Forward declarations** are necessary because of circular dependencies (`User` ↔ `Question` ↔ `Answer`).
