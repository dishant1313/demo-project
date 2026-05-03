/*
 * TicTacToe.h - Tic Tac Toe with Minimax AI
 *
 * AI uses the Minimax algorithm with alpha-beta pruning.
 * NOT random moves — optimal adversarial play.
 */

#ifndef TIC_TAC_TOE_H
#define TIC_TAC_TOE_H

#include <string>

class TicTacToeGame {
private:
    char board[3][3];
    char playerSymbol;
    char aiSymbol;

    void initBoard();
    void printBoard();
    bool isFull();
    char checkWinner();
    bool isValidMove(int row, int col);

    // Minimax AI with Alpha-Beta Pruning
    int minimax(bool isMaximizing, int depth, int alpha, int beta);
    void aiMove();

public:
    TicTacToeGame();
    double play();
    std::string getName() const;
};

#endif // TIC_TAC_TOE_H
