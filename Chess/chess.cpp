#include <bits/stdc++.h>
using namespace std;

// ============================================================
// ENUMS
// ============================================================

enum class Color {
    WHITE,
    BLACK
};

enum class GameStatus {
    NOT_STARTED,
    IN_PROGRESS,
    WHITE_WON,
    BLACK_WON,
    DRAW
};

enum class PieceType {
    KING,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT,
    PAWN
};


// ============================================================
// POSITION
// ============================================================

class Position {
    private:
        int row;
        int col;

    public:
        Position(int row , int col) : row(row) , col(col) {};

        int getRow() const {
            return row;
        }

        int getCol() const {
            return col;
        }

        bool operator==(const Position& other) const {
            return row == other.row && col == other.col;
        }

        bool operator!=(const Position& other) const {
            return row != other.row || col != other.col;
        }

        bool isValid() const {
            return row >= 0 && row < 8 && col >= 0 && col < 8;
        }
};


// ============================================================
// FORWARD DECLARATION
// ============================================================

class Board;


// ============================================================
// PIECE BASE CLASS
// ============================================================

class Piece {
    private:
        Color color;
        Position position;
        bool captured;
        bool hasMoved;

    public:
        Piece(Color color , Position position) : color(color) , position(position) , captured(false) , hasMoved(false) {};

        Color getColor() const {
            return color;
        }

        Position getPosition() const {
            return position;
        }

        void setPosition(Position newPosition) {
            position = newPosition;
        }

        bool isCaptured() const {
            return captured;
        }

        void setCaptured(bool value) {
            captured = value;
        }

        bool getHasMoved() const {
            return hasMoved;
        }

        void setHasMoved(bool value) {
            hasMoved = value;
        }

        virtual PieceType getType() const = 0;

        virtual bool canMove(
            const Position& from,
            const Position& to,
            const Board& board
        ) const = 0;

        virtual bool canAttack(
            const Position& from,
            const Position& to,
            const Board& board
        ) const = 0;

        virtual char getSymbol() const = 0;
};


// ============================================================
// BOARD
// ============================================================

class Board {
    private:
        Piece* cells[8][8];

    public:
        Board() {
            clear();
        }

        void clear() {
            for(auto& row : cells) {
                for(auto& cell : row) {
                    cell = nullptr;
                }
            }
        }

        bool isValidPosition(const Position& position) const {
            return position.isValid();
        }

        Piece* getPiece(const Position& position) const {
            if(!position.isValid()) return nullptr;

            return cells[position.getRow()][position.getCol()];
        }

        void setPiece(const Position& position, Piece* piece) {
            if(position.isValid()) {
                cells[position.getRow()][position.getCol()] = piece;
            }
        }

        void removePiece(const Position& position) {
            if(position.isValid()) {
                cells[position.getRow()][position.getCol()] = nullptr;
            }
        }

        bool isEmpty(const Position& position) const {
            return getPiece(position) == nullptr;
        }

        bool isPathClear(const Position& from, const Position& to) const {
            int rowDirection = 0;
            int colDirection = 0;

            if(to.getRow() > from.getRow()) rowDirection = 1;
            else if(to.getRow() < from.getRow()) rowDirection = -1;

            if(to.getCol() > from.getCol()) colDirection = 1;
            else if(to.getCol() < from.getCol()) colDirection = -1;

            int currentRow = from.getRow() + rowDirection;
            int currentCol = from.getCol() + colDirection;

            while(currentRow != to.getRow() && currentCol != to.getCol()) {
                if(cells[currentRow][currentCol] != nullptr) return false;

                currentRow += rowDirection;
                currentCol += colDirection;
            }

            return true;
        }

        // --------------------------------------------------------
        // Find King's position
        // --------------------------------------------------------

        Position findKing(Color color) const {
            for(int row = 0; row < 8; row++) {
                for(int col = 0; col < 8; col++) {

                    Piece* piece = cells[row][col];

                    if(piece != nullptr && !piece -> isCaptured() && piece -> getColor() == color && piece -> getType() == PieceType::KING) {
                        return Position(row, col);
                    }
                }
            }

            // Should never happen in a valid chess game.
            return Position(-1, -1);
        }

        // --------------------------------------------------------
        // Is a square attacked by a particular color?
        // --------------------------------------------------------

