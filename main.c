#include <windows.h> // 用于设置控制台编码
#include "maze_solver.h"
#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>  // 用于_getch()
// 附加功能配置（限时+限步数）
#define TIME_LIMIT 60    // 限时60秒
#define MAX_STEPS 50    // 最大步数限制

int main() {
    SetConsoleOutputCP(65001); 
    SetConsoleCP(65001);       

    // 初始化迷宫数组
    int maze[MAX_ROW][MAX_COL];
    int rows, cols, startRow, startCol, exitRow, exitCol;
    char filename[100];  // 存储迷宫文件名

    // 欢迎界面
    printf("———————————————————— 迷宫游戏 ——————————————————————\n");
    printf("操作说明：\n");
    printf("   WASD = 上下左右移动 | H = 下一步提示 | O = 显示最优解\n");
    printf("   Q = 退出游戏 | 限时60秒 | 最大50步\n");
    printf("————————————————————————————————————————————————————\n");

    // 输入迷宫文件名
    printf("请输入迷宫文件名（如 maze1.txt）：");
    scanf("%s", filename);

    // 加载迷宫文件
    if (!loadMazeFromFile(filename, maze, &rows, &cols, &startRow, &startCol, &exitRow, &exitCol)) {
        printf("文件加载失败！请检查文件名和路径是否正确。\n");
        system("pause"); // 暂停看错误
        return 1;
    }

    // 玩家状态初始化
    int playerRow = startRow;   // 玩家当前行
    int playerCol = startCol;   // 玩家当前列
    int steps = 0;              // 已走步数
    time_t startTime = time(NULL); // 开始计时
    int gameOver = 0;           // 游戏是否结束

    // 游戏主循环
    while (!gameOver) {
        // 1. 检查限时
        time_t currentTime = time(NULL);
        int remainingTime = TIME_LIMIT - (currentTime - startTime);
        if (remainingTime <= 0) {
            printf("时间到！游戏结束！\n");
            gameOver = 1;
            break;
        }

        // 2. 检查步数限制
        if (steps >= MAX_STEPS) {
            printf("步数超限（已走%d步）！游戏结束！\n", steps);
            gameOver = 1;
            break;
        }

        // 3. 清屏并显示游戏状态
        system("cls");
        printf("剩余时间：%d秒 | 已走步数：%d\n", remainingTime, steps);
        // 显示迷宫和玩家位置
        displayMaze(maze, rows, cols, playerRow, playerCol);
        // 4. 检查是否到达终点
        if (playerRow == exitRow && playerCol == exitCol) {
            printf("恭喜通关！总步数：%d | 剩余时间：%d秒\n", steps, remainingTime);
            gameOver = 1;
            break;
        }
        // 5. 等待用户输入
        printf("请输入操作（W/S/A/D/H/O/Q）：");
        char input = _getch(); 
        printf("%c\n", input); // 显示输入的字符

        // 初始化新位置
        int newRow = playerRow, newCol = playerCol;

        // 处理输入
        switch (input) {
            case DIR_UP: case 'W': newRow--; break;  // 上
            case DIR_DOWN: case 'S': newRow++; break;// 下
            case DIR_LEFT: case 'A': newCol--; break;// 左
            case DIR_RIGHT: case 'D': newCol++; break;// 右
            
            case 'H': case 'h': { // 提示下一步
                char hint = getHintNextStep(maze, rows, cols, playerRow, playerCol, exitRow, exitCol);
                if (hint) {
                    printf("最优下一步：%c\n", hint);
                } else {
                    printf("暂无可用提示（已到死胡同）！\n");
                }
                system("pause"); // 暂停看提示
                continue; // 不消耗步数
            }

            case 'O': case 'o': { // 显示所有解和最优解
                MazeSolution* solutions = findAllSolutions(maze, rows, cols, startRow, startCol, exitRow, exitCol);
                displayAllSolutions(solutions, maze, rows, cols);
                freeMazeSolution(solutions); // 释放内存
                system("pause"); // 暂停看解
                continue; // 不消耗步数
            }

            case 'Q': case 'q': { // 退出游戏
                printf("游戏退出！\n");
                gameOver = 1;
                continue;
            }

            default: { // 无效输入
                printf("无效输入！请按W/S/A/D/H/O/Q\n");
                system("pause");
                continue;
            }
        }

        // 6. 检查移动合法性
        // 边界检查（超出迷宫范围）
        if (newRow < 0 || newRow >= rows || newCol < 0 || newCol >= cols) {
            printf("不能走出迷宫！\n");
            system("pause");
            continue;
        }

        // 墙检查（无穿墙道具）
        if (maze[newRow][newCol] == CELL_WALL) {
            printf("撞到墙了！无法通过！\n");
            system("pause");
            continue;
        }

        // 7. 更新玩家位置和步数
        playerRow = newRow;
        playerCol = newCol;
        steps++;
    }

    // 游戏结束
    printf("\n游戏结束！按任意键退出...\n");
    system("pause");
    return 0;
}
//运行代码示例（Windows命令行）：
/*
cd /d "D:\data structure\data dazuoye\maze_game"
gcc "maze_game/main.c" "maze_game/maze_solver.c" -o "maze_game/maze_game.exe" -Wall -lm
cd maze_game
*/
/*直接输入：
maze_game.exe
*/
//删除maze—_game.exe
/*
cd /d "D:\data structure\data dazuoye\maze_game\maze_game"
taskkill /f /im maze_game.exe 2>nul
del /f maze_game.exe
dir
*/
