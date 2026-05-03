/*
 * FlappyBird.h - Flappy Bird Game
 *
 * A real-time terminal Flappy Bird clone where the player taps
 * SPACE or UP arrow to flap, dodging pipes that scroll left.
 *
 * Data Structures:
 * - Custom Min-Heap Priority Queue for scheduling pipe spawns
 *   and timed difficulty events
 * - 2D char buffer for flicker-free frame rendering
 * - Vector for active pipe tracking
 * - Physics simulation (gravity + velocity)
 */

#ifndef FLAPPY_BIRD_H
#define FLAPPY_BIRD_H

#include <string>
#include <vector>

// ──────────────────────────────────────────────
// Game Event (PQ-scheduled game events)
// ──────────────────────────────────────────────
struct GameEvent {
    int time;
    std::string type;
    int severity;
    std::string description;

    bool operator<(const GameEvent& other) const {
        return time > other.time;
    }
};

// ──────────────────────────────────────────────
// Pipe obstacle
// ──────────────────────────────────────────────
struct Pipe {
    int x;
    int gapTop;
    int gapSize;
    bool scored;
    bool active;
};

// ──────────────────────────────────────────────
// Flappy Bird Game
// ──────────────────────────────────────────────
class FlappyBirdGame {
private:
    static const int W = 40;
    static const int H = 18;

    // Frame buffer
    char buf[18][41];

    // Bird
    double birdY;
    double vel;
    int birdX;

    // Physics
    double gravity;
    double flapPow;

    // State
    int score;
    int ticks;
    bool over;
    bool started;
    double speed;
    int gapSize;
    int nextSpawn;
    std::string eventMsg;
    int eventTimer;

    // Pipes
    std::vector<Pipe> pipes;

    // Priority Queue (min-heap)
    std::vector<GameEvent> eventQueue;
    void heapUp(int i);
    void heapDown(int i);
    void pushEv(const GameEvent& e);
    GameEvent popEv();
    bool hasEv() const;

    // Methods
    char getKey();
    void init();
    void scheduleEvents();
    void spawn();
    void update();
    bool collide();
    void processEv();
    void render();
    void bufClear();
    void bufSet(int r, int c, char ch);
    void bufStr(int r, int c, const std::string& s);

public:
    FlappyBirdGame();
    double play();
    std::string getName() const;
};

#endif // FLAPPY_BIRD_H
