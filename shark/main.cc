#define _USE_MATH_DEFINES

#include <cmath>
#include <QCoreApplication>
#include <GEARSystem/gearsystem.hh>


using namespace GEARSystem;
//Calculate de angular distance 
float angular_distance(const Position& human, const Position& shark) {
    float distance;
    
    Angle angleHuman(true, atan2(human.y(), human.x()));
    Angle angleShark(true, atan2(shark.y(), shark.x()));
    distance = Angle::difference(angleShark, angleHuman);
    
    return(distance);   
}

//Calculate the discrete gain
void calcullatePID(const Position& human, const Position& shark, float angular_distance_lastError[3], float* last_aux){
    float aux;
    float dt, Kp, Kd, Td, T0, angular_distance_aux_U0, angular_distance_aux_Q0, angular_distance_aux_Q1, angular_distance_aux_Q2, angular_distance_past;
    dt = 0.1;                               //interval time to execute a program is 100ms
    Kp = 0.3;                               //proporcional gain
    Kd = 0.07;                              //derivative gain
    //Ki = 0.00;                            //integral gain
    T0 = dt;                               
    Td = Kd/Kp;
    //Ti = Kp/Ki;
    angular_distance_aux_U0 = 1;
    angular_distance_aux_Q0 = Kp*(1+(Td/T0)); 
    angular_distance_aux_Q1 = -Kp*(1+(2*(Td/T0)));
    angular_distance_aux_Q2 = Kp*(Td/T0);
    
    Angle angleHuman(true, atan2(human.y(), human.x()));
    Angle angleShark(true, atan2(shark.y(), shark.x()));
    Angle distance(true, Angle::difference(angleHuman, angleShark));
    float ang_dist = distance.value();                  //angular distance

    
    angular_distance_lastError[2] = angular_distance_lastError[1];
    angular_distance_lastError[1] = angular_distance_lastError[0];
    angular_distance_lastError[0] = abs(ang_dist);
    angular_distance_past = *last_aux;                  //last measure value
    
    //u(k) = u(k - 1) + q0 e(k) + q1 e(k - 1) + q2 e(k - 2)
    aux = angular_distance_aux_U0*angular_distance_past
        + angular_distance_aux_Q0*angular_distance_lastError[0]
        + angular_distance_aux_Q1*angular_distance_lastError[1]
        + angular_distance_aux_Q2*angular_distance_lastError[2];

    *last_aux = aux;

}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    Controller c;
    c.connect(QString::fromUtf8("127.0.0.1"), 0);

    Position shark;
    Position human;
    float angular_distance_lastError[3]={0.0,0.0,0.0};
    float last_aux=0.0 , sharkLinearSpeedAnt = 0.0;
    
    while (true) {                                  //cycle: 100ms
        shark = c.playerPosition(0, 0);             //get the shark position
        human = c.playerPosition(1, 0);             //get the human position
        
        float v_s = 4.0, max = 4.0, min = 0.0;
        float sharkLinearSpeed = 0.0;

        float distance = angular_distance(human, shark);
        
        //calculate de angular distance using a discrete PID
        calcullatePID( human, shark, angular_distance_lastError, &last_aux);
        
        v_s = last_aux/0.1;          // w=delta_teta/t

        // Restrict to max/min velocity
        if( v_s > max )
            v_s = max;
        else if( v_s < min )
            v_s = min;
        
        sharkLinearSpeed = v_s;
        if(distance == M_PI ) {                         
            if(sharkLinearSpeedAnt >= 0){
                sharkLinearSpeed = 4.0;
            }
            else{
                sharkLinearSpeed = -4.0;
            }
        }
        else if(distance == 0.0){                       //if align with he human wait
            sharkLinearSpeed = 0.0;
        }
        else if(((distance < 0) && (distance > -(M_PI))) || ((distance > (M_PI)))){                 // run clockwise
            sharkLinearSpeed = v_s;
        }
        else if(((distance > 0) && (distance < (M_PI))) || ((distance < -(M_PI)))){                 // run counterclockwise
            sharkLinearSpeed = -v_s;
        }

        sharkLinearSpeedAnt = sharkLinearSpeed;
        c.setSpeed(0, 0, sharkLinearSpeed, 0.0, 0.0);

        QThread::msleep(100);
    }

    return app.exec();
}