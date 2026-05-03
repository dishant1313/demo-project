/*
 * FlappyBird.cpp - Flappy Bird (Clean Buffer-Rendered Terminal Game)
 *
 * Rendering: Builds each frame in a 2D char buffer, then prints
 * the entire buffer at once for flicker-free, clean output.
 *
 * Data Structures:
 * - Custom Min-Heap Priority Queue: schedules difficulty ramps
 *   (speed increases, gap shrinks, bonus points) at set tick intervals
 * - Vector: tracks active pipes on screen
 * - Physics: gravity + velocity model for bird flight
 *
 * Controls: SPACE or UP = flap,  Q = quit,  R = retry
 */

#include "FlappyBird.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <sstream>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

// ══════════════════════════════════════════════
// Constructor
// ══════════════════════════════════════════════
FlappyBirdGame::FlappyBirdGame()
    : birdY(9), vel(0), birdX(6), gravity(0.3), flapPow(-1.5),
      score(0), ticks(0), over(false), started(false),
      speed(1), gapSize(7), nextSpawn(0), eventTimer(0) {
    srand((unsigned int)time(nullptr));
}

// ══════════════════════════════════════════════
// PRIORITY QUEUE (Min-Heap)
// ══════════════════════════════════════════════
void FlappyBirdGame::heapUp(int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (eventQueue[p].time > eventQueue[i].time) {
            std::swap(eventQueue[p], eventQueue[i]);
            i = p;
        } else break;
    }
}

void FlappyBirdGame::heapDown(int i) {
    int sz = (int)eventQueue.size();
    while (true) {
        int sm = i, l = 2*i+1, r = 2*i+2;
        if (l < sz && eventQueue[l].time < eventQueue[sm].time) sm = l;
        if (r < sz && eventQueue[r].time < eventQueue[sm].time) sm = r;
        if (sm != i) { std::swap(eventQueue[i], eventQueue[sm]); i = sm; }
        else break;
    }
}

void FlappyBirdGame::pushEv(const GameEvent& e) {
    eventQueue.push_back(e);
    heapUp((int)eventQueue.size() - 1);
}

GameEvent FlappyBirdGame::popEv() {
    GameEvent top = eventQueue[0];
    eventQueue[0] = eventQueue.back();
    eventQueue.pop_back();
    if (!eventQueue.empty()) heapDown(0);
    return top;
}

bool FlappyBirdGame::hasEv() const { return !eventQueue.empty(); }

// ══════════════════════════════════════════════
// INPUT
// ══════════════════════════════════════════════
char FlappyBirdGame::getKey() {
#ifdef _WIN32
    if (_kbhit()) {
        char ch = _getch();
        if (ch == 0 || ch == -32) {
            ch = _getch();
            if (ch == 72) return 'F'; // Up
            return 0;
        }
        if (ch == ' ') return 'F';
        if (ch == 'q' || ch == 'Q') return 'Q';
        if (ch == 'r' || ch == 'R') return 'R';
        return 0;
    }
    return 0;
#else
    struct termios oldt, newt;
    int ch, oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch == 27) { getchar(); int a = getchar(); if (a == 'A') return 'F'; }
    if (ch == ' ') return 'F';
    if (ch == 'q' || ch == 'Q') return 'Q';
    if (ch == 'r' || ch == 'R') return 'R';
    return 0;
#endif
}

// ══════════════════════════════════════════════
// BUFFER OPERATIONS (flicker-free rendering)
// ══════════════════════════════════════════════
void FlappyBirdGame::bufClear() {
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++) buf[r][c] = ' ';
        buf[r][W] = '\0';
    }
}

void FlappyBirdGame::bufSet(int r, int c, char ch) {
    if (r >= 0 && r < H && c >= 0 && c < W) buf[r][c] = ch;
}

void FlappyBirdGame::bufStr(int r, int c, const std::string& s) {
    for (int i = 0; i < (int)s.size() && c + i < W; i++) {
        if (c + i >= 0) buf[r][c + i] = s[i];
    }
}

// ══════════════════════════════════════════════
// INIT
// ══════════════════════════════════════════════
void FlappyBirdGame::init() {
    birdY = H / 2.0;
    vel = 0;
    score = 0;
    ticks = 0;
    over = false;
    started = false;
    speed = 1;
    gravity = 0.3;
    gapSize = 7;
    nextSpawn = 35;
    eventMsg = "";
    eventTimer = 0;
    pipes.clear();
    eventQueue.clear();
    scheduleEvents();

#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci; ci.dwSize = 100; ci.bVisible = FALSE;
    SetConsoleCursorInfo(h, &ci);
    system("cls");
#else
    system("clear");
#endif
}

