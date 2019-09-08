#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <windows.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMaximumSize(805, 860);
    this->setMinimumSize(805, 860);
    this->setWindowTitle("Mine Cleaner");
    this->setWindowIcon(QIcon("://img/bomb.png"));

    gameStart = false;
    editMode = false;

    origin = QPoint(5, 55);
    blackImg = QImage("://img/grey.png");
    whiteImg = QImage("://img/white.png");
    bombImg = QImage("://img/bomb.png");
    flagImg = QImage("://img/alert.png");

    ui->btn_randomMap->setIcon(QIcon("://icon/random.png"));
    ui->btn_customMap->setIcon(QIcon("://icon/custom.png"));
    ui->btn_aiMode->setIcon(QIcon("://icon/ai.png"));
    ui->btn_reset->setIcon(QIcon("://icon/reset.png"));
    ui->btn_exit->setIcon(QIcon("://icon/exit.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


// 界面绘制函数
void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    if (gameStart) {
        // 绘制雷区元素（空，1-n，雷）
        for (int row = 0; row < areaWidth; row++) {
            for (int col = 0; col < areaWidth; col++) {
                int type = mineCleaner.coverArea[row][col];
                drawBlock(painter, row, col, type);
            }
        }

        ui->lcd_bombCount->display(mineCleaner.bombCount);
        ui->lcd_flagNumber->display(mineCleaner.flagCount);
    }

    if (editMode) {
        // 绘制雷区元素（空，1-n，雷）
        for (int row = 0; row < areaWidth; row++) {
            for (int col = 0; col < areaWidth; col++) {
                int type = cusMap[row][col];
                drawBlock(painter, row, col, type);
            }
        }

        ui->lcd_bombCount->display(bombCount);
        ui->lcd_flagNumber->display(0);
    }
}

void MainWindow::drawBlock(QPainter &painter, int row, int col, int type) {
    QRect rect(origin.rx() + col*blockWidth, origin.ry() + row*blockWidth, blockWidth, blockWidth);
    switch (type) {
    // 旗子
    case -3:
        painter.drawImage(rect, blackImg);
        painter.drawImage(rect, flagImg);
        break;
    // 未探索
    case -2:
        //painter.setBrush(QBrush(QColor("#9C9C9C")));
        //painter.drawRect(rect);
        painter.drawImage(rect, blackImg);
        break;
    // 地雷
    case -1:
        painter.drawImage(rect, bombImg);
        break;
    // 空点
    case 0:
        //painter.setBrush(QBrush(QColor("#ffffff")));
        //painter.drawRect(rect);
        painter.drawImage(rect, whiteImg);
        break;
    // 周围有雷点
    default:
        painter.drawImage(rect, whiteImg);

        QFont ft("Arial", blockWidth / 2, 3);
        painter.setFont(ft);
        painter.drawText(rect, Qt::AlignCenter, QString::number(type));
        break;
    }
}


// 手动扫雷模式 或 地雷编辑模式
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint clickPos = event->pos();
    // 左键点击
    int row = -1, col = -1;
    bool clickArea = getClickRowCol(clickPos, row, col);

    if (gameStart && clickArea) {
        if(event->button() == Qt::LeftButton) {
            mineCleaner.clickCover(row, col, 0);
        } else if(event->button() == Qt::RightButton) {
            mineCleaner.clickCover(row, col, 1);
        }
        judgeWinDead();
    }
    else if (editMode && clickArea) {
        cusMap[row][col] = -1;
        bombCount++;
    }

    repaint();
}

bool MainWindow::getClickRowCol(QPoint clickPos, int &row, int &col) {
    col = (clickPos.rx() - origin.rx()) / blockWidth;
    row = (clickPos.ry() - origin.ry()) / blockWidth;

    if (row >=0 && row < areaWidth && col >= 0 && col < areaWidth) {
        return true;
    } else return false;
}


// AI扫雷模式
void MainWindow::on_btn_aiMode_clicked()
{
    while (true) {
        aiClick();
        if (judgeWinDead()) break;
    }
}

