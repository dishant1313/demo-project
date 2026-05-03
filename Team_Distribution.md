# Leaderboard Project - Team Distribution Plan

This document outlines the file and responsibility distribution for the 4 team members. The workload has been perfectly balanced so that every member gets ~32KB - 38KB of code, split between core data structures and interactive game logic. Distributing the games equally prevents any single person from being burdened by frontend logic.

---

## 👤 Member 1: Data Integration & Logic Engines
**Focus:** System integration, file handling, authentication, and state-based game logic.
**Key Responsibilities:** Managing how the user interacts with the system, processing commands, storing user data securely, and implementing the Quiz Game mechanics.

*   **Files:**
    *   `src/Leaderboard.cpp` (20.0 KB) & `include/Leaderboard.h` (5.3 KB)
    *   `include/Auth.h` (1.4 KB)
    *   `games/QuizGame.cpp` (6.7 KB) & `include/QuizGame.h` (0.9 KB)
*   **Total Workload:** ~34.3 KB
*   **Technical Challenges:** Memory management, parsing complex queries, state persistence, and managing trivia logic states.

---

## 👤 Member 2: Advanced Search & Dynamic Grids
**Focus:** Self-Balancing Binary Search Trees and grid-based game movement.
**Key Responsibilities:** Implementing the strict algorithms required for AVL and Red-Black Trees. These structures are mathematically complex and require deep attention to rotational logic. Also handles the coordinate mechanics of the Snake Game.

*   **Files:**
    *   `src/RedBlackTree.cpp` (11.0 KB) & `include/RedBlackTree.h` (1.5 KB)
    *   `src/AVLTree.cpp` (10.0 KB) & `include/AVLTree.h` (1.9 KB)
    *   `games/SnakeGame.cpp` (6.7 KB) & `include/SnakeGame.h` (0.8 KB)
*   **Total Workload:** ~31.9 KB
*   **Technical Challenges:** Left/right pointer rotations, strict height/color balancing rules, and real-time grid coordinate manipulation.

---

## 👤 Member 3: Probabilistic Models & Turn-Based Logic
**Focus:** Randomized Data Structures, Priority Maintenance, and discrete game mechanics.
**Key Responsibilities:** Building data structures that rely on random probability to maintain optimal heights, alongside the primary heap structure. Also manages the game logic for the turn-based mini-games.

*   **Files:**
    *   `src/Treap.cpp` (7.8 KB) & `include/Treap.h` (1.7 KB)
    *   `src/SkipList.cpp` (7.1 KB) & `include/SkipList.h` (1.2 KB)
    *   `src/MaxHeap.cpp` (5.5 KB) & `include/MaxHeap.h` (1.1 KB)
    *   `games/TicTacToe.cpp` (7.0 KB) & `include/TicTacToe.h` (0.7 KB)
    *   `games/NumberGuess.cpp` (4.5 KB) & `include/NumberGuess.h` (0.8 KB)
*   **Total Workload:** ~37.4 KB
*   **Technical Challenges:** Implementing probabilistic balancing, optimizing randomized node promotion, and matrix validation logic (win conditions).

---

## 👤 Member 4: Interactive Applications & Range Queries
**Focus:** Specialized Utility Data Structures & Physics-Based Game Logic.
**Key Responsibilities:** Developing specialized data structures for pattern matching, group analysis, and range-based statistics, as well as maintaining the complex physics loop of Flappy Bird.

*   **Files:**
    *   `src/Trie.cpp` (5.6 KB) & `include/Trie.h` (1.2 KB)
    *   `src/SegmentTree.cpp` (5.5 KB) & `include/SegmentTree.h` (1.2 KB)
    *   `src/DisjointSet.cpp` (5.4 KB) & `include/DisjointSet.h` (1.4 KB)
    *   `games/FlappyBird.cpp` (15.2 KB) & `include/FlappyBird.h` (2.8 KB)
*   **Total Workload:** ~38.3 KB
*   **Technical Challenges:** Game loop physics optimization, substring search algorithms, and continuous range updates.