void FlappyBirdGame::scheduleEvents() {
    pushEv({60,   "speed",  1, "SPEED UP!"});
    pushEv({140,  "speed",  1, "FASTER!"});
    pushEv({240,  "speed",  1, "TURBO!"});
    pushEv({350,  "speed",  1, "INSANE!"});

    pushEv({90,   "shrink", 1, "GAPS SHRINK!"});
    pushEv({180,  "shrink", 1, "NARROW!"});
    pushEv({300,  "shrink", 1, "TINY GAPS!"});

    pushEv({50,   "bonus",  5,  "+5 BONUS!"});
    pushEv({110,  "bonus",  10, "+10 BONUS!"});
    pushEv({200,  "bonus",  20, "+20 BONUS!"});
    pushEv({320,  "bonus",  30, "MEGA +30!"});
}

// ══════════════════════════════════════════════
// PIPES
// ══════════════════════════════════════════════
void FlappyBirdGame::spawn() {
    Pipe p;
    p.x = W - 1;
    p.gapSize = gapSize;
    p.gapTop = 2 + rand() % (H - p.gapSize - 3);
    p.scored = false;
    p.active = true;
    pipes.push_back(p);
}

// ══════════════════════════════════════════════
// UPDATE
// ══════════════════════════════════════════════
void FlappyBirdGame::update() {
    if (!started) return;

    // Bird physics
    vel += gravity;
    if (vel > 2.5) vel = 2.5;
    if (vel < -2.5) vel = -2.5;
    birdY += vel;

    // Tick event message countdown
    if (eventTimer > 0) eventTimer--;

    // Move pipes left
    for (auto& p : pipes) {
        if (!p.active) continue;
        p.x -= (int)speed;
        if (!p.scored && p.x + 3 < birdX) {
            p.scored = true;
            score += 10;
        }
        if (p.x < -3) p.active = false;
    }
    pipes.erase(std::remove_if(pipes.begin(), pipes.end(),
                [](const Pipe& p) { return !p.active; }), pipes.end());

    // Spawn pipes
    if (ticks >= nextSpawn) {
        spawn();
        nextSpawn = ticks + 14 + rand() % 6;
    }

    // Process PQ events
    processEv();
    ticks++;
}

bool FlappyBirdGame::collide() {
    int by = (int)birdY;
    if (by < 1 || by >= H - 1) return true;

    for (auto& p : pipes) {
        if (!p.active) continue;
        // Bird occupies column birdX
        if (birdX >= p.x && birdX <= p.x + 2) {
            if (by <= p.gapTop || by >= p.gapTop + p.gapSize) return true;
        }
    }
    return false;
}

void FlappyBirdGame::processEv() {
    while (hasEv() && eventQueue[0].time <= ticks) {
        GameEvent e = popEv();
        if (e.type == "speed") {
            speed = std::min(3.0, speed + 0.3);
            eventMsg = e.description;
            eventTimer = 25;
        } else if (e.type == "shrink") {
            gapSize = std::max(4, gapSize - 1);
            eventMsg = e.description;
            eventTimer = 25;
        } else if (e.type == "bonus") {
            score += e.severity;
            eventMsg = e.description;
            eventTimer = 25;
        }
    }
}

