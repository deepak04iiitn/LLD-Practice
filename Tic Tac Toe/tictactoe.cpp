#include <bits/stdc++.h>
using namespace std;

// No. of Players = 2
// Size of the board = n

enum class GameState {
    IN_PROGRESS,
    PLAYER1_WON,
    PLAYER2_WON,
    DRAW
};


class TicTacToe {
    private:
        int size;
        vector<vector<char>>board;
        bool isPlayer1;
        GameState state;
        int movesPlayed;

    public:
        TicTacToe(int size , bool isPlayer1 = true) : size(size) , isPlayer1(isPlayer1) {
            initialize();
        }

        void initialize() {
            board = vector<vector<char>>(size, vector<char>(size, ' '));
            state = GameState::IN_PROGRESS;
            movesPlayed = 0;
            printBoard();
        }

        void printBoard() {
            for(int i = 0; i < size; i++) {
                for(int j = 0; j < size; j++) {
                    cout << " " << board[i][j] << " ";
                    if(j != size - 1) cout << "|";
                }

                cout << "\n";

                // Print separator
                if(i != size - 1) {
                    for(int j = 0; j < size; j++) {
                        cout << "---";
                        if(j != size - 1) cout << "+";
                    }
                    cout << "\n";
                }
            }
        }

        GameState getState() const {
            return state;
        }

        bool playerChance(int row, int col) {
            if(state != GameState::IN_PROGRESS) {
                cout << "Game is already over." << endl;
                return false;
            }

            if(row < 0 || row >= size || col < 0 || col >= size) {
                cout << "Invalid cell." << endl;
                return false;
            }

            if(board[row][col] != ' ') {
                cout << "Cell (" << row << ", " << col << ") is already occupied." << endl;
                return false;
            }

            char symbol = isPlayer1 ? 'O' : 'X';
            int player = isPlayer1 ? 1 : 2;
            board[row][col] = symbol;
            movesPlayed++;

            if(isWinningMove(row, col, symbol)) {
                state = isPlayer1 ? GameState::PLAYER1_WON : GameState::PLAYER2_WON;
            } else if(movesPlayed == size * size) {
                state = GameState::DRAW;
            } else {
                isPlayer1 = !isPlayer1;
            }

            cout << "\nPlayer " << player << " (" << symbol << ") -> (" << row << ", " << col << ")" << endl;
            printBoard();
            return true;
        }

        // Only the row, column and both diagonals passing through the
        // last move can possibly have changed, so checking those is enough.
        bool isWinningMove(int row, int col, char symbol) {
            bool rowWin = true, colWin = true;
            bool diagWin = true, antiDiagWin = true;

            for(int i = 0; i < size; i++) {
                if(board[row][i] != symbol) rowWin = false;
                if(board[i][col] != symbol) colWin = false;
                if(board[i][i] != symbol) diagWin = false;
                if(board[i][size - 1 - i] != symbol) antiDiagWin = false;
            }

            return rowWin || colWin || diagWin || antiDiagWin;
        }
};


// ---------------- Demo ----------------

int main() {
    TicTacToe game(3);

    game.playerChance(0, 0); // O
    game.playerChance(1, 0); // X
    game.playerChance(0, 0); // invalid - cell already occupied, turn does not advance
    game.playerChance(0, 1); // O
    game.playerChance(1, 1); // X
    game.playerChance(0, 2); // O completes the top row and wins

    switch(game.getState()) {
        case GameState::PLAYER1_WON: cout << "Player 1 (O) wins!" << endl; break;
        case GameState::PLAYER2_WON: cout << "Player 2 (X) wins!" << endl; break;
        case GameState::DRAW: cout << "It's a draw!" << endl; break;
        case GameState::IN_PROGRESS: cout << "Game still in progress." << endl; break;
    }

    return 0;
}
