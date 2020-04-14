#ifndef WORLDSENSOR_HH
#define WORLDSENSOR_HH

#include <QObject>
#include <GEARSystem/gearsystem.hh>

class WorldSensor : public QObject {
    Q_OBJECT

    private:
        GEARSystem::Sensor _sensor;

    public:
        WorldSensor();

    public slots:
        void updatePositions(quint64 cycle, GEARSystem::Angle sharkPosition, GEARSystem::Position humanPosition);
};

#endif // WORLDSENSOR_HH