        bool isSquareAttacked(const Position& target , Color attackingColor) const {
            for(int row = 0 ; row < 8 ; row++) {
                for(int col = 0 ; col < 8 ; col++) {
                    Piece* piece = cells[row][col];

                    if(piece == nullptr || piece -> isCaptured() || piece -> getColor() != attackingColor) continue;

                    Position from(row , col);

                    if(piece -> canAttack(from , target , *this)) {
                        return true;
                    }
                }
            }

            return false;
        }

        bool isKingInCheck(Color color) const {
            Position kingPos = findKing(color);

            if(!kingPos.isValid()) return false;

            Color opponent = color == Color::WHITE ? Color::BLACK : Color::WHITE;

            return isSquareAttacked(kingPos , opponent);
        }

        // --------------------------------------------------------
        // Temporary move support
        //
        // Used to test:
        // "If I make this move, will my King be in check?"
        // --------------------------------------------------------

        Piece* movePieceTemporarily(const Position& from , const Position& to) {
            Piece* movingPiece = getPiece(from);
            Piece* capturedPiece = getPiece(to);

            removePiece(from);
            setPiece(to , movingPiece);

            if(movingPiece != nullptr) {
                movingPiece -> setPosition(to);
            }

            if(capturedPiece != nullptr) {
                capturedPiece -> setCaptured(true);
            }

            return capturedPiece;
        }

        void undoTemporaryMove(const Position& from , const Position& to , Piece* capturedPiece) {

            Piece* movingPiece = getPiece(to);
            removePiece(to);
            setPiece(from , movingPiece);

            if(movingPiece != nullptr) {
                movingPiece -> setPosition(from);
            }

            if(capturedPiece != nullptr) {
                capturedPiece -> setCaptured(false);
                setPiece(to , capturedPiece);
            }
        }

        // --------------------------------------------------------
        // Display board
        // --------------------------------------------------------

        void display() const {

            cout << "\n";

            for(int row = 0 ; row < 8 ; row++) {

                cout << 8 - row << "  ";

                for(int col = 0 ; col < 8 ; col++) {

                    Piece* piece = cells[row][col];

                    if(piece == nullptr) {
                        cout << ". ";
                    }
                    else {
                        cout << piece -> getSymbol() << " ";
                    }
                }

                cout << "\n";
            }

            cout << "\n   a b c d e f g h\n\n";
        }
};


// ============================================================
// HELPER FUNCTIONS
// ============================================================

bool isSameColor(Piece* first , Piece* second) {
    if(first == nullptr || second == nullptr) return false;

    return first -> getColor() == second -> getColor();
}

bool isOpponent(Piece* first , Piece* second) {
    if(first == nullptr || second == nullptr) return false;

    return first -> getColor() != second -> getColor();
}

bool isDiagonal(const Position& from , const Position& to) {
    return abs(from.getRow() - to.getRow()) == abs(from.getCol() - to.getCol());
}

bool isStraight(const Position& from , const Position& to) {
    return from.getRow() == to.getRow() || from.getCol() == to.getCol();
}


// ============================================================
// KING
// ============================================================

class King : public Piece {
    public:
        King(Color color , Position position) : Piece(color , position) {};

        PieceType getType() const override {
            return PieceType::KING;
        }

        char getSymbol() const override {
            return getColor() == Color::WHITE ? 'K' : 'k';
        }

        bool canMove(const Position& from , const Position& to , const Board& board) const override {
            int rowDiff = abs(from.getRow() - to.getRow());
            int colDiff = abs(from.getCol() - to.getCol());

            return rowDiff <= 1 && colDiff <= 1 && !(rowDiff == 0 && colDiff == 0);
        }

        bool canAttack(const Position& from , const Position& to , const Board& board) const override {
            return canMove(from , to , board);
        }
};


// ============================================================
// QUEEN
// ============================================================

class Queen : public Piece {
    public:
        Queen(Color color , Position position) : Piece(color , position) {};

        PieceType getType() const override {
            return PieceType::QUEEN;
        }

        char getSymbol() const override {
            return getColor() == Color::WHITE ? 'Q' : 'q';
        }

        bool canMove(const Position& from , const Position& to , const Board& board) const override {
            if(!isStraight(from , to) && !isDiagonal(from , to)) return false;

            return board.isPathClear(from , to);
        }

        bool canAttack(const Position& from , const Position& to , const Board& board) const override {
            return canMove(from , to , board);
        }
};


// ============================================================
// ROOK
// ============================================================

class Rook : public Piece {
    public:
        Rook(Color color , Position position) : Piece(color , position) {};

        PieceType getType() const override {
            return PieceType::ROOK;
        }

        char getSymbol() const override {
            return getColor() == Color::WHITE ? 'R' : 'r';
        }