// 获取区域八邻域中所有旗子数和未探索区域数
void MainWindow::getRoundCount(int row, int col, int &coverNum, int &flagNum) {
    coverNum = 0, flagNum = 0;
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1 ; j++) {
            // 越界错误检查
            if (i < 0 || i > areaWidth - 1) break;
            if (j < 0 || j > areaWidth - 1) continue;
            // 自身检查
            if (i == row && j == col) continue;
            // 空格和旗子计数
            int block = mineCleaner.coverArea[i][j];
            if (block == -2) coverNum++;
            if (block == -3) flagNum++;
        }
    }
}

void MainWindow::aiClick()
{
    if (mineCleaner.win || mineCleaner.dead) return;

    bool getClick = false;
    for (int row = 0; row < areaWidth; row++) {
        for (int col = 0; col < areaWidth; col++) {
            int blockNum = mineCleaner.coverArea[row][col];
            if (blockNum < 0) continue;

            int coverNum, flagNum;
            getRoundCount(row, col, coverNum, flagNum);
            // 点数 == 未探索数 + 旗子数
            if (blockNum == coverNum + flagNum) {
                // 在未探索处插旗，地雷
                for (int i = row - 1; i <= row + 1; i++) {
                    for (int j = col - 1; j <= col + 1 ; j++) {
                        if (i < 0 || i > areaWidth - 1) break;
                        if (j < 0 || j > areaWidth - 1) continue;
                        if (i == row && j == col) continue;
                        if (mineCleaner.coverArea[i][j] == -2) {
                            mineCleaner.clickCover(i, j, 1);
                            repaint();
                            Sleep(clickDelay);
                            getClick = true;
                        }
                    }
                }

            }
            // 点数 == 旗子数
            else if (blockNum == flagNum) {
                // 点开未探索处，安全
                for (int i = row - 1; i <= row + 1; i++) {
                    for (int j = col - 1; j <= col + 1 ; j++) {
                        if (i < 0 || i > areaWidth - 1) break;
                        if (j < 0 || j > areaWidth - 1) continue;
                        if (i == row && j == col) continue;
                        if (mineCleaner.coverArea[i][j] == -2) {
                            mineCleaner.clickCover(i, j, 0);
                            repaint();
                            Sleep(clickDelay);
                            getClick = true;
                        }
                    }
                }
            }
        }
    }

    if (!getClick) {
        // 随机点击未探索区
        vector<std::pair<int, int> > coverVec;
        for (int i = 0; i < areaWidth; i++) {
            for (int j = 0; j < areaWidth; j++) {
                if (mineCleaner.coverArea[i][j] == -2) {
                    coverVec.push_back(std::make_pair(i, j));
                }
            }
        }
        srand(time(NULL));
        int click = rand() % coverVec.size();
        int row = coverVec[click].first, col = coverVec[click].second;
        mineCleaner.clickCover(row, col, 0);
        repaint();
        Sleep(clickDelay);

        return;
    }
}


// 胜利失败游戏
bool MainWindow::judgeWinDead() {
    if (mineCleaner.dead) {
        QMessageBox::information(this, "", "You lose, press Reset to continue!");
        return true;
    } else if (mineCleaner.win) {
        QMessageBox::information(this, "", "You win, Press Reset to continue!");
        return true;
    }
    return false;
}


// 随机地图游戏
void MainWindow::on_btn_randomMap_clicked()
{
    areaWidth = ui->lineEdit->text().toInt();
    mineCleaner.randomMap(areaWidth);
    blockWidth = 800/areaWidth;
    gameStart = true;
    editMode = false;
    repaint();
}

// 自定义地图游戏
void MainWindow::on_btn_customMap_clicked()
{
    areaWidth = ui->lineEdit->text().toInt();
    blockWidth = 800/areaWidth;

    if (editMode) {
        mineCleaner.customMap(cusMap, areaWidth);
        gameStart = true;
        editMode = false;
    } else {
        cusMap = new int*[areaWidth];
        for(int i = 0; i < areaWidth; i ++) {
            cusMap[i] = new int[areaWidth];
            for (int j = 0; j < areaWidth; j++) {
                cusMap[i][j] = 0;
            }
        }
        gameStart = false;
        editMode = true;
    }
    repaint();
}

// 重置扫雷游戏
void MainWindow::on_btn_reset_clicked()
{
    mineCleaner.resetMap();
    repaint();
}

// 推出扫雷游戏
void MainWindow::on_btn_exit_clicked()
{
    this->close();
}
