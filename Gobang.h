#ifndef GOBANG_H
#define GOBANG_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QPoint>
#include <QObject>
#include <stack>
#include <QPair>
#include <ui_GobangUI.h>

class NetworkManager;

enum Mode{
    Server,
    Client
};

class Gobang : public QWidget
{
    Q_OBJECT
public:
    explicit Gobang(Mode mode, QWidget *parent = nullptr);
    ~Gobang();
private slots:
    void slotClicked(int x, int y);

//UI
private:
    Ui::GobangUi *ui;

private:
    //-----五子棋部分------
    QLabel *boardLabel;
    QPushButton *chessBoard[19][19];
    int boardData[19][19];
    bool isBlackTurn;

    bool checkWin(int x, int y);
    void restartGame();
    //-悔棋-
    std::stack<QPair<int, int>> historyStack;
    void undoMove();

    //-----网络部分
private:
    NetworkManager* networkManager;

    void remotePlayerMoved(int x, int y);

    //---------测量距离-------------
    /*
    bool isStartPointSet;
    QPoint startPoint, endPoint;
    bool eventFilter(QObject *watched, QEvent *event);
    */
};

#endif // GOBANG_H
