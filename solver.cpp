#include "solver.h"

Solver::Solver() {
    ROWS = 0;
    COLS = 0;
}

size_t Solver::getCurrentMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS memInfo;
    GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
    return memInfo.WorkingSetSize / 1024;
#else
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
#endif
}

bool Solver::loadMaze(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "\n[!] Error: Could not open file: " << filename << endl;
        return false;
    }

    vector<vector<char>> tempMaze;
    string line;
    
    while (getline(file, line)) {
        // Strip trailing \r if present (e.g., Windows CRLF line endings)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (!line.empty()) {
            tempMaze.push_back(vector<char>(line.begin(), line.end()));
        }
    }

    file.close();

    if (tempMaze.empty()) {
        cerr << "\n[!] Error: Maze file is empty!" << endl;
        return false;
    }

    ROWS = tempMaze.size();
    COLS = tempMaze[0].size();
    
    // Validate rectangular dimensions
    for (size_t r = 0; r < tempMaze.size(); ++r) {
        if (tempMaze[r].size() != static_cast<size_t>(COLS)) {
            cerr << "\n[!] Warning: Irregular row lengths detected. Normalizing grid.\n";
            tempMaze[r].resize(COLS, '#');
        }
    }

    maze = tempMaze;
    mazeCopy = tempMaze;

    int sx, sy, ex, ey;
    if (!findStartAndEnd(sx, sy, ex, ey)) {
        cerr << "\n[!] Warning: Maze loaded, but missing 'S' (Start) or 'E' (End).\n";
    }

    return true;
}

void Solver::setMaze(const vector<vector<char>>& newMaze) {
    if (newMaze.empty()) return;
    maze = newMaze;
    mazeCopy = newMaze;
    ROWS = maze.size();
    COLS = maze[0].size();
}

bool Solver::findStartAndEnd(int& startX, int& startY, int& endX, int& endY) const {
    startX = startY = endX = endY = -1;
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (mazeCopy[r][c] == 'S') {
                startX = r;
                startY = c;
            } else if (mazeCopy[r][c] == 'E') {
                endX = r;
                endY = c;
            }
        }
    }
    return (startX != -1 && startY != -1 && endX != -1 && endY != -1);
}

bool Solver::isValid(int x, int y) const {
    return (x >= 0 && x < ROWS && y >= 0 && y < COLS && mazeCopy[x][y] != '#');
}

void Solver::printMaze(const vector<vector<char>>& input) const {
    for (const auto& row : input) {
        for (char cell : row) {
            if (cell == 'S') {
                cout << "\033[1;34mS\033[0m "; // Bold Blue for Start
            } else if (cell == 'E') {
                cout << "\033[1;31mE\033[0m "; // Bold Red for End
            } else if (cell == '*') {
                cout << "\033[1;32m*\033[0m "; // Bold Green for Final Path
            } else if (cell == '.') {
                cout << "\033[1;33m.\033[0m "; // Bold Yellow for Explored Cells
            } else if (cell == '#') {
                cout << "# ";                   // Wall
            } else {
                cout << cell << " ";            // Open cell / space
            }
        }
        cout << "\n";
    }
}

