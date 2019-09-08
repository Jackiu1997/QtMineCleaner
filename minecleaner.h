#ifndef MINECLEANER_H
#define MINECLEANER_H

#include <vector>
using std::vector;

class MineCleaner
{
public:
    int flagCount;
    int bombCount;
    int areaWidth;
    int** coverArea; // 雷区显示区域 covered: -2

    bool win = false;
    bool dead = false;

    MineCleaner();
    void randomMap(int width);
    void customMap(int **map, int width);
    void resetMap();

    void clickCover(int row, int col, int flag);

private:
    int** mineArea;  // 雷区矩阵区域 0-n: bomb number, -1: bomb

    void buildBlockCount(int i, int j);
    void removeEmptyBlock(int row, int col);
};

#endif // MINECLEANER_H