        bool canMove(const Position& from , const Position& to , const Board& board) const override {
            
            if(!isStraight(from , to)) {
                return false;
            }

            return board.isPathClear(from , to);
        } 

        bool canAttack(const Position& from , const Position& to , const Board& board) const override {
            return canMove(from , to , board);
        }
};


// ============================================================
// BISHOP
// ============================================================

class Bishop : public Piece {
    public:
        Bishop(Color color , Position position) : Piece(color , position) {};

        PieceType getType() const override {
            return PieceType::BISHOP;
        }

        char getSymbol() const override {
            return getColor() == Color::WHITE ? 'B' : 'b';
        }

        bool canMove(const Position& from , const Position& to , const Board& board) const override {
            
            if(!isDiagonal(from , to)) {
                return false;
            }

            return board.isPathClear(from , to);
        } 

        bool canAttack(const Position& from , const Position& to , const Board& board) const override {
            return canMove(from , to , board);
        }
};


// ============================================================
// KNIGHT
// ============================================================

class Knight : public Piece {
    public:
        Knight(Color color , Position position) : Piece(color , position) {};

        PieceType getType() const override {
            return PieceType::KNIGHT;
        }

        char getSymbol() const override {
            return getColor() == Color::WHITE ? 'N' : 'n';
        }

        bool canMove(const Position& from , const Position& to , const Board& board) const override {
            
            int rowDiff = abs(from.getRow() - to.getRow());
            int colDiff = abs(from.getCol() - to.getCol());

            return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
        } 

        bool canAttack(const Position& from , const Position& to , const Board& board) const override {
            return canMove(from , to , board);
        }
};


// ============================================================
// PAWN
// ============================================================

class Pawn : public Piece {
    public:
        Pawn(Color color , Position position) : Piece(color , position) {};

        PieceType getType() const override {
            return PieceType::PAWN;
        }

        char getSymbol() const override {
            return getColor() == Color::WHITE ? 'P' : 'p';
        }

        bool canMove(const Position& from , const Position& to , const Board& board) const override {
            
            int direction = getColor() == Color::WHITE ? -1 : 1;

            int rowDiff = to.getRow() - from.getRow();
            int colDiff = abs(to.getCol() - from.getCol());

            Piece* destination = board.getPiece(to);

            // ----------------------------------------------------
            // Normal forward movement
            // ----------------------------------------------------

            if(colDiff == 0 && rowDiff == direction && destination == nullptr) return true;

            // ----------------------------------------------------
            // First move: pawn can move 2 squares
            // ----------------------------------------------------

            if(colDiff == 0 && rowDiff == 2 * direction && !getHasMoved() && destination == nullptr) {

                Position middle(from.getRow() + direction , from.getCol());

                return board.isEmpty(middle);
            }

            // ----------------------------------------------------
            // Diagonal capture
            // ----------------------------------------------------

            if(colDiff == 1 && rowDiff == direction && destination != nullptr && destination -> getColor() != getColor()) return true;

            return false;
            
        } 

        bool canAttack(const Position& from , const Position& to , const Board& board) const override {
            
            int direction = getColor() == Color::WHITE ? -1 : 1;

            int rowDiff = to.getRow() - from.getRow();
            int colDiff = abs(to.getCol() - from.getCol());

            // Pawns attack diagonally.
            return rowDiff == direction && colDiff == 1;
        }
};


// ============================================================
// PLAYER
// ============================================================

class Player {
    private:
        string name;
        Color color;

    public:
        Player(string name , Color color) : name(std::move(name)) , color(color) {};

        string getName() const {
            return name;
        }

        Color getColor() const {
            return color;
        }
};


// ============================================================
// GAME
// ============================================================

class Game {
    private:
        Board board;
        Player whitePlayer;
        Player blackPlayer;

        Color currentTurn;

        GameStatus status;

        // Game owns all pieces.
        vector<unique_ptr<Piece>> pieces;

    public:
        Game(const string& whiteName , const string& blackName) 
            : whitePlayer(whiteName , Color::WHITE) , blackPlayer(blackName , Color::BLACK) , currentTurn(Color::WHITE) , status(GameStatus::NOT_STARTED) {
                initializeBoard();

                status = GameStatus::IN_PROGRESS;
            }

        // --------------------------------------------------------
        // Board initialization
        // --------------------------------------------------------

