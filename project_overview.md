# 🏆 Adaptive Multi-Game Multiplayer Leaderboard System
## Using Advanced Data Structures — Complete Project Documentation

---

## 📁 Project Structure

```
leaderboard_project/
├── main.cpp                    # Entry point, menu-driven system
├── leaderboard_system.exe      # Compiled executable
│
├── include/                    # All header files (.h)
│   ├── AVLTree.h              # AVL Tree DS
│   ├── RedBlackTree.h         # Red-Black Tree DS
│   ├── MaxHeap.h              # Max Heap DS
│   ├── Trie.h                 # Trie DS
│   ├── SegmentTree.h          # Segment Tree DS
│   ├── Treap.h                # Treap DS
│   ├── SkipList.h             # Skip List DS
│   ├── DisjointSet.h          # Union-Find DS
│   ├── Leaderboard.h          # Core leaderboard logic
│   ├── Auth.h                 # Authentication system
│   ├── NumberGuess.h          # Game 1
│   ├── TicTacToe.h            # Game 2
│   ├── SnakeGame.h            # Game 3
│   ├── QuizGame.h             # Game 4
│   └── RacingGame.h           # Game 5
│
├── src/                        # Data structure + leaderboard implementations
│   ├── AVLTree.cpp
│   ├── RedBlackTree.cpp
│   ├── MaxHeap.cpp
│   ├── Trie.cpp
│   ├── SegmentTree.cpp
│   ├── Treap.cpp
│   ├── SkipList.cpp
│   ├── DisjointSet.cpp
│   └── Leaderboard.cpp
│
├── auth/                       # Authentication module
│   └── Auth.cpp
│
├── games/                      # 5 terminal games
│   ├── NumberGuess.cpp
│   ├── TicTacToe.cpp
│   ├── SnakeGame.cpp
│   ├── QuizGame.cpp
│   └── RacingGame.cpp
│
└── data/                       # Persistent storage
    ├── users.txt               # User credentials (auto-created)
    └── leaderboard.txt         # Leaderboard data (auto-created)
```

---

## 🔧 Compile & Run

```bash
# Compile (from project root)
g++ -std=c++17 -I include \
    src/AVLTree.cpp src/RedBlackTree.cpp src/MaxHeap.cpp \
    src/Trie.cpp src/SegmentTree.cpp src/Treap.cpp \
    src/SkipList.cpp src/DisjointSet.cpp src/Leaderboard.cpp \
    auth/Auth.cpp \
    games/NumberGuess.cpp games/TicTacToe.cpp games/SnakeGame.cpp \
    games/QuizGame.cpp games/RacingGame.cpp \
    main.cpp -o leaderboard_system

# Run
./leaderboard_system        # Linux/Mac
leaderboard_system.exe      # Windows
```

---

## 🧠 Data Structures Implemented (8 + Bonus)

