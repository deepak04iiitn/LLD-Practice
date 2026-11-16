#include <bits/stdc++.h>
using namespace std;

enum class PlayerStatus {
    WIN,
    LOSE,
    NA
};

enum class GameStatus {
    START,
    IN_PROGRESS,
    END
};


class Player {
    private:
        string name;
        int row;
        int col;
        PlayerStatus status;
        bool isTurn;

    public:
        Player(string name , int r , int c) : name(name) , row(r) , col(c) {
            status = PlayerStatus::NA;
            isTurn = false;
        };

        void setTurn(bool turn) {
            isTurn = turn;
        }

        string getName() {
            return name;
        }

        int getRow() {
            return row;
        }

        void setRow(int r) {
            row = r;
        }

        int getCol() {
            return col;
        }

        void setCol(int c) {
            col = c;
        }

        PlayerStatus getStatus() {
            return status;
        }

        void setStatus(PlayerStatus st) {
            status = st;
        }

        bool getIsTurn() {
            return isTurn;
        }
};


class Snake {
    private:
        int headRow;
        int headCol;
        int tailRow;
        int tailCol;

    public:
        Snake(int hr , int hc , int tr , int tc) : headRow(hr) , headCol(hc) , tailRow(tr) , tailCol(tc) {};

        int getHeadRow() {
            return headRow;
        }

        int getHeadCol() {
            return headCol;
        }

        int getTailRow() {
            return tailRow;
        }

        int getTailCol() {
            return tailCol;
        }
};


class Ladder {
    private:
        int startRow;
        int startCol;
        int endRow;
        int endCol;

    public:
        Ladder(int sr , int sc , int er , int ec) : startRow(sr) , startCol(sc) , endRow(er) , endCol(ec) {};

        int getStartRow() {
            return startRow;
        }

        int getStartCol() {
            return startCol;
        }

        int getEndRow() {
            return endRow;
        }

        int getEndCol() {
            return endCol;
        }
};


class Board {
    private:
        int rows;
        int cols;
        vector<Snake*> snakes;
        vector<Ladder*> ladders;
        vector<vector<int>> matrix;
        vector<pair<int,int>> position; // cell number -> (row, col)

    public:
        Board(int r , int c , vector<Snake*> s , vector<Ladder*> l) : rows(r) , cols(c) , snakes(s) , ladders(l) {
            initializeBoard();
        };

        // Cell 1 is bottom-left, numbering zig-zags upward. Last cell is on row 0.
        void initializeBoard() {
            matrix.assign(rows , vector<int>(cols , 0));
            position.assign(rows * cols + 1 , {-1 , -1});

            int cnt = 1;
            bool leftToRight = true;

            for(int i = 0 ; i < rows ; i++) {
                for(int j = 0 ; j < cols ; j++) {
                    int r = rows - 1 - i;
                    int c = leftToRight ? j : cols - 1 - j;
                    matrix[r][c] = cnt;
                    position[cnt] = {r , c};
                    cnt++;
                }

                leftToRight = !leftToRight;
            }
        }

        int getRows() {
            return rows;
        }

        int getCols() {
            return cols;
        }

        int getTotalCells() {
            return rows * cols;
        }

        int getNumber(int r , int c) {
            return matrix[r][c];
        }

        pair<int,int> getCoordinates(int num) {
            return position[num];
        }

        vector<Snake*> getSnakes() {
            return snakes;
        }

        vector<Ladder*> getLadders() {
            return ladders;
        }
};


class GameService {
    private:
        vector<Player*> players;
        Board* board;
        GameStatus gameStatus;
        int turnIdx;
        int winners;

        int rollDice() {
            return rand() % 6 + 1;
        }

