#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QPainter>
#include <QMouseEvent>
#include "minecleaner.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btn_aiMode_clicked();

    void on_btn_reset_clicked();

    void on_btn_exit_clicked();

    void on_btn_randomMap_clicked();

    void on_btn_customMap_clicked();

private:
    int areaWidth;

    bool editMode;
    int bombCount;
    int **cusMap;

    bool gameStart;
    MineCleaner mineCleaner;

    QImage blackImg;
    QImage whiteImg;
    QImage bombImg;
    QImage flagImg;
    QPoint origin;
    int blockWidth;

    Ui::MainWindow *ui;
    void paintEvent(QPaintEvent *event);
    void drawBlock(QPainter &painter, int row, int col, int type);

    void mousePressEvent(QMouseEvent *event);
    bool getClickRowCol(QPoint clickPos, int &row, int &col);

    int clickDelay = 100;
    void aiClick();
    void getRoundCount(int row, int col, int &coverNum, int &flagNum);

    bool judgeWinDead();
};

#endif // MAINWINDOW_H
