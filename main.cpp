#include "solver.h"
#include "generator.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <cstdlib>

using namespace std;

// Helper to safely clear bad cin state and flush input line
void clearCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Helper to display available maze files in maze/ directory
void listAvailableMazes() {
    cout << "\n--- Available Mazes in 'maze/' directory ---\n";
#ifdef _WIN32
    system("dir /b maze");
#else
    system("ls maze");
#endif
    cout << "--------------------------------------------\n";
}

int main() {
    Solver solver;
    bool hasMaze = false;

    // Default load maze1.txt if present for immediate convenience
    if (solver.loadMaze("maze/maze1.txt")) {
        hasMaze = true;
    }

    int choice = 0;

    while (choice != 9) {
        cout << "\n====================================================\n";
        cout << "   MAZESOLVER - PATHFINDING VISUALIZER & BENCHMARK  \n";
        cout << "====================================================\n";
        if (hasMaze) {
            int sx, sy, ex, ey;
            solver.findStartAndEnd(sx, sy, ex, ey);
            cout << " [Status: Maze loaded (" << solver.ROWS << "x" << solver.COLS 
                 << ") | Start: (" << sx << "," << sy << ") | End: (" << ex << "," << ey << ")]\n";
        } else {
            cout << " [Status: No maze loaded]\n";
        }
        cout << "----------------------------------------------------\n";
        cout << " 1. Load an existing maze from file\n";
        cout << " 2. Generate a random maze\n";
        cout << " 3. Display current maze\n";
        cout << " 4. Solve using Breadth First Search (BFS)\n";
        cout << " 5. Solve using Depth First Search (DFS)\n";
        cout << " 6. Solve using Dijkstra's Algorithm\n";
        cout << " 7. Solve using A* Algorithm\n";
        cout << " 8. Compare all four algorithms (Benchmark)\n";
        cout << " 9. Exit\n";
        cout << "----------------------------------------------------\n";
        cout << "Enter your choice (1-9): ";

        if (!(cin >> choice)) {
            clearCin();
            cout << "\n[!] Invalid input! Please enter a number from 1 to 9.\n";
            continue;
        }

        switch (choice) {
            case 1: { // Load existing maze
                listAvailableMazes();
                cout << "Enter the path of the maze file (e.g., maze/maze1.txt): ";
                string filepath;
                cin >> filepath;

                if (solver.loadMaze(filepath)) {
                    hasMaze = true;
                    cout << "\n[✓] Maze successfully loaded from " << filepath << "!\n";
                    cout << "Dimensions: " << solver.ROWS << " rows x " << solver.COLS << " columns\n";
                    solver.printMaze(solver.mazeCopy);
                } else {
                    cout << "\n[!] Failed to load maze. Check the file path and format.\n";
                }
                break;
            }

            case 2: { // Generate random maze
                int height, width;
                cout << "\nEnter maze dimensions (Height Width, minimum 5 5): ";
                if (!(cin >> height >> width)) {
                    clearCin();
                    cout << "[!] Invalid dimension input! Dimensions must be integers.\n";
                    break;
                }

                if (height < 5 || width < 5) {
                    cout << "[!] Dimensions too small. Adjusting to minimum 5x5 for wall integrity.\n";
                    if (height < 5) height = 5;
                    if (width < 5) width = 5;
                }

                Generator gen(height, width);
                gen.generateMaze(1, 1);
                
                string outPath = "maze/RecentlyGeneratedMaze.txt";
                gen.saveMazeToFile(outPath);

                if (solver.loadMaze(outPath)) {
                    hasMaze = true;
                    cout << "\n[✓] New random maze generated and loaded!\n";
                    solver.printMaze(solver.mazeCopy);
                }
                break;
            }

            case 3: { // Display maze
                if (!hasMaze) {
                    cout << "\n[!] No maze loaded yet. Please load or generate a maze first.\n";
                } else {
                    cout << "\n--- Current Original Maze ---\n";
                    solver.printMaze(solver.mazeCopy);
                }
                break;
            }

            case 4: { // BFS
                if (!hasMaze) {
                    cout << "\n[!] Please load or generate a maze first (Option 1 or 2).\n";
                } else {
                    solver.solveBFS(true);
                }
                break;
            }

            case 5: { // DFS
                if (!hasMaze) {
                    cout << "\n[!] Please load or generate a maze first (Option 1 or 2).\n";
                } else {
                    solver.solveDFS(true);
                }
                break;
            }

            case 6: { // Dijkstra
                if (!hasMaze) {
                    cout << "\n[!] Please load or generate a maze first (Option 1 or 2).\n";
                } else {
                    solver.solveDijkstra(true);
                }
                break;
            }

            case 7: { // A*
                if (!hasMaze) {
                    cout << "\n[!] Please load or generate a maze first (Option 1 or 2).\n";
                } else {
                    solver.solveAStar(true);
                }
                break;
            }

            case 8: { // Compare all 4
                if (!hasMaze) {
                    cout << "\n[!] Please load or generate a maze first (Option 1 or 2).\n";
                } else {
                    solver.compareAll();
                }
                break;
            }

            case 9: { // Exit
                cout << "\nThank you for using MazeSolver! Exiting program...\n";
                break;
            }

            default: {
                cout << "\n[!] Invalid choice: " << choice << ". Please select between 1 and 9.\n";
                break;
            }
        }
    }

    return 0;
}