        // returns true if player landed on a snake head / ladder start and was moved
        bool applySnakeOrLadder(Player* pl) {
            int r = pl -> getRow();
            int c = pl -> getCol();

            for(Snake* s : board -> getSnakes()) {
                if(s -> getHeadRow() == r && s -> getHeadCol() == c) {
                    pl -> setRow(s -> getTailRow());
                    pl -> setCol(s -> getTailCol());
                    cout << "  Snake! " << pl -> getName() << " slides down to "
                         << board -> getNumber(s -> getTailRow() , s -> getTailCol()) << endl;
                    return true;
                }
            }

            for(Ladder* l : board -> getLadders()) {
                if(l -> getStartRow() == r && l -> getStartCol() == c) {
                    pl -> setRow(l -> getEndRow());
                    pl -> setCol(l -> getEndCol());
                    cout << "  Ladder! " << pl -> getName() << " climbs up to "
                         << board -> getNumber(l -> getEndRow() , l -> getEndCol()) << endl;
                    return true;
                }
            }

            return false;
        }

        void makeMove(Player* pl) {
            int dice = rollDice();
            int cur = board -> getNumber(pl -> getRow() , pl -> getCol());
            int next = cur + dice;

            cout << pl -> getName() << " rolled " << dice << " (" << cur << " -> ";

            if(next > board -> getTotalCells()) {
                cout << cur << ") needs exact roll" << endl;
                return;
            }

            pair<int,int> pos = board -> getCoordinates(next);
            
            pl -> setRow(pos.first);
            pl -> setCol(pos.second);
            cout << next << ")" << endl;

            // snakes/ladders must not chain: one jump per move
            applySnakeOrLadder(pl);

            if(board -> getNumber(pl -> getRow() , pl -> getCol()) == board -> getTotalCells()) {
                pl -> setStatus(PlayerStatus::WIN);
                winners++;
                cout << "*** " << pl -> getName() << " wins! ***" << endl;
            }
        }

    public:
        GameService(Board* b , vector<Player*> p) : players(p) , board(b) {
            gameStatus = GameStatus::START;
            turnIdx = 0;
            winners = 0;
        };

        GameStatus getGameStatus() {
            return gameStatus;
        }

        void play() {
            gameStatus = GameStatus::IN_PROGRESS;
            int n = players.size();

            // game ends at the first winner
            while(winners < 1) {
                Player* pl = players[turnIdx];
                turnIdx = (turnIdx + 1) % n;

                if(pl -> getStatus() != PlayerStatus::NA) continue;

                pl -> setTurn(true);
                makeMove(pl);
                pl -> setTurn(false);
            }

            for(Player* pl : players) {
                if(pl -> getStatus() == PlayerStatus::NA) pl -> setStatus(PlayerStatus::LOSE);
            }

            gameStatus = GameStatus::END;
            cout << "\nGame over." << endl;
            for(Player* pl : players) {
                cout << pl -> getName() << ": " << (pl -> getStatus() == PlayerStatus::WIN ? "WIN" : "LOSE") << endl;
            }
        }
};


int main() {
    srand(time(nullptr));

    int rows = 10 , cols = 10;
    // Board is built first so snakes/ladders can be defined by cell number
    Board temp(rows , cols , {} , {});

    auto cell = [&](int num) { return temp.getCoordinates(num); };

    vector<pair<int,int>> snakePairs = {{99 , 54} , {70 , 55} , {52 , 42} , {25 , 2} , {95 , 72}};
    vector<pair<int,int>> ladderPairs = {{6 , 25} , {11 , 40} , {60 , 85} , {46 , 90} , {17 , 69}};

    vector<Snake*> snakes;
    vector<Ladder*> ladders;

    for(auto &p : snakePairs) {
        auto h = cell(p.first);
        auto t = cell(p.second);
        snakes.push_back(new Snake(h.first , h.second , t.first , t.second));
    }

    for(auto &p : ladderPairs) {
        auto s = cell(p.first);
        auto e = cell(p.second);
        ladders.push_back(new Ladder(s.first , s.second , e.first , e.second));
    }

    Board* board = new Board(rows , cols , snakes , ladders);

    // everyone starts at cell 1 (bottom-left)
    vector<Player*> players = {
        new Player("Alice" , rows - 1 , 0),
        new Player("Bob" , rows - 1 , 0),
        new Player("Charlie" , rows - 1 , 0)
    };

    GameService game(board , players);
    game.play();

    return 0;
}
