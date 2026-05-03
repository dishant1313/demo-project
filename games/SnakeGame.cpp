/*
 * SnakeGame.cpp - Snake Game Implementation
 *
 * Data Structures Used:
 * - 2D char array for the game grid
 * - std::deque for the snake body (O(1) push_front/pop_back)
 *
 * The snake grows when eating food, game ends on wall/self collision.
 * Uses platform-specific non-blocking input (Windows: _kbhit/_getch).
 *
 * Scoring: 10 * food_eaten + length_bonus
 */

#include "SnakeGame.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

SnakeGame::SnakeGame() : direction(1), score(0), gameOver(false) {
    srand((unsigned int)time(nullptr));
}

void SnakeGame::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

char SnakeGame::getInput() {
#ifdef _WIN32
    if (_kbhit()) {
        char ch = _getch();
        if (ch == 0 || ch == -32) {
            ch = _getch(); // Get the actual key code
            switch (ch) {
                case 72: return 'w'; // Up arrow
                case 80: return 's'; // Down arrow
                case 75: return 'a'; // Left arrow
                case 77: return 'd'; // Right arrow
            }
        }
        return ch;
    }
    return 0;
#else
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) return (char)ch;
    return 0;
#endif
}

void SnakeGame::initGame() {
    // Clear grid
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            grid[i][j] = ' ';

    // Initialize snake in the middle (using deque)
    snake.clear();
    int startX = WIDTH / 2;
    int startY = HEIGHT / 2;
    snake.push_front({startY, startX});
    snake.push_back({startY, startX - 1});
    snake.push_back({startY, startX - 2});

    direction = 1; // Start moving right
    score = 0;
    gameOver = false;

    placeFood();
}

void SnakeGame::placeFood() {
    int x, y;
    bool onSnake;
    do {
        x = 1 + rand() % (WIDTH - 2);
        y = 1 + rand() % (HEIGHT - 2);
        onSnake = false;
        for (auto& segment : snake) {
            if (segment.first == y && segment.second == x) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);

    food = {y, x};
}

void SnakeGame::drawGrid() {
    clearScreen();

    std::cout << "  SNAKE GAME | Score: " << score
              << " | Length: " << snake.size() << "\n";
    std::cout << "  Controls: W/A/S/D or Arrow Keys | Q to Quit\n\n";

    // Top border
    std::cout << "  ";
    for (int j = 0; j < WIDTH; j++) std::cout << "##";
    std::cout << "\n";

    for (int i = 0; i < HEIGHT; i++) {
        std::cout << "  ##"; // Left wall
        for (int j = 0; j < WIDTH - 2; j++) {
            int gj = j + 1;

            bool isHead = (snake.front().first == i && snake.front().second == gj);
            bool isBody = false;
            for (size_t s = 1; s < snake.size(); s++) {
                if (snake[s].first == i && snake[s].second == gj) {
                    isBody = true;
                    break;
                }
            }
            bool isFood = (food.first == i && food.second == gj);

            if (isHead)
                std::cout << "@@"; // Snake head
            else if (isBody)
                std::cout << "oo"; // Snake body
            else if (isFood)
                std::cout << "**"; // Food
            else
                std::cout << "  ";
        }
        std::cout << "##\n"; // Right wall
    }

    // Bottom border
    std::cout << "  ";
    for (int j = 0; j < WIDTH; j++) std::cout << "##";
    std::cout << "\n";
}

void SnakeGame::moveSnake() {
    auto head = snake.front();
    int newY = head.first;
    int newX = head.second;

    switch (direction) {
        case 0: newY--; break; // UP
        case 1: newX++; break; // RIGHT
        case 2: newY++; break; // DOWN
        case 3: newX--; break; // LEFT
    }

    // Check wall collision
    if (newX <= 0 || newX >= WIDTH - 1 || newY < 0 || newY >= HEIGHT) {
        gameOver = true;
        return;
    }

    // Check self collision (using deque traversal)
    for (auto& segment : snake) {
        if (segment.first == newY && segment.second == newX) {
            gameOver = true;
            return;
        }
    }

    // Move: push new head to front of deque
    snake.push_front({newY, newX});

    // Check food collision
    if (newY == food.first && newX == food.second) {
        score += 10;
        placeFood(); // Don't pop_back — snake grows
    } else {
        snake.pop_back(); // Remove tail — snake moves
    }
}

double SnakeGame::play() {
    std::cout << "\n==========================================\n";
    std::cout << "          SNAKE GAME\n";
    std::cout << "==========================================\n";
    std::cout << "  Uses Deque + Grid Data Structures\n";
    std::cout << "  Controls: W/A/S/D or Arrow Keys\n";
    std::cout << "  Eat food (**) to grow!\n";
    std::cout << "==========================================\n";
    std::cout << "\n  Press ENTER to start...";
    std::string dummy;
    std::getline(std::cin, dummy);

    initGame();

    while (!gameOver) {
        drawGrid();

        char input = getInput();
        if (input == 'q' || input == 'Q') break;

        // Prevent 180-degree turns
        if ((input == 'w' || input == 'W') && direction != 2) direction = 0;
        else if ((input == 'd' || input == 'D') && direction != 3) direction = 1;
        else if ((input == 's' || input == 'S') && direction != 0) direction = 2;
        else if ((input == 'a' || input == 'A') && direction != 1) direction = 3;

        moveSnake();

#ifdef _WIN32
        Sleep(150); // Game speed (ms)
#else
        usleep(150000);
#endif
    }

    drawGrid();

    double finalScore = score + (int)snake.size() * 2.0;

    std::cout << "\n  == GAME OVER ==\n";
    std::cout << "  Food eaten:   " << score / 10 << "\n";
    std::cout << "  Snake length: " << snake.size() << "\n";
    std::cout << "  Final Score:  " << std::fixed << std::setprecision(1)
              << finalScore << "\n\n";
    std::cout << "  Press ENTER to continue...";
    std::getline(std::cin, dummy);

    return finalScore;
}

std::string SnakeGame::getName() const {
    return "Snake";
}
