#include "minecleaner.h"
#include <QDebug>
#include <random>
#include <ctime>

MineCleaner::MineCleaner() {}

void MineCleaner::randomMap(int width) {
    bombCount = 0;
    areaWidth = width;
    flagCount = 0;
    win = false;
    dead = false;

    srand(time(NULL));
    mineArea = new int*[areaWidth];
    coverArea = new int*[areaWidth];
    for(int i = 0; i < areaWidth; i ++) {
        mineArea[i] = new int[areaWidth];
        coverArea[i] = new int[areaWidth];
        // 初始化mineArea（置空：0）
        // 初始化coverArea（置未探索：-2）
        for (int j = 0; j < areaWidth; j++) {
            // 随机生成地雷
            if (rand() % 10 == 5) { // 10%
                mineArea[i][j] = -1;
            } else mineArea[i][j] = 0;
            coverArea[i][j] = -2;
        }
    }

    // 雷区地图
    for (int i = 0; i < areaWidth; i++) {
        for (int j = 0; j < areaWidth; j++) {
            if (mineArea[i][j] == -1) {
                bombCount++;
                continue;
            }
            buildBlockCount(i, j);
        }
    }
}

void MineCleaner::customMap(int **map, int width) {
    bombCount = 0;
    areaWidth = width;
    win = false;
    dead = false;
    flagCount = 0;

    mineArea = new int*[areaWidth];
    coverArea = new int*[areaWidth];
    for(int i = 0; i < areaWidth; i ++) {
        mineArea[i] = new int[areaWidth];
        coverArea[i] = new int[areaWidth];
        // 初始化mineArea（copy map）
        // 初始化coverArea（置未探索：-2）
        for (int j = 0; j < areaWidth; j++) {
            qDebug() << map[i][j];
            mineArea[i][j] = map[i][j];
            coverArea[i][j] = -2;
        }
    }

    // 雷区地图
    for (int i = 0; i < areaWidth; i++) {
        for (int j = 0; j < areaWidth; j++) {
            if (mineArea[i][j] == -1) {
                bombCount++;
                continue;
            }
            buildBlockCount(i, j);
        }
    }
}

void MineCleaner::resetMap() {
    for (int i = 0; i < areaWidth; i++) {
        for (int j = 0; j < areaWidth; j++) {
            coverArea[i][j] = -2;
        }
    }
    win = false;
    dead = false;
    flagCount = 0;
}

// 建立非雷区块数字
void MineCleaner::buildBlockCount(int row, int col)
{
    int bombNum = 0;
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1 ; j++) {
            // 越界错误检查
            if (i < 0 || i > areaWidth - 1) break;
            if (j < 0 || j > areaWidth - 1) continue;
            // 自身检查
            if (i == row && j == col) continue;
            // 邻接区域雷计数
            if (mineArea[i][j] == -1) bombNum++;
        }
    }
    mineArea[row][col] = bombNum;
}

// 点击响应 coverArea
void MineCleaner::clickCover(int row, int col, int flag)
{
    // 胜利/失败
    if (win || dead) return;

    // 放置/取消标志旗
    if (flag) {
        coverArea[row][col] = coverArea[row][col] == -2 ? -3 : -2;
        if (coverArea[row][col] == -3) flagCount++;
        else flagCount--;
        return;
    }

    // 判断失败检查：点到地雷
    if (mineArea[row][col] == -1) {
        dead = true;
        coverArea[row][col] = -1;
        return;
    }
    // 点到空白点
    else if (mineArea[row][col] == 0) {
        removeEmptyBlock(row, col);
    }
    // 点到其他点
    else {
        coverArea[row][col] = mineArea[row][col];
    }

    // 判断胜利检测
    //flagCount = 0;
    int coverCount = 0;
    for (int i = 0; i < areaWidth; i++) {
        for (int j = 0; j < areaWidth; j++) {
            //if (coverArea[i][j] == -3) flagCount++;
            if (coverArea[i][j] == -2) coverCount++;
        }
    }
    if (coverCount == 0 && flagCount == bombCount) win = true;
}

// 递归清除空白块
void MineCleaner::removeEmptyBlock(int row, int col)
{
    coverArea[row][col] = mineArea[row][col];
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++) {
            // 越界错误检查
            if (i < 0 || i > areaWidth - 1) continue;
            if (j < 0 || j > areaWidth - 1) continue;
            // 自身检查
            if (i == row && j == col) continue;
            // 已打开检查
            if (coverArea[i][j] != -2) continue;

            // 递归消除
            coverArea[i][j] = mineArea[i][j];
            if (mineArea[i][j] == 0) {
                removeEmptyBlock(i, j);
            }
        }
    }
}
