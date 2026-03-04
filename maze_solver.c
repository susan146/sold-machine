#include "maze_solver.h"
#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// freePath函数的实现
void freePath(Path* path) {
    if (path == NULL) {
        return;
    }
    PathNode* node = path->head;
    while (node != NULL) {
        PathNode* temp = node;
        node = node->next;
        free(temp);
    }
    free(path);
}

// 方向数组（上、下、左、右）
int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
char dirChars[4] = {'W', 'S', 'A', 'D'};

// 创建新的路径节点
PathNode* createPathNode(int row, int col) {
    PathNode* node = (PathNode*)malloc(sizeof(PathNode));
    node->row = row;
    node->col = col;
    node->next = NULL;
    return node;
}

// 复制路径（用于保存解）
Path* copyPath(Path* src) {
    Path* newPath = (Path*)malloc(sizeof(Path));
    newPath->length = src->length;
    newPath->head = NULL;
    PathNode* srcNode = src->head;
    PathNode** newNode = &(newPath->head);
    while (srcNode) {
        *newNode = createPathNode(srcNode->row, srcNode->col);
        srcNode = srcNode->next;
        newNode = &((*newNode)->next);
    }
    return newPath;
}

// DFS找所有解（递归）
void dfs(int maze[MAX_ROW][MAX_COL], int rows, int cols, int row, int col,
         int exitRow, int exitCol, int visited[MAX_ROW][MAX_COL],
         Path* currentPath, MazeSolution* solutions) {
    // 边界/墙/已访问 检查
    if (row < 0 || row >= rows || col < 0 || col >= cols || 
        maze[row][col] == CELL_WALL || visited[row][col]) {
        return;
    }

    // 添加当前节点到路径
    PathNode* node = createPathNode(row, col);
    node->next = currentPath->head;
    currentPath->head = node;
    currentPath->length++;
    visited[row][col] = 1;

    // 到达终点：保存当前路径
    if (row == exitRow && col == exitCol) {
        solutions->solutions = (Path**)realloc(solutions->solutions, 
                                   (solutions->count + 1) * sizeof(Path*));
        solutions->solutions[solutions->count] = copyPath(currentPath);
        solutions->count++;
    } else {
        // 递归遍历四个方向
        for (int i = 0; i < 4; i++) {
            dfs(maze, rows, cols, row + dirs[i][0], col + dirs[i][1],
                exitRow, exitCol, visited, currentPath, solutions);
        }
    }

    // 回溯：移除当前节点，取消访问
    PathNode* temp = currentPath->head;
    currentPath->head = currentPath->head->next;
    free(temp);
    currentPath->length--;
    visited[row][col] = 0;
}

// 找所有解
MazeSolution* findAllSolutions(int maze[MAX_ROW][MAX_COL], int rows, int cols,
                              int startRow, int startCol, int exitRow, int exitCol) {
    MazeSolution* solutions = (MazeSolution*)malloc(sizeof(MazeSolution));
    solutions->solutions = NULL;
    solutions->count = 0;
    solutions->optimal = NULL;

    // 初始化访问数组
    int visited[MAX_ROW][MAX_COL] = {0};
    // 初始化当前路径
    Path* currentPath = (Path*)malloc(sizeof(Path));
    currentPath->head = NULL;
    currentPath->length = 0;

    // 执行DFS
    dfs(maze, rows, cols, startRow, startCol, exitRow, exitCol,
        visited, currentPath, solutions);

    // 找最优解（最短路径）
    if (solutions->count > 0) {
        solutions->optimal = solutions->solutions[0];
        for (int i = 1; i < solutions->count; i++) {
            if (solutions->solutions[i]->length < solutions->optimal->length) {
                solutions->optimal = solutions->solutions[i];
            }
        }
    }
    freePath(currentPath);
    return solutions;
}