        void initializeBoard() {

            // White pieces

            addPiece<Rook>(
                Color::WHITE,
                Position(7, 0)
            );

            addPiece<Knight>(
                Color::WHITE,
                Position(7, 1)
            );

            addPiece<Bishop>(
                Color::WHITE,
                Position(7, 2)
            );

            addPiece<Queen>(
                Color::WHITE,
                Position(7, 3)
            );

            addPiece<King>(
                Color::WHITE,
                Position(7, 4)
            );

            addPiece<Bishop>(
                Color::WHITE,
                Position(7, 5)
            );

            addPiece<Knight>(
                Color::WHITE,
                Position(7, 6)
            );

            addPiece<Rook>(
                Color::WHITE,
                Position(7, 7)
            );

            for(int col = 0; col < 8; col++) {

                addPiece<Pawn>(
                    Color::WHITE,
                    Position(6, col)
                );
            }


            // Black pieces

            addPiece<Rook>(
                Color::BLACK,
                Position(0, 0)
            );

            addPiece<Knight>(
                Color::BLACK,
                Position(0, 1)
            );

            addPiece<Bishop>(
                Color::BLACK,
                Position(0, 2)
            );

            addPiece<Queen>(
                Color::BLACK,
                Position(0, 3)
            );

            addPiece<King>(
                Color::BLACK,
                Position(0, 4)
            );

            addPiece<Bishop>(
                Color::BLACK,
                Position(0, 5)
            );

            addPiece<Knight>(
                Color::BLACK,
                Position(0, 6)
            );

            addPiece<Rook>(
                Color::BLACK,
                Position(0, 7)
            );

            for (int col = 0; col < 8; col++) {

                addPiece<Pawn>(
                    Color::BLACK,
                    Position(1, col)
                );
            }
        }

        // --------------------------------------------------------
        // Generic piece creation
        // --------------------------------------------------------

        template <typename T>
        void addPiece(
            Color color,
            Position position
        ) {

            auto piece =
                make_unique<T>(color, position);

            Piece* rawPointer = piece.get();

            pieces.push_back(std::move(piece));

            board.setPiece(position, rawPointer);
        }

        // --------------------------------------------------------
        // Convert chess notation to Position
        //
        // "e2" -> Position(6,4)
        // --------------------------------------------------------

        Position parsePosition(
            const string& square
        ) const {

            if (square.length() != 2) {
                return Position(-1, -1);
            }

            char file = square[0];
            char rank = square[1];

            if (file < 'a' || file > 'h' ||
                rank < '1' || rank > '8') {

                return Position(-1, -1);
            }

            int col = file - 'a';

            // rank 8 -> row 0
            // rank 1 -> row 7

            int row = 8 - (rank - '0');

            return Position(row, col);
        }

        // --------------------------------------------------------
        // Make a move
        // --------------------------------------------------------

        bool makeMove(
            const string& fromSquare,
            const string& toSquare
        ) {

            if (status != GameStatus::IN_PROGRESS) {
                cout << "Game is already over.\n";
                return false;
            }

            Position from =
                parsePosition(fromSquare);

            Position to =
                parsePosition(toSquare);

            if (!from.isValid() || !to.isValid()) {
                cout << "Invalid position.\n";
                return false;
            }

            Piece* piece =
                board.getPiece(from);

            // ----------------------------------------------------
            // Validate source
            // ----------------------------------------------------

            if (piece == nullptr) {
                cout << "No piece at source position.\n";
                return false;
            }

            // ----------------------------------------------------
            // Validate turn
            // ----------------------------------------------------

            if (piece->getColor() != currentTurn) {

                cout << "It is not this player's turn.\n";
                return false;
            }

            // ----------------------------------------------------
            // Can't capture own piece
            // ----------------------------------------------------

            Piece* destination =
                board.getPiece(to);

            if (destination != nullptr &&
                destination->getColor() == currentTurn) {

                cout << "Cannot capture your own piece.\n";
                return false;
            }

            // ----------------------------------------------------
            // Piece-specific movement
            // ----------------------------------------------------

            if (!piece->canMove(from, to, board)) {

                cout << "Invalid movement for this piece.\n";
                return false;
            }

            // ----------------------------------------------------
            // IMPORTANT:
            //
            // Check whether this move leaves our King in check.
            // ----------------------------------------------------

            if (wouldLeaveKingInCheck(from, to)) {

                cout << "Illegal move: King would remain in check.\n";
                return false;
            }

            // ----------------------------------------------------
            // Execute actual move
            // ----------------------------------------------------

            executeMove(from, to);

            // ----------------------------------------------------
            // Change turn
            // ----------------------------------------------------

            currentTurn =
                currentTurn == Color::WHITE
                ? Color::BLACK
                : Color::WHITE;

            // ----------------------------------------------------
            // Check game state
            // ----------------------------------------------------

            updateGameStatus();

            return true;
        }

