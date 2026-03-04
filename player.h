#ifndef PLAYER_H
#define PLAYER_H
// 方向宏定义（对应WASD）
#define DIR_UP 'w'
#define DIR_DOWN 's'
#define DIR_LEFT 'a'
#define DIR_RIGHT 'd'
// 只保留freePath的声明（告诉编译器有这个函数，实现放在maze_solver.c里）
void freePath(Path* path);
#endif