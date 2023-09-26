#include <QApplication>
#include "../Gobang.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Gobang gobang(Mode::Client);

    gobang.show();

    return app.exec();
}

