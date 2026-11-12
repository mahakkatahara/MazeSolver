# MazeSolver – Pathfinding Algorithm Visualizer and Benchmark Tool

**Academic Project:** B.Tech Semester 3 – Data Structures and Algorithms (DSA)  
**Language:** C++ (C++17 Standard)  
**Repository:** [https://github.com/mahakkatahara/MazeSolver](https://github.com/mahakkatahara/MazeSolver)

---

## 📌 Project Overview

**MazeSolver** is an interactive, terminal-based C++ application designed to solve 2D grid mazes using classical graph search and pathfinding algorithms. The tool provides visual step-by-step resolution, terminal map rendering, and a side-by-side benchmarking engine that evaluates and compares algorithm performance across time and exploration metrics.

---

## ✨ Features

- **Interactive Terminal Menu:** Clean 9-option CLI menu with input validation to prevent crashes.
- **Multiple Mazes Supported:** Load pre-saved mazes (`maze/maze1.txt` to `maze6.txt`, `unreachable.txt`) or generate custom randomized mazes.
- **Procedural Maze Generator:** Generates perfect mazes using randomized Depth-First Search with backtracking.
- **Visual Terminal Grid Display:**
  - `S` : Start Position (Bold Blue)
  - `E` : Destination / End Position (Bold Red)
  - `#` : Obstacle / Wall
  - `.` : Explored Node / Cell (Bold Yellow)
  - `*` : Optimal Reconstructed Path (Bold Green)
  - ` ` : Unexplored Open Path
- **Four Distinct Pathfinding Solvers:**
  1. Breadth First Search (BFS)
  2. Depth First Search (DFS)
  3. Dijkstra's Algorithm
  4. A* Search Algorithm
- **Comprehensive Benchmarking Tool:** Compares all 4 algorithms on the identical maze state, measuring:
  - Path existence (`Yes` / `No`)
  - Path length (number of steps)
  - Number of explored nodes
  - Execution time (in milliseconds using `std::chrono`)
- **Robust Error Handling:** Handles missing files, unreachable targets, non-numeric input, and irregular maze dimensions.

---

## 🧠 Algorithms & DSA Concepts Used

| Algorithm | Primary Data Structure | Strategy | Shortest Path Guarantee? | Time Complexity | Space Complexity |
|---|---|---|---|---|---|
| **BFS** | `std::queue<pair<int, int>>` | Level-by-level exploration | **Yes** (unweighted) | $O(V + E)$ | $O(V)$ |
| **DFS** | `std::stack<pair<int, int>>` | Deep greedy exploration & backtracking | **No** (often wandering) | $O(V + E)$ | $O(V)$ |
| **Dijkstra** | `std::priority_queue` (min-heap) | Greedy lowest cumulative cost | **Yes** (non-negative weights) | $O((V + E) \log V)$ | $O(V)$ |
| **A\*** | `std::priority_queue` (min-heap) | Heuristic-guided best-first search ($f = g + h$) | **Yes** (with admissible heuristic) | $O((V + E) \log V)$ | $O(V)$ |

### DSA Details:
1. **Breadth First Search (BFS):**
   - Implements a FIFO queue and a 2D boolean array `visited[][]`.
   - Explores equidistant neighbors uniformly.
   - Guaranteed to find the minimal number of steps in unweighted grids.
2. **Depth First Search (DFS):**
   - Implements a LIFO stack and tracks parent coordinates.
   - Dives deep into corridors until hitting dead ends, then backtracks.
   - Demonstrates why greedy unguided exploration does not yield shortest paths.
3. **Dijkstra's Algorithm:**
   - Implements a min-heap priority queue storing tuples `(distance, x, y)`.
   - Maintains a 2D array `dist[][]` initialized to infinity.
   - Under uniform grid movement cost ($cost = 1$), Dijkstra guarantees the optimal shortest path.
4. **A\* Search Algorithm:**
   - Combines path cost $g(n)$ (distance from start) and heuristic cost $h(n)$ (estimated distance to goal).
   - Uses the **Manhattan Distance heuristic** for 4-directional movement:
     $$h(n) = |x - \text{targetX}| + |y - \text{targetY}|$$
   - Since Manhattan distance never overestimates the actual grid distance (admissible and consistent), A* finds the optimal path while exploring significantly fewer nodes than uninformed search algorithms.

---

## 📁 Project Structure

```
MazeSolver/
├── main.cpp          # CLI interface, interactive menu, user interaction
├── solver.h          # Solver class header, Node and AlgorithmResult structures
├── solver.cpp        # Implementations of BFS, DFS, Dijkstra, A*, and benchmarking
├── generator.h       # Maze Generator class header
├── generator.cpp     # Randomized DFS maze generation logic
├── maze/             # Directory containing pre-saved and generated maze files
│   ├── maze1.txt
│   ├── maze2.txt
│   ├── maze3.txt
│   ├── maze4.txt
│   ├── maze5.txt
│   ├── maze6.txt
│   ├── unreachable.txt
│   └── RecentlyGeneratedMaze.txt
├── README.md         # Project documentation and user guide
└── .gitignore        # Build artifacts exclusion
```

---

## ⚙️ Compilation & Execution

### Prerequisites
- A modern C++ compiler supporting **C++17** (`g++`, `clang++`, or MSVC).

### 1. Compile
```bash
g++ -std=c++17 main.cpp generator.cpp solver.cpp -o main
```

### 2. Run
On Linux / macOS:
```bash
./main
```
On Windows:
```cmd
main.exe
```

---

## 🖥️ Menu Options

```text
====================================================
   MAZESOLVER - PATHFINDING VISUALIZER & BENCHMARK  
====================================================
 1. Load an existing maze from file
 2. Generate a random maze
 3. Display current maze
 4. Solve using Breadth First Search (BFS)
 5. Solve using Depth First Search (DFS)
 6. Solve using Dijkstra's Algorithm
 7. Solve using A* Algorithm
 8. Compare all four algorithms (Benchmark)
 9. Exit
====================================================
```

---

## 📊 Sample Benchmark Output

Running Option 8 (Compare All) on a $23 \times 110$ maze (`maze/maze2.txt`):

```text
========================================================================================
                     ALGORITHM BENCHMARK & COMPARISON REPORT
========================================================================================
 Maze Dimensions: 23 rows x 110 columns
 Start Point: (2, 5) | End Point: (20, 105)
----------------------------------------------------------------------------------------
Algorithm                   Path Found?   Path Length     Nodes Explored    Time (ms)       
--------------------------------------------------------------------------------------------
Breadth First Search (BFS)  Yes           139             1706              0.2851          
Depth First Search (DFS)    Yes           309             317               0.0726          
Dijkstra's Algorithm        Yes           139             1709              0.9688          
A* Search Algorithm         Yes           139             1590              1.2415          
============================================================================================

Key Observations & DSA Insights:
 1. BFS vs DFS:
    - BFS guarantees the shortest path (139 steps) because it explores level-by-level.
    - DFS explores greedily down one branch and backtracks. It took 309 steps, which is typically longer than optimal.
 2. Dijkstra vs BFS:
    - With uniform edge weights (each move = 1), Dijkstra's shortest path length matches BFS (139 steps).
    - Dijkstra maintains a priority queue, making it versatile for weighted graphs.
 3. A* Search:
    - A* uses the Manhattan distance heuristic h(n) = |dx| + |dy| to guide its search directly toward the target.
    - Because Manhattan distance never overestimates true grid cost (it is admissible), A* guarantees the optimal shortest path (139 steps).
    - Notice how A* explores fewer or more focused nodes (1590) compared to blind uniform search.
========================================================================================
```

---

## 🧪 Testing Scenarios Verified

1. **Standard Mazes:** Tested across small (`maze1.txt`), medium (`maze5.txt`), and large (`maze2.txt`) grids.
2. **Path Exists:** Correct reconstruction and coordinate reporting for all 4 solvers.
3. **Unreachable Destination (`maze/unreachable.txt`):** Correctly identifies blocked targets and reports `Path Found: No` without hanging or crashing.
4. **Input Validation:** Tested invalid character inputs, out-of-range choices, invalid file paths, and sub-5x5 dimensions.
5. **Memory Consistency:** Ensures grid states are reset between consecutive algorithm runs during benchmarking.

---

## ⚠️ Limitations & Future Work

- Movement is currently constrained to 4 cardinal directions (North, South, East, West). 8-directional diagonal movement with Euclidean heuristic could be added in the future.
- Visual animation speed can be made configurable for classroom demonstrations.

---

## 👤 Author
Developed for B.Tech Semester 3 Data Structures and Algorithms coursework.
GitHub: [mahakkatahara](https://github.com/mahakkatahara)
