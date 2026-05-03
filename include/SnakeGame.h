/*
 * SnakeGame.h - Snake Game with Grid + Deque Logic
 *
 * Uses a 2D grid and deque for the snake body.
 * Provides non-blocking keyboard input on Windows.
 */

#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include <string>
#include <deque>
#include <utility>

class SnakeGame {
private:
    static const int WIDTH = 20;
    static const int HEIGHT = 15;

    char grid[15][20];
    std::deque<std::pair<int, int>> snake;  // Deque for O(1) head/tail operations
    std::pair<int, int> food;
    int direction; // 0=UP, 1=RIGHT, 2=DOWN, 3=LEFT
    int score;
    bool gameOver;

    void initGame();
    void placeFood();
    void drawGrid();
    void moveSnake();
    void clearScreen();
    char getInput();

public:
    SnakeGame();
    double play();
    std::string getName() const;
};

#endif // SNAKE_GAME_H
