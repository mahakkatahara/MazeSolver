#include "generator.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <fstream>

using namespace std;

Generator::Generator(int rows, int cols) {
    // Ensure minimum dimensions of at least 5x5 for outer borders and corridors
    if (rows < 5) rows = 5;
    if (cols < 5) cols = 5;

    // Ensure odd dimensions for proper maze generation with walls
    ROWS = (rows % 2 == 0) ? rows + 1 : rows;
    COLS = (cols % 2 == 0) ? cols + 1 : cols;

    maze.resize(ROWS, std::vector<char>(COLS, '#'));  // Initialize with walls
}

bool Generator::isValid(int x, int y) {
    return (x > 0 && x < ROWS - 1 && y > 0 && y < COLS - 1 && maze[x][y] == '#');
}

void Generator::generateMaze(int startX, int startY) {
    // Clamp start coordinates to valid odd interior positions
    if (startX <= 0 || startX >= ROWS - 1) startX = 1;
    if (startY <= 0 || startY >= COLS - 1) startY = 1;
    if (startX % 2 == 0) startX++;
    if (startY % 2 == 0) startY++;

    std::random_device rd;
    std::mt19937 g(rd());

    std::stack<std::pair<int, int>> stk;
    stk.push({startX, startY});
    maze[startX][startY] = 'S';  // Mark Start

    std::vector<std::pair<int, int>> directions = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};

    while (!stk.empty()) {
        auto curr = stk.top();
        int x = curr.first;
        int y = curr.second;

        std::vector<std::pair<int, int>> neighbors;

        for (auto dir : directions) {
            int nx = x + dir.first;
            int ny = y + dir.second;

            if (isValid(nx, ny))
                neighbors.push_back({nx, ny});
        }

        if (!neighbors.empty()) {
            std::shuffle(neighbors.begin(), neighbors.end(), g);
            auto next = neighbors[0];
            int nx = next.first;
            int ny = next.second;

            // Remove wall between (x, y) and (nx, ny)
            maze[(x + nx) / 2][(y + ny) / 2] = ' ';
            maze[nx][ny] = ' ';

            stk.push({nx, ny});
        } else {
            stk.pop();
        }
    }

    // Preserve 'S' at start
    maze[startX][startY] = 'S';

    // Mark End Point at bottom-right open cell
    for (int i = ROWS - 2; i >= 0; --i) {
        for (int j = COLS - 2; j >= 0; --j) {
            if (maze[i][j] == ' ' && !(i == startX && j == startY)) {
                maze[i][j] = 'E';
                return;
            }
        }
    }
}

void Generator::printMaze() {
    for (const auto& row : maze) {
        for (char cell : row) {
            if (cell == 'S')
                std::cout << "\033[1;34mS\033[0m ";
            else if (cell == 'E')
                std::cout << "\033[1;31mE\033[0m ";
            else if (cell == '#')
                std::cout << "# ";
            else
                std::cout << cell << " ";
        }
        std::cout << '\n';
    }
}

void Generator::saveMazeToFile(const std::string& filename) {
    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing.\n";
        return;
    }

    for (const auto& row : maze) {
        for (const auto& cell : row) {
            outFile << cell;
        }
        outFile << '\n';
    }

    outFile.close();
    std::cout << "Maze saved successfully to " << filename << "\n";
}
