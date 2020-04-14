#include <QCoreApplication>
#include <QObject>
#include <GEARSystem/gearsystem.hh>
#include "world.hh"
#include "worldactuator.hh"
#include "worldsensor.hh"
#include "metatypes.hh"

using namespace GEARSystem;

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    qRegisterMetaType<Angle>();
    qRegisterMetaType<AngularSpeed>();
    qRegisterMetaType<Position>();
    qRegisterMetaType<Velocity>();

    WorldSensor* sensor = new WorldSensor();
    WorldActuator* actuator = new WorldActuator(QString::fromUtf8("Castaway"));
    World* world = new World(sensor, actuator);

    QThread worldThread;
    world->moveToThread(&worldThread);

    QThread sensorThread;
    sensor->moveToThread(&sensorThread);

    QThread actuatorThread;
    actuator->moveToThread(&actuatorThread);

    worldThread.start();
    sensorThread.start();
    actuatorThread.start();

    QTimer* timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, world, &World::updateMap);
    timer->start(10);

    return app.exec();
}
