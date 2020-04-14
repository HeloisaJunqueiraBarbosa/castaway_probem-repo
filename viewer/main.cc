#include <QCoreApplication>
#include <GEARSystem/gearsystem.hh>

using namespace GEARSystem;

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    Controller c;
    c.connect(QString::fromUtf8("127.0.0.1"), 0);

    Position shark, human;
    while (true) {
        shark = c.playerPosition(0, 0);
        human = c.playerPosition(1, 0);

        qWarning() << "Shark:" << shark.x() << "x" << shark.y();
        qWarning() << "Human:" << human.x() << "x" << human.y();

        QThread::msleep(100);
    }

    return app.exec();
}
