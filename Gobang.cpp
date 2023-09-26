#include "Gobang.h"
#include "networkmanager.h"
#include <QDebug>
#include <QGridLayout>
#include <QPixmap>
#include <QIcon>
#include <QEvent>
#include <QMouseEvent>
#include <QMessageBox>



Gobang::Gobang(Mode mode, QWidget *parent) : QWidget(parent), ui(new Ui::GobangUi){
    // 初始化逻辑
    memset(boardData, 0, sizeof(boardData));
    isBlackTurn = true;
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

   // 初始化并设置棋盘背景图像
    boardLabel = new QLabel(this);
    boardLabel->setAlignment(Qt::AlignCenter);
    boardLabel->setScaledContents(true);
    QPixmap boardPixmap(":/images/board.png");
    boardLabel->setFixedSize(boardPixmap.width()+150, boardPixmap.height()+150);
    boardLabel->setPixmap(boardPixmap);
    mainLayout->addWidget(boardLabel);
    setFixedSize(boardPixmap.width()+150, boardPixmap.height()+150);

    for(int i=0;i<19;i++){
        for(int j=0;j<19;j++){
            chessBoard[i][j] = new QPushButton(this);
            chessBoard[i][j]->setFixedSize(40, 40);
            chessBoard[i][j]->setIconSize(QSize(50,50));
            chessBoard[i][j]->setFlat(false);
            chessBoard[i][j]->setStyleSheet("background:transparent;border:none;");
            int x = j * 46+22;
            int y = i * 46+20;
            chessBoard[i][j]->move(x, y);
            connect(chessBoard[i][j], &QPushButton::clicked,[this, i, j](){
                slotClicked(i, j);
            });
        }
    }
    //放置以及初始化按钮
    ui->setupUi(this);
    connect(ui->restartButton, &QPushButton::clicked, this, &Gobang::restartGame);
    connect(ui->undoButton, &QPushButton::clicked, this, &Gobang::undoMove);

    //网络初始化
    networkManager = new NetworkManager(this);
    connect(networkManager, &NetworkManager::receiveMove, this, &Gobang::remotePlayerMoved);
    if(mode==Mode::Server){
        networkManager->startServer();
    }else{
        networkManager->connectToServer("127.0.0.1", 12345);
    }
}



Gobang::~Gobang(){
    delete ui;
    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
            delete chessBoard[i][j];
        }
    }
    //QGridLayout以及子对象属于qt,不用释放
}

void Gobang::slotClicked(int x, int y) {
    if (boardData[x][y] != 0) {  // 如果此位置已被占用，直接返回
        return;
    }

    if (isBlackTurn) {
        chessBoard[x][y]->setIcon(QIcon(":/images/black.png"));
        boardData[x][y] = 1;  // 表示黑子
    } else {
        chessBoard[x][y]->setIcon(QIcon(":/images/white.png"));
        boardData[x][y] = 2;  // 表示白子
    }
    if(checkWin(x, y)){
        QMessageBox msgBox;
        msgBox.setText(isBlackTurn? "黑棋胜利": "白棋胜利");
        QPushButton *fRestartButton = msgBox.addButton("重新开始", QMessageBox::AcceptRole);
        QPushButton *fQuitButton = msgBox.addButton("退出游戏", QMessageBox::RejectRole);
        msgBox.exec();

        if(msgBox.clickedButton() == fRestartButton) {
            restartGame();
        } else if(msgBox.clickedButton() == fQuitButton) {
            close();
        }
    }

    isBlackTurn = !isBlackTurn;
    historyStack.push(qMakePair(x,y));

    networkManager->sendData(x, y);
}


bool Gobang::checkWin(int x, int y){
    int currentPlayer = boardData[x][y]; // 获取当前玩家的棋子颜色
    if(currentPlayer == 0) return false;
    //检查上下左右斜边依次是否有五连子
    //水平，数值，主对角线，副对角线
    int dx[] = {1, 0, 1, 1};
    int dy[] = {0, 1, 1, -1};
    for(int dir=0;dir<4;dir++){
        int count = 1;
        for(int i=1;i<5;i++){//前查找
            int newX = x + dx[dir]*i;
            int newY = y + dy[dir]*i;
            if(newX >=0 && newX <19 && newY >=0 && newY < 19 && boardData[newX][newY]==currentPlayer){
                count++;
            }else{
                break;
            }
        }
        for(int i=1;i<5;i++){//后查找
            int newX = x - dx[dir]*i;
            int newY = y - dy[dir]*i;
            if(newX >=0 && newX <19 && newY >=0 && newY < 19 && boardData[newX][newY]==currentPlayer){
                count++;
            }else{
                break;
            }
        }
        if(count >= 5) return true;
    }
    return false;
}

void Gobang::restartGame() {
    // 清除棋盘数据
    memset(boardData, 0, sizeof(boardData));
    while(!historyStack.empty()){
        historyStack.pop();
    }
    for(int i=0; i<19; i++) {
        for(int j=0; j<19; j++) {
            chessBoard[i][j]->setIcon(QIcon());
        }
    }
    isBlackTurn = true; // 如果黑子始终是先手
}

void Gobang::undoMove(){
    if(historyStack.empty()) return;
    QPair<int, int> lastMove = historyStack.top();
    historyStack.pop();
    boardData[lastMove.first][lastMove.second] = 0;
    chessBoard[lastMove.first][lastMove.second]->setIcon(QIcon());
    isBlackTurn = !isBlackTurn;
}

void Gobang::remotePlayerMoved(int x, int y){
    slotClicked(x, y);
}

//----------测量距离---------------
/*
bool Gobang::eventFilter(QObject *watched, QEvent *event) {
    if (watched == boardLabel && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        // 你的测量逻辑
        if(!isStartPointSet){
            startPoint = mouseEvent->pos();
            isStartPointSet = true;
        } else {
            endPoint = mouseEvent->pos();
            isStartPointSet = false;
            qDebug() << "x distance is" << endPoint.x() - startPoint.x();
        }
        return true;
    }
    return QWidget::eventFilter(watched, event);
}
*/