    private:

        // --------------------------------------------------------
        // Execute move
        // --------------------------------------------------------

        void executeMove(
            const Position& from,
            const Position& to
        ) {

            Piece* movingPiece =
                board.getPiece(from);

            Piece* capturedPiece =
                board.getPiece(to);

            // Remove source.
            board.removePiece(from);

            // Capture destination.
            if (capturedPiece != nullptr) {
                capturedPiece->setCaptured(true);
            }

            // Place moving piece.
            board.setPiece(to, movingPiece);

            movingPiece->setPosition(to);
            movingPiece->setHasMoved(true);
        }

        // --------------------------------------------------------
        // Check whether a move leaves own King in check
        // --------------------------------------------------------

        bool wouldLeaveKingInCheck(
            const Position& from,
            const Position& to
        ) {

            Piece* movingPiece =
                board.getPiece(from);

            if (movingPiece == nullptr) {
                return true;
            }

            Color playerColor =
                movingPiece->getColor();

            bool previousHasMoved =
                movingPiece->getHasMoved();

            Piece* capturedPiece =
                board.movePieceTemporarily(from, to);

            bool kingInCheck =
                board.isKingInCheck(playerColor);

            // Restore board.
            board.undoTemporaryMove(
                from,
                to,
                capturedPiece
            );

            // Restore state.
            movingPiece->setHasMoved(previousHasMoved);

            return kingInCheck;
        }

        // --------------------------------------------------------
        // Does player have at least one legal move?
        // --------------------------------------------------------

        bool hasAnyLegalMove(Color color) {

            for (int fromRow = 0; fromRow < 8; fromRow++) {

                for (int fromCol = 0; fromCol < 8; fromCol++) {

                    Position from(fromRow, fromCol);

                    Piece* piece =
                        board.getPiece(from);

                    if (piece == nullptr ||
                        piece->isCaptured() ||
                        piece->getColor() != color) {

                        continue;
                    }

                    // Try every possible destination.
                    for (int toRow = 0; toRow < 8; toRow++) {

                        for (int toCol = 0; toCol < 8; toCol++) {

                            Position to(toRow, toCol);

                            if (from == to) {
                                continue;
                            }

                            Piece* destination =
                                board.getPiece(to);

                            // Can't capture own piece.
                            if (destination != nullptr &&
                                destination->getColor() == color) {

                                continue;
                            }

                            // Piece movement rules.
                            if (!piece->canMove(
                                    from,
                                    to,
                                    board)) {

                                continue;
                            }

                            // Would King still be safe?
                            if (!wouldLeaveKingInCheck(
                                    from,
                                    to)) {

                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }

        // --------------------------------------------------------
        // Update check/checkmate/stalemate
        // --------------------------------------------------------

        void updateGameStatus() {

            Color playerToMove = currentTurn;

            bool inCheck =
                board.isKingInCheck(playerToMove);

            bool hasLegalMove =
                hasAnyLegalMove(playerToMove);

            // ----------------------------------------------------
            // Checkmate
            // ----------------------------------------------------

            if (inCheck && !hasLegalMove) {

                if (playerToMove == Color::WHITE) {
                    status = GameStatus::BLACK_WON;
                }
                else {
                    status = GameStatus::WHITE_WON;
                }

                return;
            }

            // ----------------------------------------------------
            // Stalemate
            // ----------------------------------------------------

            if (!inCheck && !hasLegalMove) {

                status = GameStatus::DRAW;
                return;
            }

            // Otherwise game continues.
            status = GameStatus::IN_PROGRESS;
        }

    public:

        // --------------------------------------------------------
        // Public game information
        // --------------------------------------------------------

        void displayBoard() const {
            board.display();
        }

        GameStatus getStatus() const {
            return status;
        }

        Color getCurrentTurn() const {
            return currentTurn;
        }
};


// ============================================================
// MAIN
// ============================================================

int main() {

    Game game(
        "Deepak",
        "Opponent"
    );

    game.displayBoard();

    // Example legal moves

    cout << "White moves e2 -> e4\n";

    game.makeMove("e2", "e4");

    game.displayBoard();

    cout << "Black moves e7 -> e5\n";

    game.makeMove("e7", "e5");

    game.displayBoard();

    cout << "White moves g1 -> f3\n";

    game.makeMove("g1", "f3");

    game.displayBoard();

    return 0;
}

