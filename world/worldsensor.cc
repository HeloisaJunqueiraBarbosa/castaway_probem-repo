#include "worldsensor.hh"

WorldSensor::WorldSensor() {
    this->_sensor.connect(QString::fromUtf8("127.0.0.1"), 0);
    this->_sensor.addTeam(0, QString::fromUtf8("Shark"));
    this->_sensor.addTeam(1, QString::fromUtf8("Human"));
    this->_sensor.addPlayer(0, 0);
    this->_sensor.addPlayer(1, 0);
}

void WorldSensor::updatePositions(quint64 cycle, GEARSystem::Angle sharkPosition, GEARSystem::Position humanPosition) {
    // Converts from r=1.0 and angle to x and y (just ignore the variable type)
    GEARSystem::Velocity shark(true, 1.0, sharkPosition);

    this->_sensor.setPlayerPosition(0, 0, GEARSystem::Position(true, shark.x(), shark.y(), 0.0));
    this->_sensor.setPlayerPosition(1, 0, humanPosition);
}
