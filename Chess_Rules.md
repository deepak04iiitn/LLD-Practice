# ♟️ Chess Rules Explained (for Chess LLD)

A plain-language guide to every chess rule you need before designing the Low Level Design (LLD) of a Chess game. Each section explains the rule simply, gives examples, and ends with a **"LLD takeaway"** so you know how the rule shows up in code.

---

## Table of Contents

1. [The Board](#1-the-board)
2. [Initial Board Setup](#2-initial-board-setup)
3. [Turns](#3-turns)
4. [How Each Piece Moves (Overview)](#4-how-each-piece-moves-overview)
5. [Pawn](#5-pawn-)
6. [Rook](#6-rook-)
7. [Bishop](#7-bishop-)
8. [Queen](#8-queen-)
9. [Knight](#9-knight-)
10. [King](#10-king-)
11. [Capturing](#11-capturing)
12. [You Cannot Capture Your Own Piece](#12-you-cannot-capture-your-own-piece)
13. [Who Can Jump and Who Cannot](#13-who-can-jump-and-who-cannot)
14. [Check](#14-check)
15. [What Can You Do When in Check?](#15-what-can-you-do-when-in-check)
16. [You Cannot Leave Your King in Check](#16-you-cannot-leave-your-king-in-check)
17. [Checkmate](#17-checkmate)
18. [Stalemate](#18-stalemate)
19. [Castling](#19-castling)
20. [En Passant](#20-en-passant)
21. [Pawn Promotion](#21-pawn-promotion)
22. [Draw](#22-draw)
23. [Mapping the Rules to Your LLD](#23-mapping-the-rules-to-your-lld)
24. [What to Implement vs. Just Discuss](#24-what-to-implement-vs-just-discuss-interview-strategy)

---

## 1. The Board

Chess is played on a square grid:

- **8 × 8 = 64 squares**
- **Files** are the columns, labelled **a → h** (left to right)
- **Ranks** are the rows, labelled **1 → 8** (bottom to top, from White's side)

Every square has a unique name made of *file + rank*:

```
a1, b1, c1 ... h1
a2, b2, c2 ... h2
...
a8, b8, c8 ... h8
```

So `e4` means "column e, row 4".

### The pieces

Each player starts with **16 pieces**:

| Piece  | Count |
|--------|-------|
| King   | 1     |
| Queen  | 1     |
| Rook   | 2     |
| Bishop | 2     |
| Knight | 2     |
| Pawn   | 8     |

### The two sides

There are two colors: **White** and **Black**. **White always moves first.**

> **LLD takeaway:** You need a `Color` enum (`WHITE`, `BLACK`), a `Board` of 64 cells, and a way to convert names like `"e4"` into row/column numbers (e.g. `e4 → row 3, col 4` if you use 0-indexing).

---

## 2. Initial Board Setup

Your `Board.initialize()` method has to place every piece in the correct starting square. This is what the board looks like at the start:

```
8   R N B Q K B N R      ← Black's back rank
7   P P P P P P P P      ← Black's pawns
6   . . . . . . . .
5   . . . . . . . .
4   . . . . . . . .
3   . . . . . . . .
2   P P P P P P P P      ← White's pawns
1   R N B Q K B N R      ← White's back rank

    a b c d e f g h
```

(`R` = Rook, `N` = Knight, `B` = Bishop, `Q` = Queen, `K` = King, `P` = Pawn, `.` = empty)

### White's back rank (rank 1)

| Square | Piece  |
|--------|--------|
| a1     | Rook   |
| b1     | Knight |
| c1     | Bishop |
| d1     | Queen  |
| e1     | King   |
| f1     | Bishop |
| g1     | Knight |
| h1     | Rook   |

- **a2 to h2** are all **Pawns**.
- **Black** has the *same arrangement mirrored* on ranks **8** (back rank) and **7** (pawns).

> **Memory trick:** "The Queen stands on her own color." White Queen on `d1` (a light square), Black Queen on `d8` (a dark square). The Queen is always on the **d-file**, the King on the **e-file**.

> **LLD takeaway:** `Board.initialize()` fills rows 0, 1, 6, 7 with the right pieces and leaves rows 2–5 empty.

---

## 3. Turns

Players **alternate** turns, strictly:

```
White → Black → White → Black → ...
```

You can never move twice in a row, and you can never skip a turn.

Your `Game` class needs to keep track of whose turn it is:

```java
Color currentTurn;
```

After every **successful** move, flip it:

```java
currentTurn = opposite(currentTurn);
```

> **Important:** Only flip the turn if the move was valid. An invalid move attempt should leave the turn unchanged so the same player can try again.

---

## 4. How Each Piece Moves (Overview)

This is the most important part of the LLD. Every piece has its own movement rule. In OOP terms, this is where **polymorphism** comes in: each piece class implements its own version of "can I move from here to there?".

| Piece  | Moves like…                               | Can jump over pieces? |
|--------|-------------------------------------------|-----------------------|
| Pawn   | Forward 1 (or 2 on first move); captures diagonally | No |
| Rook   | Straight lines (up/down/left/right)       | No |
| Bishop | Diagonals                                 | No |
| Queen  | Rook + Bishop combined                    | No |
| Knight | "L" shape                                 | **Yes** |
| King   | 1 square in any direction                 | No |

The next sections explain each one in detail.

---

## 5. Pawn ♙

Pawns are the trickiest basic piece because **they move one way but capture another way.**

### Normal movement

A pawn moves **one square straight forward**.

- White pawns move "up" the board: `e2 → e3`
- Black pawns move "down" the board: `e7 → e6`

The pawn can only move forward if the square in front of it is **empty**. A pawn can never move backward or sideways.

### First move (double step)

The very first time a pawn moves, it is allowed to move **two squares** forward:

```
e2 → e4
```

This is only allowed if **both** squares are empty:

```
e3 = empty
e4 = empty
```

If something is sitting on `e3`, the pawn can't jump over it to reach `e4`.

### Capturing

A pawn captures **one square diagonally forward** — never straight ahead.

For a White pawn on `e4`:

```
   d5   e5   f5
     ↖  ↑  ↗
        e4
```

- `e5` (straight ahead) → **move only**, can't capture there
- `d5` and `f5` (diagonals) → **capture only**, and only if an enemy piece is actually there

Example:

```
White pawn on e4
Black piece on d5

e4 → d5   ✅ this is a capture
```

A pawn **cannot** capture the piece directly in front of it. If an enemy piece is right in front, the pawn is simply blocked.

### Summary table

| Action        | Direction                    | Destination must be…          |
|---------------|------------------------------|-------------------------------|
| Move 1 step   | Straight forward             | Empty                         |
| Move 2 steps  | Straight forward (first move only) | Both squares empty      |
| Capture       | Diagonal forward (1 step)    | Contains an enemy piece       |

### Promotion

When a pawn reaches the far end of the board it must be **promoted** (see [section 21](#21-pawn-promotion)).

> **LLD takeaway:** `Pawn` needs to know its color (to know which direction is "forward") and whether it has moved yet (to allow the double step). Its `canMove()` logic must check **whether the destination is empty or an enemy**, because the rule differs for straight moves vs. diagonal moves.

---

## 6. Rook ♖

The rook moves in **straight lines**:

```
    ↑
 ←  R  →
    ↓
```

It can move **any number of squares** up, down, left, or right.

Example: `e4 → e8` is potentially valid.

### It cannot jump

There must be **nothing in between** the source and the destination:

```
Rook (e4)
  ↓
  ↓
Pawn (e6)   ← blocking piece
  ↓
  e8        ← Rook can NOT reach this
```

The rook can reach `e5`, and it can even capture the pawn on `e6` if it's an enemy, but it can't go past it.

### Capturing

If the destination holds an **enemy** piece, the rook captures it (the enemy piece is removed and the rook takes its square).

> **LLD takeaway:** A rook move is valid if `sourceRow == destRow` **or** `sourceCol == destCol`, **and** the path between is clear.

---

## 7. Bishop ♗

The bishop moves **diagonally**:

```
 ↖   ↗
   B
 ↙   ↘
```

Any number of squares along a diagonal.

Example path: `c1 → d2 → e3 → f4 → g5`

### It cannot jump

Just like the rook, a bishop is blocked by any piece in its way. It can capture the first enemy piece it runs into, but can't go past it.

### Fun fact: bishops stay on one color

A bishop always remains on the same color of square it started on. A bishop that starts on a dark square will only ever visit dark squares. (You don't need this for the LLD, but it's good to know. It's also why each side has two bishops: one for each color.)

> **LLD takeaway:** A bishop move is valid if `abs(rowDiff) == abs(colDiff)` (and rowDiff ≠ 0) and the path is clear.

---

## 8. Queen ♕

The queen is the **most powerful piece**. She combines the rook and the bishop:

```
↖  ↑  ↗
←  Q  →
↙  ↓  ↘
```

She can move **any number of squares** in any of the 8 directions: up, down, left, right, and the four diagonals.

She **cannot jump** over pieces.

> **LLD takeaway:** `Queen` move logic = `Rook` logic **OR** `Bishop` logic. You can reuse the same helper methods (or even compose/delegate to them).

---

## 9. Knight ♘

The knight moves in an **"L" shape**:

> 2 squares in one direction **+** 1 square perpendicular

```
      X       X
  X               X
          N
  X               X
      X       X
```

From `e4`, a knight can go to exactly these squares:

```
c3   c5
d2   d6
f2   f6
g3   g5
```

That's up to **8 possible destinations**. (Fewer if the knight is near an edge or if its own pieces occupy some of those squares.)

### The special rule: knights can jump!

The knight is the **only piece that can jump over other pieces**. It doesn't matter what's in between:

```
Knight
  ↓
Pawn    ← doesn't matter
  ↓
Pawn    ← doesn't matter
```

It just lands on its destination square. So for knights, you **do not** need a path-clear check.

### Quick check for LLD

A move is a valid knight move if:

```
(abs(rowDiff) == 2 && abs(colDiff) == 1)
   OR
(abs(rowDiff) == 1 && abs(colDiff) == 2)
```

---

## 10. King ♔

The king moves **exactly one square in any direction**:

```
↖ ↑ ↗
← K →
↙ ↓ ↘
```

From `e4`, the king can potentially go to:

```
d3   e3   f3
d4        f4
d5   e5   f5
```

### ⚠️ VERY IMPORTANT: The king cannot move into check

A king may never step onto a square that an enemy piece is attacking.

Example:

```
White King:  e4
Black Rook:  e8
```

The rook attacks the whole e-file (as long as nothing blocks it). So White's king **cannot** move to `e5` or `e3` (they're still on the rook's file). It would have to step sideways to `d3`, `d4`, `f4`, etc.

This rule is why you will need both:

- `isValidMove()` — does this piece's movement pattern allow this move?
- `isCheck()` / `isSquareAttacked()` — is a given square under attack by the enemy?

> **LLD takeaway:** The king's basic move is easy (`abs(rowDiff) <= 1 && abs(colDiff) <= 1`, not both zero). The hard part is the "don't step into an attacked square" check, which needs board-level knowledge.

---

## 11. Capturing

Almost every piece captures **the same way it moves.** If a rook can move to a square, and an enemy piece is on that square, the rook can capture it.

Example:

```
White Rook:   e4
Black Knight: e7

e4 → e7   ✅  the rook captures the knight
```

What happens technically: the captured piece is removed from the board, and the capturing piece takes its place.

```cpp
board[destination] = movingPiece;
board[source] = nullptr;
```

### The exception: Pawn

| Action         | Direction          |
|----------------|--------------------|
| Normal move    | Straight forward ↑ |
| Capture        | Diagonal forward ↖ ↗ |

Remember this carefully. It is the most common bug in chess implementations.

---

## 12. You Cannot Capture Your Own Piece

A piece can only land on a square that is **empty** or holds an **enemy** piece. It can never land on a square occupied by a friendly piece.

Example:

```
White Rook:   a1
White Knight: a3

Rook a1 → a3   ❌ invalid (friendly piece is there)
```

In code:

```cpp
if (destination->piece != nullptr &&
    destination->piece->color == currentPlayer) {
    // invalid move
}
```

---

## 13. Who Can Jump and Who Cannot

**Cannot jump** over other pieces:

- Rook
- Bishop
- Queen
- King
- Pawn

**Can jump:**

- **Knight** only

Example of a blocked rook:

```
Rook → X → X
        ↑
   Rook cannot reach the second X
```

Because of this, your LLD needs a helper such as:

```
isPathClear(source, destination)
```

It walks through every square *between* source and destination (not including either end) and returns `false` if any of them has a piece. The Knight simply skips this check.

---

## 14. Check

**Check** means a player's **King is currently under attack** by an enemy piece (i.e., if it were the enemy's turn, they could capture the king).

Example:

```
White King: e1
Black Rook: e8
```

If nothing is between them:

```
Black Rook (e8)
     │
     │
     │
White King (e1)
```

White is **in check**. White must deal with it immediately on their next move.

---

## 15. What Can You Do When in Check?

If your king is in check, your next move **must** remove the check. There are exactly three ways:

### Option 1 — Move the King

Move the king to a square that is not attacked.

```
King → safe square
```

### Option 2 — Capture the attacking piece

The king itself, or any other piece of yours, captures the piece that is giving check.

```
King / another piece
        ↓
   captures attacker
```

### Option 3 — Block the attack

This only works against **long-range attackers** (Rook, Bishop, Queen), because they attack along a line. Put one of your pieces on a square *between* the attacker and your king.

```
Black Rook
    │
White Bishop   ← blocks the line
    │
White King
```

Now the king is no longer attacked.

> **Note:** You cannot "block" a Knight's check (it jumps), nor a Pawn's check (it's adjacent). Against those, you can only move the king or capture the attacker.

If **none** of these three options is possible, it's **checkmate**.

---

## 16. You Cannot Leave Your King in Check

This is a **critical** rule for the LLD.

**You may never make a move that results in your own king being in check** — whether you were in check before or not.

### Example (pinned piece)

Before the move:

```
Black Rook
    │
White Bishop    ← shielding the king
    │
White King
```

If the bishop moves away, the rook suddenly attacks the king:

```
Black Rook
    │
    │
White King   ← now in check!
```

So that bishop move is **illegal**. A piece in this situation is called **pinned**.

### What this means for your code

It's not enough to ask:

```cpp
piece->isValidMovement(source, destination);
```

You must *also* ask:

> **"If I make this move, will my own king be in check?"**

### Standard technique: simulate the move

```
1. Temporarily make the move on the board
2. Check whether the current player's king is now attacked
3. Undo the move
4. If the king was attacked → the move is illegal
```

This is one of the most important design challenges in Chess LLD. Plan for it in your `Board`/`Game` class.

---

## 17. Checkmate

**Checkmate** = the king is in check **and** there is no legal move to escape.

```
King is attacked
      +
Can't move the King
      +
Can't capture the attacker
      +
Can't block the attack
      ↓
  CHECKMATE
```

The game **ends immediately**. The player who delivered the checkmate **wins**.

> **LLD takeaway:** After each move, check whether the *opponent* is in check. If so, see if they have **any** legal move at all. If not → `CHECKMATE`.

---

## 18. Stalemate

Stalemate is easy to confuse with checkmate. The difference is whether the king is currently in check.

| Situation                              | Result        |
|----------------------------------------|---------------|
| King **in check** + **no** legal moves | **Checkmate** → that player **loses** |
| King **not in check** + **no** legal moves | **Stalemate** → **draw** |

Decision tree:

```
           Player has no legal moves?
                      │
            ┌─────────┴─────────┐
            │                   │
      King in check?       King not in check
            │                   │
        CHECKMATE           STALEMATE
            │                   │
         Lose                 Draw
```

In a stalemate, the player isn't under attack but literally cannot move anything legally. Nobody wins — it's a draw.

> For an SDE-1 interview, knowing this distinction is enough.

---

## 19. Castling

Castling is a **special move** involving the **King + a Rook**. It is the **only move where two pieces move in the same turn.**

### Kingside castling (short castle)

```
Before:   King e1     Rook h1
After:    King g1     Rook f1
```

- King: `e1 → g1`
- Rook: `h1 → f1`

### Queenside castling (long castle)

```
Before:   King e1     Rook a1
After:    King c1     Rook d1
```

- King: `e1 → c1`
- Rook: `a1 → d1`

(Black does the same on rank 8: `e8 → g8` / `e8 → c8`.)

### Conditions — castling is allowed only if ALL of these are true

1. The king has **never moved**.
2. The rook involved has **never moved**.
3. All squares **between** the king and that rook are **empty**.
4. The king is **not currently in check**.
5. The king does **not pass through** an attacked square.
6. The king does **not end up on** an attacked square.

Example: for kingside castling, the king travels `e1 → f1 → g1`. If `f1` or `g1` is attacked by an enemy piece, castling is not allowed.

> **Interview tip:** Initially just **discuss** castling rather than implementing it, unless the interviewer specifically asks. If you do implement it, you need `hasMoved` flags on `King` and `Rook`.

---

## 20. En Passant

This is probably the **most confusing** chess rule. You are **not** expected to implement it for an SDE-1 interview unless specifically asked.

It's a **special pawn capture**.

### Setup

```
White pawn: e5
Black pawn: d7
```

Black plays the double step:

```
d7 → d5
```

The black pawn has now "jumped past" `d6`, landing right next to White's pawn on `e5`.

### The capture

White can capture **as if the black pawn had only moved one square**:

```
White pawn: e5 → d6     (d6 is empty!)
Black pawn on d5: removed
```

Even though `d6` is empty, White moves there diagonally, and the black pawn on `d5` (the one that just passed) is removed.

### Important timing rule

En passant is only available **immediately on the very next move.** If White doesn't take it right away, the opportunity is gone forever.

> **Interview tip:** Know what it is and be able to explain it, but don't prioritize implementing it. If you do, the `Game` must remember the **last move** (specifically: "was it a pawn double-step, and to which square?").

---

## 21. Pawn Promotion

When a pawn reaches the **last rank** on the opposite side of the board, it **must** be promoted:

- **White** pawn reaches **rank 8**
- **Black** pawn reaches **rank 1**

It is replaced by one of:

- Queen
- Rook
- Bishop
- Knight

(It can't stay a pawn and can't become a King.) Most players choose the **Queen** because she's the strongest.

Flow:

```
White Pawn reaches rank 8
           ↓
      Promotion
           ↓
  Choose: Q / R / B / N
```

### Simplified LLD approach

```java
promotePawn(PieceType type)
```

You can also default to Queen if you want to keep it simple, and mention that the user could choose in a fuller version.

---

## 22. Draw

A **draw** means nobody wins. For the LLD, keep it simple.

### The one to implement

- **Stalemate** — no legal moves and the king is **not** in check.

### Other official draw conditions (know them, don't implement them)

| Draw type              | Meaning (simple terms) |
|------------------------|------------------------|
| Agreement              | Both players agree to draw |
| Threefold repetition   | The same board position occurs 3 times |
| Fifty-move rule        | 50 moves with no pawn move and no capture |
| Insufficient material  | Neither side has enough pieces to ever checkmate (e.g., King vs. King) |

> Don't implement these for an SDE-1 LLD unless the interviewer explicitly asks.

---

## 23. Mapping the Rules to Your LLD

You don't need a class for every rule. Here's a reasonable mental model:

```
                     Game
                      │
           ┌──────────┴──────────┐
           │                     │
        Player                 Board
                                 │
                             64 Cells
                                 │
                               Piece
                                 │
     ┌────────┬────────┬───────┼────────┬────────┐
     │        │        │       │        │        │
   King     Queen     Rook   Bishop   Knight    Pawn
```

### What `Game` holds

```
Game
 │
 ├── currentPlayer
 ├── board
 ├── gameStatus
 │
 └── makeMove(source, destination)
```

### The `makeMove(source, destination)` flow

This checklist pulls together every rule above:

```
makeMove(source, destination)
   │
   ├── 1. Is it this player's turn?
   ├── 2. Is the source square occupied?
   ├── 3. Does the piece belong to the current player?
   ├── 4. Is the piece's movement pattern valid?
   ├── 5. Is the path clear? (skip for Knight)
   ├── 6. Is the destination valid? (empty or enemy, not friendly)
   ├── 7. Would this leave my own King in check?
   │
   └── 8. Execute the move
           │
           ├── Capture if required
           ├── Promotion if required
           ├── Is the opponent in Check?
           ├── Is it Checkmate? / Stalemate?
           └── Switch turn
```

### Which rule belongs to which part of the design?

| Rule                                   | Where it lives |
|----------------------------------------|----------------|
| Movement pattern of each piece         | Each `Piece` subclass (polymorphism) |
| Path-clear check                       | `Board` helper (`isPathClear`) |
| Turn order                             | `Game` (`currentTurn`) |
| Can't capture own piece                | `Game`/`Board` validation |
| Is king attacked? / Check              | `Board` or `Game` (`isCheck`, `isSquareAttacked`) |
| Can't leave own king in check          | `Game` (simulate move → check → undo) |
| Checkmate / Stalemate                  | `Game` (after each move, see if opponent has any legal move) |
| Promotion                              | `Game`/`Board` after move execution |
| Castling / En passant                  | Extra logic in `Game` (optional) |

### Suggested class responsibilities (quick sketch)

```java
enum Color { WHITE, BLACK }
enum GameStatus { ACTIVE, CHECK, CHECKMATE, STALEMATE, DRAW }

abstract class Piece {
    Color color;
    abstract boolean canMove(Board board, Cell from, Cell to);
}

class Cell {
    int row, col;
    Piece piece;
}

class Board {
    Cell[][] cells = new Cell[8][8];
    void initialize();
    boolean isPathClear(Cell from, Cell to);
    boolean isSquareAttacked(Cell cell, Color byColor);
}

class Game {
    Board board;
    Player whitePlayer, blackPlayer;
    Color currentTurn;
    GameStatus status;
    boolean makeMove(Cell from, Cell to);
}
```

---

## 24. What to Implement vs. Just Discuss (Interview Strategy)

For an **SDE-1 LLD interview**, a sensible priority list:

### ✅ Implement (core)

- Board, cells, and initial setup
- All six piece types with correct movement
- Path-clear check (except Knight)
- Turn handling
- Capturing (and not capturing your own piece)
- Check detection
- "Can't leave own king in check"
- Checkmate and stalemate detection
- Basic pawn promotion (even if it just defaults to Queen)

### 💬 Discuss (only implement if asked)

- Castling
- En passant
- Other draw rules (repetition, fifty-move, insufficient material, agreement)
- Undo/redo, move history, timers, AI opponent

### Why this ordering?

Interviewers mainly want to see **clean object-oriented design** (good class structure, polymorphism for pieces, separation of concerns) and that you can handle the **core rules correctly**. Knowing about the advanced rules and being able to explain how you'd extend the design to support them is usually enough.

---

*End of guide.*