| # | Data Structure | File | Use Case | Time Complexity |
|---|---------------|------|----------|----------------|
| 1 | **AVL Tree** | [AVLTree.h](file:///d:/DSII/CP/leaderboard_project/include/AVLTree.h) / [AVLTree.cpp](file:///d:/DSII/CP/leaderboard_project/src/AVLTree.cpp) | Leaderboard ranking + O(log n) rank queries | Insert/Delete/Rank: O(log n) |
| 2 | **Red-Black Tree** | [RedBlackTree.h](file:///d:/DSII/CP/leaderboard_project/include/RedBlackTree.h) / [RedBlackTree.cpp](file:///d:/DSII/CP/leaderboard_project/src/RedBlackTree.cpp) | Alternative balanced ranking structure | Insert/Delete/Search: O(log n) |
| 3 | **Max Heap** | [MaxHeap.h](file:///d:/DSII/CP/leaderboard_project/include/MaxHeap.h) / [MaxHeap.cpp](file:///d:/DSII/CP/leaderboard_project/src/MaxHeap.cpp) | Top-K player extraction | Insert: O(log n), Top-K: O(k log n) |
| 4 | **Trie** | [Trie.h](file:///d:/DSII/CP/leaderboard_project/include/Trie.h) / [Trie.cpp](file:///d:/DSII/CP/leaderboard_project/src/Trie.cpp) | Username search & autocomplete | Insert/Search: O(m), Autocomplete: O(m+k) |
| 5 | **Segment Tree** | [SegmentTree.h](file:///d:/DSII/CP/leaderboard_project/include/SegmentTree.h) / [SegmentTree.cpp](file:///d:/DSII/CP/leaderboard_project/src/SegmentTree.cpp) | Range queries (sum, max, min) on scores | Build: O(n), Query/Update: O(log n) |
| 6 | **Treap** | [Treap.h](file:///d:/DSII/CP/leaderboard_project/include/Treap.h) / [Treap.cpp](file:///d:/DSII/CP/leaderboard_project/src/Treap.cpp) | Randomized BST for probabilistic ranking | Expected O(log n) for all operations |
| 7 | **Skip List** | [SkipList.h](file:///d:/DSII/CP/leaderboard_project/include/SkipList.h) / [SkipList.cpp](file:///d:/DSII/CP/leaderboard_project/src/SkipList.cpp) | Fast probabilistic sorted structure | Expected O(log n), Space: O(n log n) |
| 8 | **Disjoint Set (Union-Find)** | [DisjointSet.h](file:///d:/DSII/CP/leaderboard_project/include/DisjointSet.h) / [DisjointSet.cpp](file:///d:/DSII/CP/leaderboard_project/src/DisjointSet.cpp) | Player clustering/grouping/tiers | Find/Union: O(α(n)) ≈ O(1) amortized |

---

## 📊 Dynamic Scoring Formula (Research-Driven)

> [!IMPORTANT]
> This is **NOT** simple score sorting. The system uses a multi-factor dynamic scoring model.

### Formula

```
DynamicScore = 0.5 × RecentPerformance
             + 0.3 × Consistency
             + 0.2 × GameDifficultyWeight

FinalScore = DynamicScore × DecayFactor^(days_since_last_game)
```

### Components

| Component | Weight | Calculation |
|-----------|--------|-------------|
| **Recent Performance** | 50% | Exponentially-weighted average of last 20 scores. More recent = higher weight: `weight = 2^(i/N)` |
| **Consistency** | 30% | `100 - CoeffOfVariation`. Lower score variance = higher consistency (0-100 scale) |
| **Game Difficulty** | 20% | Weighted average of difficulty multipliers across all games played |
| **Time Decay** | Multiplier | `score × 0.95^(days_elapsed)`. Inactive players naturally decay |

### Game Difficulty Weights

| Game | Weight | Rationale |
|------|--------|-----------|
| Number Guess | 0.6 | Low complexity |
| Quiz Challenge | 0.8 | Medium knowledge-based |
| Racing Simulation | 0.85 | Strategy-heavy |
| Tic Tac Toe | 0.9 | Hard (plays against optimal Minimax AI) |
| Snake | 1.0 | Highest — real-time skill required |

---

## 🎮 Games

| # | Game | DSA Used | Key Feature |
|---|------|----------|-------------|
| 1 | **Number Guess** | Binary Search logic | Hints mirror binary search; efficiency scoring |
| 2 | **Tic Tac Toe** | **Minimax + Alpha-Beta Pruning** | Plays optimally; not random moves |
| 3 | **Snake** | **Deque + 2D Grid** | Real-time movement; platform-specific input |
| 4 | **Quiz Challenge** | **Trie-based validation** | Prefix matching for partial credit |
| 5 | **Racing Simulation** | **Priority Queue (custom heap)** | Event-driven race with AI competitors |

---

## 🔐 Authentication System

- Custom polynomial hash + XOR scramble (passwords NOT stored in plain text)
- File-based persistence (`data/users.txt`)
- Duplicate username detection
- Session management
- Interactive terminal UI with colored output

---

## ⚙️ System Features

### Menu Options (after login)
1. **Play 5 games** — each returns a score
2. **View Global Leaderboard** — AVL Tree in-order traversal
3. **View Top K Players** — Max Heap extraction
4. **Search Player** — Trie prefix autocomplete
5. **Get Player Rank** — AVL Tree order-statistics O(log n)
6. **Range Query** — Segment Tree sum/max over score range
7. **View Player Groups** — DSU cluster enumeration
8. **View Player Stats** — Complete breakdown per game
9. **Auto-Cluster by Tier** — DSU with Diamond/Gold/Silver/Bronze tiers

---

## 🎯 Research Gaps Addressed

| Problem | Solution |
|---------|----------|
| Static ranking | Dynamic scoring with multi-factor formula |
| No time relevance | Time-decay: `0.95^days` multiplier |
| No multi-game intelligence | Per-game difficulty weights |
| Limited queries | AVL rank, Segment Tree range, Trie search, Heap Top-K |
| No personalization | DSU clustering into tiers by score similarity |

---

## 💾 Data Persistence

- **users.txt**: `username:hashedPassword` format
- **leaderboard.txt**: `username|score|gamesPlayed|consistency|recentScores|timestamps|gamePlayCounts|gameTotalScores`
- Auto-loaded on startup, auto-saved on exit

---

## 🚀 Bonus Features Implemented

- ✅ Colored terminal UI (ANSI escape codes)
- ✅ Windows console ANSI support enablement
- ✅ Unicode box-drawing characters for premium look
- ✅ Password hashing (custom polynomial + XOR hash)
- ✅ Auto-clustering into named tiers
