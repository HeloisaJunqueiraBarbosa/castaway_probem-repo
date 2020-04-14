#include "worldactuator.hh"

#include <GEARSystem/gearsystem.hh>

WorldActuator::WorldActuator(const QString& name) : GEARSystem::Actuator(name) {
    this->GEARSystem::Actuator::connect(QString::fromUtf8("127.0.0.1"), 0);
}

void WorldActuator::setSpeed(uint8 teamNum, uint8 playerNum, float x, float y, float) {
    // Shark
    if (teamNum == 0 && playerNum == 0) {
        emit sharkSpeedSet(GEARSystem::AngularSpeed(true, x /* /r=1.0 */));
    }

    // Human
    else if (teamNum == 1 && playerNum == 0) {
        emit humanSpeedSet(GEARSystem::Velocity(true, x, y));
    }
}
