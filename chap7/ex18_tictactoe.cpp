/*
# TicTacToe accepts (no) arguments
# it allows two players to play tic-tac-toe on a 3x3 board
# it outputs the board after each move and declares a winner or tie, and offers replay
*/

#include <iostream>
#include <array>
#include <string>

using namespace std;

void printBoard(const array<array<char,3>,3>& b) {
    cout << "Columns\n";
    cout << "  1 2 3\n";
    for (int r=0;r<3;++r) {
        cout << "Row " << (r+1) << ": ";
        for (int c=0;c<3;++c) {
            cout << b[r][c];
            if (c < 2) cout << ' ';
        }
        cout << '\n';
    }
}

bool checkWin(const array<array<char,3>,3>& b, char p) {
    for (int i=0;i<3;++i) {
        if (b[i][0]==p && b[i][1]==p && b[i][2]==p) return true;
        if (b[0][i]==p && b[1][i]==p && b[2][i]==p) return true;
    }
    if (b[0][0]==p && b[1][1]==p && b[2][2]==p) return true;
    if (b[0][2]==p && b[1][1]==p && b[2][0]==p) return true;
    return false;
}

int main(){
    while (true) {
        array<array<char,3>,3> board;
        for (auto &r: board) for (auto &c: r) c = '*';
        int moves = 0;
        char player = 'X';
        bool gameOver = false;
        while (!gameOver) {
            printBoard(board);
            cout << "Player " << player << "'s turn." << '\n';
            cout << "Enter a row and column to place an " << player << "." << '\n';
            int r,c;
            cout << "Row: ";
            if (!(cin >> r)) {
                cin.clear(); string junk; getline(cin, junk);
                cout << "Invalid input; please enter a number for Row." << '\n';
                continue;
            }
            cout << "Column: ";
            if (!(cin >> c)) {
                cin.clear(); string junk; getline(cin, junk);
                cout << "Invalid input; please enter a number for Column." << '\n';
                continue;
            }
            if (r<1 || r>3) { cout << "Invalid Row!" << '\n'; continue; }
            if (c<1 || c>3) { cout << "Invalid Column!" << '\n'; continue; }
            if (board[r-1][c-1] != '*') { cout << "That spot is already taken!" << '\n'; continue; }
            board[r-1][c-1] = player; ++moves;
            if (checkWin(board, player)) {
                printBoard(board);
                if (player == 'X') cout << "Player 1 (X) WINS!!!!!" << '\n';
                else cout << "Player 2 (O) WINS!!!!!" << '\n';
                gameOver = true;
            } else if (moves == 9) {
                printBoard(board);
                cout << "The game is a tie.\n";
                gameOver = true;
            } else {
                player = (player == 'X') ? 'O' : 'X';
            }
        }
    cout << "Play again? (y/n): ";
    char ans;
    if (!(cin >> ans)) { break; }
    string rest; getline(cin, rest); // consume remainder of line
    if (ans != 'y' && ans != 'Y') break;
    }
    return 0;
}