// ══════════════════════════════════════════════
// RENDER (buffer-based, flicker-free)
// ══════════════════════════════════════════════
void FlappyBirdGame::render() {
    bufClear();

    int by = (int)birdY;

    // Draw ground
    for (int c = 0; c < W; c++) {
        bufSet(H - 1, c, (c % 2 == 0) ? '~' : '_');
    }

    // Draw ceiling
    for (int c = 0; c < W; c++) {
        bufSet(0, c, '-');
    }

    // Draw pipes
    for (auto& p : pipes) {
        if (!p.active) continue;
        for (int r = 1; r < H - 1; r++) {
            bool inGap = (r > p.gapTop && r < p.gapTop + p.gapSize);
            if (!inGap) {
                // Pipe body
                bufSet(r, p.x,     '[');
                bufSet(r, p.x + 1, '#');
                bufSet(r, p.x + 2, ']');
            }
            // Gap edges (pipe lips)
            if (r == p.gapTop) {
                bufSet(r, p.x,     '[');
                bufSet(r, p.x + 1, '=');
                bufSet(r, p.x + 2, ']');
            }
            if (r == p.gapTop + p.gapSize) {
                bufSet(r, p.x,     '[');
                bufSet(r, p.x + 1, '=');
                bufSet(r, p.x + 2, ']');
            }
        }
    }

    // Draw bird (overwrites anything underneath)
    if (by >= 1 && by < H - 1) {
        if (vel < -0.5) {
            // Flapping up
            bufSet(by, birdX, '>');
            bufSet(by - 1, birdX, '^');
        } else if (vel > 1.5) {
            // Diving
            bufSet(by, birdX, '>');
            bufSet(by + 1 < H - 1 ? by + 1 : by, birdX, 'v');
        } else {
            // Gliding
            bufSet(by, birdX, '>');
        }
        // Eye
        bufSet(by, birdX - 1, 'o');
    }

    // ── Output entire frame at once ──
#ifdef _WIN32
    COORD coord = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#endif

    // Build output string (single cout call = no flicker)
    std::ostringstream out;

    out << "\n";
    out << "    ########################################\n";
    out << "    #       F L A P P Y   B I R D          #\n";
    out << "    #   SPACE/UP: Flap          Q: Quit    #\n";
    out << "    ########################################\n";

    // Score bar
    out << "    # Score: " << std::setw(4) << std::left << score
        << "  Pipes: " << std::setw(3) << std::left << (score / 10)
        << "  Speed: " << std::fixed << std::setprecision(1) << speed
        << "        #\n";
    out << "    ########################################\n";

    // Game area with borders
    for (int r = 0; r < H; r++) {
        out << "    #";
        out << buf[r];
        out << "#\n";
    }

    out << "    ########################################\n";

    // Bottom status
    if (!started && !over) {
        out << "    #   Press SPACE or UP to start!       #\n";
    } else if (over) {
        out << "    #  GAME OVER! Score:" << std::setw(4) << std::left << score << " R=Retry Q=Quit #\n";
    } else if (eventTimer > 0 && !eventMsg.empty()) {
        out << "    #   >> " << std::setw(30) << std::left << eventMsg << "#\n";
    } else {
        std::string diff;
        if (ticks < 60) diff = "Easy";
        else if (ticks < 140) diff = "Medium";
        else if (ticks < 240) diff = "Hard";
        else diff = "INSANE";
        out << "    #   Difficulty: " << std::setw(21) << std::left << diff << "#\n";
    }

    out << "    ########################################\n";

    std::cout << out.str();
}

// ══════════════════════════════════════════════
// MAIN GAME LOOP
// ══════════════════════════════════════════════
double FlappyBirdGame::play() {
    std::cout << "\n  ================================================\n";
    std::cout << "           F L A P P Y   B I R D\n";
    std::cout << "  ================================================\n";
    std::cout << "  Uses Priority Queue (Min-Heap) for Event System\n";
    std::cout << "\n";
    std::cout << "  Controls:\n";
    std::cout << "    SPACE or UP Arrow  =  Flap (fly up)\n";
    std::cout << "    R                  =  Retry\n";
    std::cout << "    Q                  =  Quit\n";
    std::cout << "\n";
    std::cout << "  Rules:\n";
    std::cout << "    - Tap to flap and stay in the air\n";
    std::cout << "    - Fly through the gaps in pipes\n";
    std::cout << "    - Don't hit pipes, ceiling, or ground\n";
    std::cout << "    - Difficulty ramps up over time (via PQ)\n";
    std::cout << "  ================================================\n";
    std::cout << "\n  Press ENTER to begin...";
    std::string dummy;
    std::getline(std::cin, dummy);

    init();

    while (true) {
        char key = getKey();

        if (key == 'Q') break;

        if (over) {
            // Retry on R or SPACE
            if (key == 'F' || key == 'R') {
                init();
                continue;
            }
            render();
#ifdef _WIN32
            Sleep(50);
#else
            usleep(50000);
#endif
            continue;
        }

        if (key == 'F') {
            if (!started) started = true;
            vel = flapPow;
        }

        update();

        if (started && collide()) {
            over = true;
        }

        render();

#ifdef _WIN32
        Sleep(90);
#else
        usleep(90000);
#endif
    }

    // Restore cursor
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci; ci.dwSize = 100; ci.bVisible = TRUE;
    SetConsoleCursorInfo(h, &ci);
    system("cls");
#else
    system("clear");
#endif

    double totalScore = (double)score;

    std::cout << "\n  ================================================\n";
    std::cout << "         F L A P P Y   B I R D  -  RESULTS\n";
    std::cout << "  ================================================\n";
    std::cout << "  Pipes Cleared:  " << score / 10 << "\n";
    std::cout << "  Total Score:    " << score << "\n";
    std::cout << "  Ticks Survived: " << ticks << "\n";
    std::cout << "  ================================================\n";

    return totalScore;
}

std::string FlappyBirdGame::getName() const {
    return "Flappy Bird";
}