void Solver::clearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// -------------------------------------------------------------
// Phase 1 – Breadth First Search (BFS)
// -------------------------------------------------------------
AlgorithmResult Solver::solveBFS(bool display) {
    AlgorithmResult result;
    result.algorithmName = "Breadth First Search (BFS)";

    if (ROWS == 0 || COLS == 0) {
        cout << "[!] No maze loaded.\n";
        return result;
    }

    int startX, startY, endX, endY;
    if (!findStartAndEnd(startX, startY, endX, endY)) {
        cout << "[!] Maze must have both 'S' (Start) and 'E' (End).\n";
        return result;
    }

    auto workingGrid = mazeCopy;
    vector<vector<bool>> visited(ROWS, vector<bool>(COLS, false));
    vector<vector<pair<int, int>>> parent(ROWS, vector<pair<int, int>>(COLS, {-1, -1}));
    queue<pair<int, int>> q;

    auto startTime = chrono::high_resolution_clock::now();

    q.push({startX, startY});
    visited[startX][startY] = true;

    bool reachedEnd = false;

    while (!q.empty()) {
        auto [cx, cy] = q.front();
        q.pop();
        result.nodesExplored++;

        if (cx == endX && cy == endY) {
            reachedEnd = true;
            break;
        }

        // Mark explored cell (avoid overwriting start or end)
        if (workingGrid[cx][cy] != 'S' && workingGrid[cx][cy] != 'E') {
            workingGrid[cx][cy] = '.';
        }

        for (int i = 0; i < 4; ++i) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (isValid(nx, ny) && !visited[nx][ny]) {
                visited[nx][ny] = true;
                parent[nx][ny] = {cx, cy};
                q.push({nx, ny});
            }
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    result.executionTimeMs = chrono::duration<double, milli>(endTime - startTime).count();

    if (reachedEnd) {
        result.pathFound = true;
        // Reconstruct path from parent pointers
        int currX = endX, currY = endY;
        while (!(currX == startX && currY == startY)) {
            result.path.push_back({currX, currY});
            auto p = parent[currX][currY];
            currX = p.first;
            currY = p.second;
        }
        result.path.push_back({startX, startY});
        reverse(result.path.begin(), result.path.end());
        result.pathLength = static_cast<int>(result.path.size());

        // Overlay path onto working grid
        for (const auto& pt : result.path) {
            if (workingGrid[pt.first][pt.second] != 'S' && workingGrid[pt.first][pt.second] != 'E') {
                workingGrid[pt.first][pt.second] = '*';
            }
        }
    } else {
        result.pathFound = false;
        result.pathLength = 0;
    }

    result.solvedMaze = workingGrid;

    if (display) {
        cout << "\n============================================\n";
        cout << "       " << result.algorithmName << "\n";
        cout << "============================================\n";
        if (result.pathFound) {
            cout << " [✓] Path found successfully!\n";
            cout << " Path Length     : " << result.pathLength << " steps\n";
            cout << " Nodes Explored  : " << result.nodesExplored << "\n";
            cout << " Execution Time  : " << fixed << setprecision(3) << result.executionTimeMs << " ms\n";
            cout << "--------------------------------------------\n";
            cout << " Solved Maze (S=Start, E=End, *=Path, .=Explored):\n\n";
            printMaze(result.solvedMaze);
            cout << "\n Path coordinates: ";
            for (size_t i = 0; i < result.path.size(); ++i) {
                cout << "(" << result.path[i].first << "," << result.path[i].second << ")";
                if (i + 1 < result.path.size()) cout << " -> ";
            }
            cout << "\n";
        } else {
            cout << " [x] No path exists to the destination!\n";
            cout << " Nodes Explored  : " << result.nodesExplored << "\n";
            cout << " Execution Time  : " << fixed << setprecision(3) << result.executionTimeMs << " ms\n";
            cout << "--------------------------------------------\n";
            printMaze(result.solvedMaze);
        }
        cout << "============================================\n";
    }

    return result;
}

// -------------------------------------------------------------
// Phase 2 – Depth First Search (DFS)
// -------------------------------------------------------------
AlgorithmResult Solver::solveDFS(bool display) {
    AlgorithmResult result;
    result.algorithmName = "Depth First Search (DFS)";

    if (ROWS == 0 || COLS == 0) {
        cout << "[!] No maze loaded.\n";
        return result;
    }

    int startX, startY, endX, endY;
    if (!findStartAndEnd(startX, startY, endX, endY)) {
        cout << "[!] Maze must have both 'S' (Start) and 'E' (End).\n";
        return result;
    }

    auto workingGrid = mazeCopy;
    vector<vector<bool>> visited(ROWS, vector<bool>(COLS, false));
    vector<vector<pair<int, int>>> parent(ROWS, vector<pair<int, int>>(COLS, {-1, -1}));
    stack<pair<int, int>> stk;

    auto startTime = chrono::high_resolution_clock::now();

    stk.push({startX, startY});
    visited[startX][startY] = true;

    bool reachedEnd = false;

    while (!stk.empty()) {
        auto [cx, cy] = stk.top();
        stk.pop();
        result.nodesExplored++;

        if (cx == endX && cy == endY) {
            reachedEnd = true;
            break;
        }

        if (workingGrid[cx][cy] != 'S' && workingGrid[cx][cy] != 'E') {
            workingGrid[cx][cy] = '.';
        }

        // Push valid neighbors onto stack
        for (int i = 0; i < 4; ++i) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (isValid(nx, ny) && !visited[nx][ny]) {
                visited[nx][ny] = true;
                parent[nx][ny] = {cx, cy};
                stk.push({nx, ny});
            }
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    result.executionTimeMs = chrono::duration<double, milli>(endTime - startTime).count();

    if (reachedEnd) {
        result.pathFound = true;
        int currX = endX, currY = endY;
        while (!(currX == startX && currY == startY)) {
            result.path.push_back({currX, currY});
            auto p = parent[currX][currY];
            currX = p.first;
            currY = p.second;
        }
        result.path.push_back({startX, startY});
        reverse(result.path.begin(), result.path.end());
        result.pathLength = static_cast<int>(result.path.size());

        for (const auto& pt : result.path) {
            if (workingGrid[pt.first][pt.second] != 'S' && workingGrid[pt.first][pt.second] != 'E') {
                workingGrid[pt.first][pt.second] = '*';
            }
        }
    } else {
        result.pathFound = false;
        result.pathLength = 0;
    }

    result.solvedMaze = workingGrid;

    if (display) {
        cout << "\n============================================\n";
        cout << "       " << result.algorithmName << "\n";
        cout << "============================================\n";
        if (result.pathFound) {
            cout << " [✓] Path found!\n";
            cout << " Path Length     : " << result.pathLength << " steps (DFS does not guarantee shortest path)\n";
            cout << " Nodes Explored  : " << result.nodesExplored << "\n";
            cout << " Execution Time  : " << fixed << setprecision(3) << result.executionTimeMs << " ms\n";
            cout << "--------------------------------------------\n";
            cout << " Solved Maze (S=Start, E=End, *=Path, .=Explored):\n\n";
            printMaze(result.solvedMaze);
            cout << "\n Path coordinates: ";
            for (size_t i = 0; i < result.path.size(); ++i) {
                cout << "(" << result.path[i].first << "," << result.path[i].second << ")";
                if (i + 1 < result.path.size()) cout << " -> ";
            }
            cout << "\n";
        } else {
            cout << " [x] No path exists to the destination!\n";
            cout << " Nodes Explored  : " << result.nodesExplored << "\n";
            cout << " Execution Time  : " << fixed << setprecision(3) << result.executionTimeMs << " ms\n";
            cout << "--------------------------------------------\n";
            printMaze(result.solvedMaze);
        }
        cout << "============================================\n";
    }

    return result;
}

// -------------------------------------------------------------
// Phase 3 – Dijkstra's Algorithm
// -------------------------------------------------------------
AlgorithmResult Solver::solveDijkstra(bool display) {
    AlgorithmResult result;
    result.algorithmName = "Dijkstra's Algorithm";

    if (ROWS == 0 || COLS == 0) {
        cout << "[!] No maze loaded.\n";
        return result;
    }

    int startX, startY, endX, endY;
    if (!findStartAndEnd(startX, startY, endX, endY)) {
        cout << "[!] Maze must have both 'S' (Start) and 'E' (End).\n";
        return result;
    }

    auto workingGrid = mazeCopy;
    const int INF = 1e9;
    vector<vector<int>> dist(ROWS, vector<int>(COLS, INF));
    vector<vector<pair<int, int>>> parent(ROWS, vector<pair<int, int>>(COLS, {-1, -1}));
    vector<vector<bool>> visited(ROWS, vector<bool>(COLS, false));

    // Min-heap priority_queue: tuple<cost, x, y>
    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<tuple<int, int, int>>> pq;

    auto startTime = chrono::high_resolution_clock::now();

    dist[startX][startY] = 0;
    pq.push({0, startX, startY});

    bool reachedEnd = false;

    while (!pq.empty()) {
        auto [d, cx, cy] = pq.top();
        pq.pop();

        if (visited[cx][cy]) continue;
        visited[cx][cy] = true;
        result.nodesExplored++;

        if (cx == endX && cy == endY) {
            reachedEnd = true;
            break;
        }

        if (workingGrid[cx][cy] != 'S' && workingGrid[cx][cy] != 'E') {
            workingGrid[cx][cy] = '.';
        }

        for (int i = 0; i < 4; ++i) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (isValid(nx, ny)) {
                int moveCost = 1; // Uniform edge weight of 1 step in maze grid
                if (dist[cx][cy] + moveCost < dist[nx][ny]) {
                    dist[nx][ny] = dist[cx][cy] + moveCost;
                    parent[nx][ny] = {cx, cy};
                    pq.push({dist[nx][ny], nx, ny});
                }
            }
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    result.executionTimeMs = chrono::duration<double, milli>(endTime - startTime).count();

    if (reachedEnd) {
        result.pathFound = true;
        int currX = endX, currY = endY;
        while (!(currX == startX && currY == startY)) {
            result.path.push_back({currX, currY});
            auto p = parent[currX][currY];
            currX = p.first;
            currY = p.second;
        }
        result.path.push_back({startX, startY});
        reverse(result.path.begin(), result.path.end());
        result.pathLength = static_cast<int>(result.path.size());

        for (const auto& pt : result.path) {
            if (workingGrid[pt.first][pt.second] != 'S' && workingGrid[pt.first][pt.second] != 'E') {
                workingGrid[pt.first][pt.second] = '*';
            }
        }
    } else {
        result.pathFound = false;
        result.pathLength = 0;
    }

    result.solvedMaze = workingGrid;

    if (display) {
        cout << "\n============================================\n";
        cout << "       " << result.algorithmName << "\n";
        cout << "============================================\n";
        if (result.pathFound) {
            cout << " [✓] Optimal shortest path found!\n";
            cout << " Path Length     : " << result.pathLength << " steps\n";
            cout << " Total Path Cost : " << dist[endX][endY] << " units\n";
            cout << " Nodes Explored  : " << result.nodesExplored << "\n";
            cout << " Execution Time  : " << fixed << setprecision(3) << result.executionTimeMs << " ms\n";
            cout << " Note: In uniform-cost grids, Dijkstra operates like weighted BFS.\n";
            cout << "--------------------------------------------\n";
            cout << " Solved Maze (S=Start, E=End, *=Path, .=Explored):\n\n";
            printMaze(result.solvedMaze);
            cout << "\n Path coordinates: ";
            for (size_t i = 0; i < result.path.size(); ++i) {
                cout << "(" << result.path[i].first << "," << result.path[i].second << ")";
                if (i + 1 < result.path.size()) cout << " -> ";
            }
            cout << "\n";
        } else {
            cout << " [x] Destination is unreachable!\n";
            cout << " Nodes Explored  : " << result.nodesExplored << "\n";
            cout << " Execution Time  : " << fixed << setprecision(3) << result.executionTimeMs << " ms\n";
            cout << "--------------------------------------------\n";
            printMaze(result.solvedMaze);
        }
        cout << "============================================\n";
    }

    return result;
}

// -------------------------------------------------------------
// Phase 4 – A* Algorithm
// -------------------------------------------------------------
AlgorithmResult Solver::solveAStar(bool display) {
    AlgorithmResult result;
    result.algorithmName = "A* Search Algorithm";

    if (ROWS == 0 || COLS == 0) {
        cout << "[!] No maze loaded.\n";
        return result;
    }

    int startX, startY, endX, endY;
    if (!findStartAndEnd(startX, startY, endX, endY)) {
        cout << "[!] Maze must have both 'S' (Start) and 'E' (End).\n";
        return result;
    }

    auto workingGrid = mazeCopy;
    const int INF = 1e9;
    vector<vector<int>> gScore(ROWS, vector<int>(COLS, INF));
    vector<vector<int>> fScore(ROWS, vector<int>(COLS, INF));
    vector<vector<pair<int, int>>> parent(ROWS, vector<pair<int, int>>(COLS, {-1, -1}));
    vector<vector<bool>> visited(ROWS, vector<bool>(COLS, false));

    // Manhattan distance heuristic for 4-directional grid
    auto heuristic = [](int x1, int y1, int x2, int y2) -> int {
        return abs(x1 - x2) + abs(y1 - y2);
    };

    // Priority queue tuple: <f_score, g_score, x, y>
    priority_queue<tuple<int, int, int, int>, 
                   vector<tuple<int, int, int, int>>, 
                   greater<tuple<int, int, int, int>>> pq;

    auto startTime = chrono::high_resolution_clock::now();

    gScore[startX][startY] = 0;
    fScore[startX][startY] = heuristic(startX, startY, endX, endY);
    pq.push({fScore[startX][startY], 0, startX, startY});

    bool reachedEnd = false;

    while (!pq.empty()) {
        auto [currF, currG, cx, cy] = pq.top();
        pq.pop();

        if (visited[cx][cy]) continue;
        visited[cx][cy] = true;
        result.nodesExplored++;

        if (cx == endX && cy == endY) {
            reachedEnd = true;
            break;
        }

        if (workingGrid[cx][cy] != 'S' && workingGrid[cx][cy] != 'E') {
            workingGrid[cx][cy] = '.';
        }

        for (int i = 0; i < 4; ++i) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (isValid(nx, ny) && !visited[nx][ny]) {
                int tentativeG = currG + 1;
                if (tentativeG < gScore[nx][ny]) {
                    parent[nx][ny] = {cx, cy};
                    gScore[nx][ny] = tentativeG;
                    int h = heuristic(nx, ny, endX, endY);
                    fScore[nx][ny] = tentativeG + h;
                    pq.push({fScore[nx][ny], tentativeG, nx, ny});
                }
            }
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    result.executionTimeMs = chrono::duration<double, milli>(endTime - startTime).count();

    if (reachedEnd) {
        result.pathFound = true;
        int currX = endX, currY = endY;
        while (!(currX == startX && currY == startY)) {
            result.path.push_back({currX, currY});
            auto p = parent[currX][currY];
            currX = p.first;
            currY = p.second;
        }
        result.path.push_back({startX, startY});
        reverse(result.path.begin(), result.path.end());
        result.pathLength = static_cast<int>(result.path.size());

        for (const auto& pt : result.path) {
            if (workingGrid[pt.first][pt.second] != 'S' && workingGrid[pt.first][pt.second] != 'E') {
                workingGrid[pt.first][pt.second] = '*';
            }
        }
    } else {
        result.pathFound = false;
        result.pathLength = 0;
    }

    result.solvedMaze = workingGrid;

    if (display) {
        cout << "\n============================================\n";
        cout << "         " << result.algorithmName << "\n";
        cout << "============================================\n";
        if (result.pathFound) {
            cout << " [✓] Optimal shortest path found using Manhattan Heuristic!\n";
            cout << " Path Length     : " << result.pathLength << " steps\n";
            cout << " Nodes Explored  : " << result.nodesExplored << "\n";
            cout << " Execution Time  : " << fixed << setprecision(3) << result.executionTimeMs << " ms\n";
            cout << " Heuristic Used  : h(n) = |x - targetX| + |y - targetY|\n";
            cout << "--------------------------------------------\n";
            cout << " Solved Maze (S=Start, E=End, *=Path, .=Explored):\n\n";
            printMaze(result.solvedMaze);
            cout << "\n Path coordinates: ";
            for (size_t i = 0; i < result.path.size(); ++i) {
                cout << "(" << result.path[i].first << "," << result.path[i].second << ")";
                if (i + 1 < result.path.size()) cout << " -> ";
            }
            cout << "\n";
        } else {
            cout << " [x] Destination is unreachable!\n";
            cout << " Nodes Explored  : " << result.nodesExplored << "\n";
            cout << " Execution Time  : " << fixed << setprecision(3) << result.executionTimeMs << " ms\n";
            cout << "--------------------------------------------\n";
            printMaze(result.solvedMaze);
        }
        cout << "============================================\n";
    }

    return result;
}

// -------------------------------------------------------------
// Algorithm Comparison and Benchmarking Tool
// -------------------------------------------------------------
void Solver::compareAll() {
    if (ROWS == 0 || COLS == 0) {
        cout << "\n[!] Please load or generate a maze first before comparing algorithms.\n";
        return;
    }

    int sx, sy, ex, ey;
    if (!findStartAndEnd(sx, sy, ex, ey)) {
        cout << "\n[!] Cannot compare: Maze must contain valid Start ('S') and End ('E') positions.\n";
        return;
    }

    cout << "\n========================================================================================\n";
    cout << "                     ALGORITHM BENCHMARK & COMPARISON REPORT\n";
    cout << "========================================================================================\n";
    cout << " Maze Dimensions: " << ROWS << " rows x " << COLS << " columns\n";
    cout << " Start Point: (" << sx << ", " << sy << ") | End Point: (" << ex << ", " << ey << ")\n";
    cout << "----------------------------------------------------------------------------------------\n";

    // Run all 4 algorithms on identical original maze copy without visual spam
    AlgorithmResult rBFS = solveBFS(false);
    AlgorithmResult rDFS = solveDFS(false);
    AlgorithmResult rDijkstra = solveDijkstra(false);
    AlgorithmResult rAStar = solveAStar(false);

    vector<AlgorithmResult> results = {rBFS, rDFS, rDijkstra, rAStar};

    // Print benchmark table
    cout << left 
         << setw(28) << "Algorithm"
         << setw(14) << "Path Found?"
         << setw(16) << "Path Length"
         << setw(18) << "Nodes Explored"
         << setw(16) << "Time (ms)"
         << "\n";
    cout << string(92, '-') << "\n";

    for (const auto& r : results) {
        cout << left 
             << setw(28) << r.algorithmName
             << setw(14) << (r.pathFound ? "Yes" : "No")
             << setw(16) << (r.pathFound ? to_string(r.pathLength) : "N/A")
             << setw(18) << r.nodesExplored
             << fixed << setprecision(4) << setw(16) << r.executionTimeMs
             << "\n";
    }
    cout << string(92, '=') << "\n";

    // Academic analysis
    cout << "\nKey Observations & DSA Insights:\n";
    cout << " 1. BFS vs DFS:\n";
    cout << "    - BFS guarantees the shortest path (" << (rBFS.pathFound ? to_string(rBFS.pathLength) : "0") << " steps) because it explores level-by-level.\n";
    cout << "    - DFS explores greedily down one branch and backtracks. It took " << (rDFS.pathFound ? to_string(rDFS.pathLength) : "0") << " steps, which is typically longer than optimal.\n";
    cout << " 2. Dijkstra vs BFS:\n";
    cout << "    - With uniform edge weights (each move = 1), Dijkstra's shortest path length matches BFS (" << (rDijkstra.pathFound ? to_string(rDijkstra.pathLength) : "0") << " steps).\n";
    cout << "    - Dijkstra maintains a priority queue, making it versatile for weighted graphs.\n";
    cout << " 3. A* Search:\n";
    cout << "    - A* uses the Manhattan distance heuristic h(n) = |dx| + |dy| to guide its search directly toward the target.\n";
    cout << "    - Because Manhattan distance never overestimates true grid cost (it is admissible), A* guarantees the optimal shortest path (" << (rAStar.pathFound ? to_string(rAStar.pathLength) : "0") << " steps).\n";
    cout << "    - Notice how A* explores fewer or more focused nodes (" << rAStar.nodesExplored << ") compared to blind uniform search.\n";
    cout << "========================================================================================\n";
}

// -------------------------------------------------------------
// Backward Compatibility Methods
// -------------------------------------------------------------
void Solver::estimateBFSMemoryUsage(int rows, int cols, int estimatedQueueSize) {
    size_t visitedMem = rows * cols * sizeof(bool);
    size_t parentMem = rows * cols * sizeof(pair<int, int>);
    size_t queueMem = estimatedQueueSize * sizeof(Node);
    size_t total = visitedMem + parentMem + queueMem;

    cout << "\n--- Estimated BFS Memory Usage ---\n";
    cout << "Visited[][]     : " << visitedMem / 1024.0 << " KB\n";
    cout << "Parent[][]      : " << parentMem / 1024.0 << " KB\n";
    cout << "Queue           : " << queueMem / 1024.0 << " KB\n";
    cout << "Total Estimated : " << total / 1024.0 << " KB\n";
}

bool Solver::bfs(int startX, int startY) {
    // If coordinates are provided, place 'S' at that position if not already set
    if (startX >= 0 && startX < ROWS && startY >= 0 && startY < COLS) {
        mazeCopy[startX][startY] = 'S';
    }
    auto res = solveBFS(true);
    return res.pathFound;
}
