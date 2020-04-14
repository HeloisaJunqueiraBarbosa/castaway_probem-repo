#ifndef WORLD_HH
#define WORLD_HH


#include <GEARSystem/gearsystem.hh>
#include <QObject>

#include "worldsensor.hh"
#include "worldactuator.hh"


class World : public QObject{
    Q_OBJECT

    private:
        quint64 _cycle;
        bool    _isOver;
        WorldSensor* _sensor;
        WorldActuator* _actuator;

        GEARSystem::Angle    _sharkPosition;
        GEARSystem::Position _humanPosition;

        GEARSystem::AngularSpeed _sharkSpeed;
        GEARSystem::Velocity     _humanSpeed;

    public:
        World(WorldSensor* sensor, WorldActuator* actuator);

    private:
        bool checkGameOver();
        quint8 checkWinner();
        float distance(const GEARSystem::Position& a, const GEARSystem::Position& b);
        void save();

    public slots:
        void setSharkSpeed(GEARSystem::AngularSpeed speed);
        void setHumanSpeed(GEARSystem::Velocity speed);
        void updateMap(/*qreal interval*/);

    signals:
        void positionsUpdated(quint64 cycle, GEARSystem::Angle sharkPosition, GEARSystem::Position humanPosition);
};

#endif // WORLD_HH
