#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <fstream>
#include <queue>
#include <stack>
#include <vector>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <climits>
#include <string>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#endif

using namespace std;

// Coordinate structure
struct Node {
    int x, y;
    Node(int _x = 0, int _y = 0) : x(_x), y(_y) {}
};

// Result structure to store metrics for pathfinding algorithms
struct AlgorithmResult {
    string algorithmName;
    bool pathFound = false;
    int pathLength = 0;
    int nodesExplored = 0;
    double executionTimeMs = 0.0;
    vector<pair<int, int>> path;
    vector<vector<char>> solvedMaze;
};

class Solver {
public:
    vector<vector<char>> maze;
    vector<vector<char>> mazeCopy;
    int ROWS, COLS;

    // 4-directional movements: Up, Down, Left, Right
    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = {0, 0, -1, 1};

    Solver();

    // Core maze utilities
    bool loadMaze(const string& filename);
    void setMaze(const vector<vector<char>>& newMaze);
    bool findStartAndEnd(int& startX, int& startY, int& endX, int& endY) const;
    bool isValid(int x, int y) const;
    void printMaze(const vector<vector<char>>& input) const;
    void clearScreen() const;

    // Pathfinding Algorithms
    AlgorithmResult solveBFS(bool display = true);
    AlgorithmResult solveDFS(bool display = true);
    AlgorithmResult solveDijkstra(bool display = true);
    AlgorithmResult solveAStar(bool display = true);

    // Benchmarking & Comparison
    void compareAll();

    // Memory estimation & backward compatibility
    size_t getCurrentMemoryUsage();
    void estimateBFSMemoryUsage(int rows, int cols, int estimatedQueueSize = 1000);
    bool bfs(int startX = -1, int startY = -1);
};

#endif // SOLVER_H
