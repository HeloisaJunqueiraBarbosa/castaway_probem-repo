#include "world.hh"
#include "worldsensor.hh"
#include "worldactuator.hh"

#include <GEARSystem/gearsystem.hh>

#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>

World::World(WorldSensor* sensor, WorldActuator* actuator) {
    this->_sensor   = sensor;
    this->_actuator = actuator;

    this->_cycle = 0;
    this->_isOver = false;

    this->_sharkSpeed.setValue(0.0);
    this->_humanSpeed.setVelocity(0.0, 0.0);

    // TODO: alterar para aleatório
    this->_sharkPosition.setValue(QRandomGenerator::global()->generateDouble() * GEARSystem::Angle::twoPi);
    qreal abs = QRandomGenerator::global()->generateDouble() * 1.0;
    qreal arg = QRandomGenerator::global()->generateDouble() * GEARSystem::Angle::twoPi;
    this->_humanPosition.setPosition(abs*cos(arg), abs*sin(arg), 0.0);

    QObject::connect(this->_actuator, &WorldActuator::sharkSpeedSet, this, &World::setSharkSpeed);
    QObject::connect(this->_actuator, &WorldActuator::humanSpeedSet, this, &World::setHumanSpeed);
    QObject::connect(this, &World::positionsUpdated, this->_sensor, &WorldSensor::updatePositions);
}

void World::setSharkSpeed(GEARSystem::AngularSpeed speed) {
    this->_sharkSpeed.setValue(speed.value());
}

void World::setHumanSpeed(GEARSystem::Velocity speed) {
    this->_humanSpeed.setVelocity(speed.x(), speed.y());
}

void World::updateMap(/*qreal interval*/) {
    if (!this->_isOver) {
        qreal interval = 0.01;
        this->_sharkPosition.setValue(this->_sharkPosition.value() + this->_sharkSpeed.value()*interval);
        this->_humanPosition.setPosition(this->_humanPosition.x() + this->_humanSpeed.x()*interval, this->_humanPosition.y() + this->_humanSpeed.y()*interval, 0.0);
        this->_cycle++;

        this->save();

        this->_isOver = this->checkGameOver();

        if (this->_isOver) {
            QObject::disconnect(this->_actuator, &WorldActuator::sharkSpeedSet, this, &World::setSharkSpeed);
            QObject::disconnect(this->_actuator, &WorldActuator::humanSpeedSet, this, &World::setHumanSpeed);
            QObject::disconnect(this, &World::positionsUpdated, this->_sensor, &WorldSensor::updatePositions);

            // Converts from r=1.0 and angle to x and y (just ignore the variable type)
            GEARSystem::Velocity shark(true, 1.0, this->_sharkPosition);

            qWarning() << "Game Over!";
            qWarning() << "Shark:" << shark.x() << "x" << shark.y();
            qWarning() << "Human:" << this->_humanPosition.x() << "x" << this->_humanPosition.y();

            quint8 winner = this->checkWinner();

            if (winner == 0) {
                qWarning() << "Shark wins!";
            }
            else {
                qWarning() << "Human wins!";
            }
        }

        else {
            emit positionsUpdated(this->_cycle, this->_sharkPosition, this->_humanPosition);
        }
    }
}

bool World::checkGameOver() {
    return (this->distance(this->_humanPosition, Position(true, 0.0, 0.0, 0.0)) < 1.0) ? false : true;
}

quint8 World::checkWinner() {
    // Converts from r=1.0 and angle to x and y (just ignore the variable type)
    GEARSystem::Velocity sharkAux(true, 1.0, this->_sharkPosition);
    GEARSystem::Position shark(true, sharkAux.x(), sharkAux.y(), 0.0);
    GEARSystem::Position human = this->_humanPosition;

    if (this->distance(shark, human) < 0.01) {
        // Shark wins
        return(0);
    }
    else {
        // Human wins
        return(1);
    }
}

float World::distance(const GEARSystem::Position& a, const GEARSystem::Position& b) {
    return(sqrtf(powf(a.x()-b.x(), 2.0) + powf(a.y()-b.y(), 2.0)));
}

void World::save() {
    QString filename = "data.csv";
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite | QIODevice::Append)) {
        QTextStream stream(&file);
        // Converts from r=1.0 and angle to x and y (just ignore the variable type)
        GEARSystem::Velocity sharkAux(true, 1.0, this->_sharkPosition);
        stream << this->_cycle << ";" << sharkAux.x() << ";" << sharkAux.y() << ";" << this->_humanPosition.x() << ";" << this->_humanPosition.y()  << endl;
    }
}
