#ifndef WORLDACTUATOR_HH
#define WORLDACTUATOR_HH

#include <GEARSystem/gearsystem.hh>

class WorldActuator : public QObject, public GEARSystem::Actuator {
    Q_OBJECT

    public:
        WorldActuator(const QString& name);

    public:
        virtual void setSpeed(uint8 teamNum, uint8 playerNum, float x, float y, float theta);
        virtual void kick(uint8, uint8, float) {}
        virtual void chipKick(uint8, uint8, float) {}
        virtual void kickOnTouch(uint8, uint8, bool, float) {}
        virtual void chipKickOnTouch(uint8, uint8, bool, float) {}
        virtual void holdBall(uint8, uint8, bool) {}

    signals:
        void sharkSpeedSet(GEARSystem::AngularSpeed sharkSpeed);
        void humanSpeedSet(GEARSystem::Velocity humanSpeed);
};

#endif // WORLDACTUATOR_HH
