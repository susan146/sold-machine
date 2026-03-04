#ifndef MAZE_SOLVER_H
#define MAZE_SOLVER_H
#define MAX_ROW 100
#define MAX_COL 100
#define CELL_EMPTY 0
#define CELL_WALL 1
#define CELL_START 2
#define CELL_EXIT 3

// 路径节点（存坐标）
typedef struct PathNode {
    int row;
    int col;
    struct PathNode* next;
} PathNode;

// 路径结构（存整条路径+长度）
typedef struct {
    PathNode* head;
    int length;
} Path;

// 迷宫解集合（所有解+最优解）
typedef struct {
    Path** solutions;  // 所有解的数组
    int count;         // 解的数量
    Path* optimal;     // 最优解（最短路径）
} MazeSolution;

// 函数声明
MazeSolution* findAllSolutions(int maze[MAX_ROW][MAX_COL], int rows, int cols,
                              int startRow, int startCol, int exitRow, int exitCol);
Path* findOptimalSolution(int maze[MAX_ROW][MAX_COL], int rows, int cols,
                         int startRow, int startCol, int exitRow, int exitCol);
char getHintNextStep(int maze[MAX_ROW][MAX_COL], int rows, int cols,
                    int curRow, int curCol, int exitRow, int exitCol);
void displayAllSolutions(MazeSolution* solution, int maze[MAX_ROW][MAX_COL],
                        int rows, int cols);
void freeMazeSolution(MazeSolution* solution);
void displayMaze(int maze[MAX_ROW][MAX_COL], int rows, int cols, int playerRow, int playerCol);
int loadMazeFromFile(const char* filename, int maze[MAX_ROW][MAX_COL],
                    int* rows, int* cols, int* startRow, int* startCol,
                    int* exitRow, int* exitCol);
// 补充freePath的声明
void freePath(Path* path);
#endif