// BFS找最优解（单独实现，效率更高）
Path* findOptimalSolution(int maze[MAX_ROW][MAX_COL], int rows, int cols,
                         int startRow, int startCol, int exitRow, int exitCol) {
    // 记录访问状态和父节点
    int visited[MAX_ROW][MAX_COL] = {0};
    int parentRow[MAX_ROW][MAX_COL], parentCol[MAX_ROW][MAX_COL];
    memset(parentRow, -1, sizeof(parentRow));
    memset(parentCol, -1, sizeof(parentCol));

    // 队列（BFS核心）
    int queue[MAX_ROW*MAX_COL][2];
    int front = 0, rear = 0;
    queue[rear][0] = startRow;
    queue[rear][1] = startCol;
    rear++;
    visited[startRow][startCol] = 1;

    // BFS遍历
    while (front < rear) {
        int r = queue[front][0];
        int c = queue[front][1];
        front++;

        // 到达终点
        if (r == exitRow && c == exitCol) {
            // 回溯构建路径
            Path* path = (Path*)malloc(sizeof(Path));
            path->head = NULL;
            path->length = 0;
            int curR = r, curC = c;
            while (curR != -1 && curC != -1) {
                PathNode* node = createPathNode(curR, curC);
                node->next = path->head;
                path->head = node;
                path->length++;
                int tempR = parentRow[curR][curC];
                int tempC = parentCol[curR][curC];
                curR = tempR;
                curC = tempC;
            }
            return path;
        }

        // 遍历四个方向
        for (int i = 0; i < 4; i++) {
            int nr = r + dirs[i][0];
            int nc = c + dirs[i][1];
            if (nr >= 0 && nr < rows && nc >=0 && nc < cols && 
                maze[nr][nc] != CELL_WALL && !visited[nr][nc]) {
                visited[nr][nc] = 1;
                parentRow[nr][nc] = r;
                parentCol[nr][nc] = c;
                queue[rear][0] = nr;
                queue[rear][1] = nc;
                rear++;
            }
        }
    }
    return NULL; // 无解
}

// 提示最优下一步
char getHintNextStep(int maze[MAX_ROW][MAX_COL], int rows, int cols,
                    int curRow, int curCol, int exitRow, int exitCol) {
    Path* optimal = findOptimalSolution(maze, rows, cols, curRow, curCol, exitRow, exitCol);
    if (!optimal || !optimal->head || !optimal->head->next) {
        freePath(optimal);
        return '\0'; // 无提示
    }
    // 最优路径的下一个节点
    PathNode* nextNode = optimal->head->next;
    int nr = nextNode->row, nc = nextNode->col;

    // 判断方向
    char hint = '\0';
    for (int i = 0; i < 4; i++) {
        if (curRow + dirs[i][0] == nr && curCol + dirs[i][1] == nc) {
            hint = dirChars[i];
            break;
        }
    }
    freePath(optimal);
    return hint;
}

// 从文件加载迷宫
int loadMazeFromFile(const char* filename, int maze[MAX_ROW][MAX_COL],
                    int* rows, int* cols, int* startRow, int* startCol,
                    int* exitRow, int* exitCol) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("错误：找不到文件 %s\n", filename);
        return 0;
    }
    // 读取行列数
    fscanf(file, "%d %d", rows, cols);
    // 读取起点和终点
    fscanf(file, "%d %d %d %d", startRow, startCol, exitRow, exitCol);

    // 读取迷宫数据
    for (int i = 0; i < *rows; i++) {
        for (int j = 0; j < *cols; j++) {
            fscanf(file, "%d", &maze[i][j]);
        }
    }
    fclose(file);
    // 标记起点和出口 
    maze[*startRow][*startCol] = CELL_START; // 起点标为2（S）
    maze[*exitRow][*exitCol] = CELL_EXIT;    // 出口标为3（E）
    return 1;
}

// 展示迷宫（带玩家位置）
void displayMaze(int maze[MAX_ROW][MAX_COL], int rows, int cols, int playerRow, int playerCol) {
    printf("———————————————————— 迷宫 ————————————————————\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (i == playerRow && j == playerCol) {
                printf("P ");  // 玩家
            } else if (maze[i][j] == CELL_WALL) {
                printf("■ ");  // 墙
            } else if (maze[i][j] == CELL_START) {
                printf("S ");  // 起点
            } else if (maze[i][j] == CELL_EXIT) {
                printf("E ");  // 终点
            } else {
                printf("  ");  // 通路
            }
        }
        printf("\n");
    }
    printf("————————————————————————————————————————\n");
}

// 释放迷宫解的内存
void freeMazeSolution(MazeSolution* solution) {
    if (!solution) return;
    // 释放所有解
    for (int i = 0; i < solution->count; i++) {
        freePath(solution->solutions[i]);
    }
    free(solution->solutions);
    // 释放最优解
    freePath(solution->optimal);
    // 释放解集合本身
    free(solution);
}

// 展示所有解
void displayAllSolutions(MazeSolution* solution, int maze[MAX_ROW][MAX_COL], 
                         int rows, int cols) {
    if (!solution || solution->count == 0) {
        printf("迷宫无解！\n");
        return;
    }

    printf("\n找到 %d 个解！\n", solution->count);
    printf("最优解（最短路径）长度：%d 步\n", solution->optimal->length);
    for (int i = 0; i < solution->count; i++) {
        printf("解 %d（长度 %d 步）：", i+1, solution->solutions[i]->length);
        PathNode* node = solution->solutions[i]->head;
        while (node) {
            printf("(%d,%d)", node->row, node->col);
            if (node->next) printf("->");
            node = node->next;
        }
        printf("\n");
    }
}