/*
 * TicTacToe.cpp - Tic Tac Toe with Minimax AI
 *
 * AI Implementation:
 * - Uses Minimax algorithm for optimal play
 * - Alpha-Beta pruning for efficiency
 * - Depth-limited to prefer shorter winning paths
 *
 * Scoring:
 * - Win:  150 points (fewer moves = bonus)
 * - Draw:  75 points (impressive against optimal AI)
 * - Loss:  20 points (participation)
 */

#include "TicTacToe.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>

TicTacToeGame::TicTacToeGame() : playerSymbol('X'), aiSymbol('O') {
    initBoard();
}

void TicTacToeGame::initBoard() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = ' ';
}

void TicTacToeGame::printBoard() {
    std::cout << "\n";
    std::cout << "      1   2   3\n";
    std::cout << "    +---+---+---+\n";
    for (int i = 0; i < 3; i++) {
        std::cout << "  " << (i + 1) << " |";
        for (int j = 0; j < 3; j++) {
            char c = board[i][j];
            if (c == ' ')
                std::cout << "   |";
            else
                std::cout << " " << c << " |";
        }
        std::cout << "\n";
        if (i < 2)
            std::cout << "    +---+---+---+\n";
    }
    std::cout << "    +---+---+---+\n";
}

bool TicTacToeGame::isFull() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == ' ') return false;
    return true;
}

char TicTacToeGame::checkWinner() {
    // Check rows
    for (int i = 0; i < 3; i++)
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return board[i][0];

    // Check columns
    for (int j = 0; j < 3; j++)
        if (board[0][j] != ' ' && board[0][j] == board[1][j] && board[1][j] == board[2][j])
            return board[0][j];

    // Check diagonals
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return board[0][0];
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return board[0][2];

    return ' ';
}

bool TicTacToeGame::isValidMove(int row, int col) {
    return row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ';
}

// ──────────────────────────────────────────────
// Minimax with Alpha-Beta Pruning
// Time: O(b^d) reduced significantly by pruning
// b = branching factor (~9), d = depth (~9)
// ──────────────────────────────────────────────
int TicTacToeGame::minimax(bool isMaximizing, int depth, int alpha, int beta) {
    char winner = checkWinner();

    // Terminal states
    if (winner == aiSymbol) return 10 - depth;  // AI wins (prefer shorter paths)
    if (winner == playerSymbol) return depth - 10; // Player wins
    if (isFull()) return 0; // Draw

    if (isMaximizing) {
        int maxEval = -1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = aiSymbol;
                    int eval = minimax(false, depth + 1, alpha, beta);
                    board[i][j] = ' ';
                    maxEval = std::max(maxEval, eval);
                    alpha = std::max(alpha, eval);
                    if (beta <= alpha) break; // Alpha-beta pruning
                }
            }
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = 1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = playerSymbol;
                    int eval = minimax(true, depth + 1, alpha, beta);
                    board[i][j] = ' ';
                    minEval = std::min(minEval, eval);
                    beta = std::min(beta, eval);
                    if (beta <= alpha) break;
                }
            }
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

void TicTacToeGame::aiMove() {
    int bestScore = -1000;
    int bestRow = -1, bestCol = -1;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = aiSymbol;
                int score = minimax(false, 0, -1000, 1000);
                board[i][j] = ' ';
                if (score > bestScore) {
                    bestScore = score;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }

    if (bestRow != -1 && bestCol != -1) {
        board[bestRow][bestCol] = aiSymbol;
        std::cout << "  AI plays: (" << bestRow + 1 << ", " << bestCol + 1 << ")\n";
    }
}

double TicTacToeGame::play() {
    initBoard();

    std::cout << "\n==========================================\n";
    std::cout << "     TIC TAC TOE vs MINIMAX AI\n";
    std::cout << "==========================================\n";
    std::cout << "  You are X, AI is O\n";
    std::cout << "  Enter moves as: row col (e.g., 1 2)\n";
    std::cout << "  AI uses Minimax + Alpha-Beta Pruning\n";
    std::cout << "==========================================\n";

    int moveCount = 0;
    bool playerTurn = true;

    while (true) {
        printBoard();
        char winner = checkWinner();

        if (winner == playerSymbol) {
            double score = 150.0 + (9 - moveCount) * 5; // Bonus for fewer moves
            std::cout << "\n  YOU WIN! Score: " << score << "\n";
            return score;
        }
        if (winner == aiSymbol) {
            std::cout << "\n  AI WINS! Better luck next time.\n";
            std::cout << "  Score: 20 (participation)\n";
            return 20.0;
        }
        if (isFull()) {
            std::cout << "\n  DRAW! Impressive against optimal AI!\n";
            std::cout << "  Score: 75\n";
            return 75.0;
        }

        if (playerTurn) {
            std::cout << "\n  Your move (row col): ";
            std::string input;
            std::getline(std::cin, input);

            int row = -1, col = -1;
            try {
                size_t space = input.find(' ');
                if (space != std::string::npos) {
                    row = std::stoi(input.substr(0, space)) - 1;
                    col = std::stoi(input.substr(space + 1)) - 1;
                }
            } catch (...) {}

            if (!isValidMove(row, col)) {
                std::cout << "  Invalid move! Try again.\n";
                continue;
            }

            board[row][col] = playerSymbol;
            moveCount++;
            playerTurn = false;
        } else {
            std::cout << "\n  AI is thinking...\n";
            aiMove();
            moveCount++;
            playerTurn = true;
        }
    }
}

std::string TicTacToeGame::getName() const {
    return "Tic Tac Toe";
}
