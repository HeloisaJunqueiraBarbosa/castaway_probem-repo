#define _USE_MATH_DEFINES

#include <cmath>
#include <QCoreApplication>
#include <GEARSystem/gearsystem.hh>

using namespace GEARSystem;

Angle angular_distance(const Position& human, const Position& shark) {
    
    Angle angleHuman(true, atan2(human.y(), human.x()));
    Angle angleShark(true, atan2(shark.y(), shark.x()));
    Angle distance(true, Angle::difference(angleHuman, angleShark));
    
    return(distance);
}

//compare the time to the castaway runs in a linear trajectory and the time that it takesto the to shark reach him
bool timeToBorder(const Position& human, const Position& shark){
    float dist_border, timeHuman, timeShark, velocity_shark, velocity_human;
    velocity_human = 1.0;
    velocity_shark = 4.0;
    
    dist_border = 1-(sqrt(pow(human.x(),2)+pow(human.y(),2)));
    Angle angle = angular_distance(human,shark);
	float ang_dist = angle.value();
 
    if(ang_dist > M_PI){
        ang_dist = 2*(M_PI) - ang_dist;
    }
	
    timeHuman = dist_border/velocity_human;
    timeShark = ang_dist/velocity_shark;
    qWarning() << "timeShark:" << timeShark;
    qWarning() << "timeHuman:" << timeHuman;
	
    if(timeShark > timeHuman){
        return true;
    }
    else{
        return false;
    }
}

// linear velocity
Velocity goTo(const Position& human) {
    
    Angle angle(true, atan2(human.y(), human.x()));

    float v_modulo = 1.0;
    Velocity castawayVelocity(true, v_modulo, angle);
    return(castawayVelocity);
}

// velocity to reach the radius to run in circle
Velocity goTo_with_PID(const Position& human, float pdestino, float angular_distance_lastError[3], float* last_aux){
    float aux, dist, dist_destination, distance;
    float dt, Kp, Kd, Td, T0, angular_distance_aux_U0, angular_distance_aux_Q0, angular_distance_aux_Q1, angular_distance_aux_Q2, angular_distance_past;
    dt = 0.1;                                   // interval time to execute a program is 100ms
    Kp = 0.5;                                   //proportional gain
    Kd = 0.01;                                  //derivative gain
    //Ki = 0.0;                                 //integral gain
    T0 = dt;                               
    Td = Kd/Kp;
    //Ti = Kp/Ki;
    angular_distance_aux_U0 = 1;
    angular_distance_aux_Q0 = Kp*(1+(Td/T0)); 
    angular_distance_aux_Q1 = -Kp*(1+(2*(Td/T0))); 			//-(T0/Ti));
    angular_distance_aux_Q2 = Kp*(Td/T0);
    
    Angle angleHuman(true, atan2(human.y(), human.x()));
    Position destino(true, (pdestino*cos(angleHuman.value())), (pdestino*sin(angleHuman.value())), 0.0);
    dist= sqrt(pow(human.x(),2)+pow(human.y(),2));
    dist_destination = sqrt(pow(destino.x(),2)+pow(destino.y(),2));
    distance = dist_destination - dist;
 
    //discrete gain
    angular_distance_lastError[2] = angular_distance_lastError[1];
    angular_distance_lastError[1] = angular_distance_lastError[0];
    angular_distance_lastError[0] = abs(distance);
    angular_distance_past = *last_aux;                  //last measure value
    
    //u(k) = u(k - 1) + q0 e(k) + q1 e(k - 1) + q2 e(k - 2)
    aux = angular_distance_aux_U0*angular_distance_past
        + angular_distance_aux_Q0*angular_distance_lastError[0]
        + angular_distance_aux_Q1*angular_distance_lastError[1]
        + angular_distance_aux_Q2*angular_distance_lastError[2];

    *last_aux = aux;
    //angular position of the castaway
    Angle angle(true, atan2(human.y(), human.x()));

    // linear velocity    
    float v_modulo = aux/0.1;           //v_c = d/t
    Velocity velocity(true, v_modulo, angle);
    return(velocity);
}

// angular velocity
Velocity moveAtRadius(const Position& human, float radius){
    float vx=0.0, vy=0.0, teta = 0.0;
    
    Angle angleHuman(true, atan2(human.y(), human.x()));
    teta = (((1/radius)*(0.1/2))+ angleHuman.value());    //delta_teta = (w*t+teta)=(((v_c)/(t/2))+teta) using a t/2 gives a small interval
    vx = -1*sin(teta);
    vy = 1*cos(teta);
    Velocity velocity(true, vx, vy);

    return(velocity);
}


int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    Controller c;
    c.connect(QString::fromUtf8("127.0.0.1"), 0);

    Position shark;
    Position human;
    bool align = false;
    float radius=0, dist=0, pdestino = 0.0;
    float angular_distance_lastError[3]={0.0,0.0,0.0};
    float last_aux=0.0;
    
    while (true) {                                      //cycle: 100ms      
        shark = c.playerPosition(0, 0);
        human = c.playerPosition(1, 0);

        float humanX = 0.0;
        float humanY = 0.0; 
        

        dist= (sqrt(pow(human.x(),2)+pow(human.y(),2)));
        Angle angular_dist = angular_distance(human, shark);

        if(timeToBorder(human, shark)==true){           								 //verify if the castaway can run linearly to the border before the shark reaches him
            Velocity velocity_linear = goTo(human);                                  //run linearly to reaches the border
            humanX = velocity_linear.x();            
            humanY = velocity_linear.y();
        }
        else                                            								//run around the center in a radius between 0.215<radius<0.225 (the smaller the radius less turns he needs to run)
        { 
            if(align == true ){
                if(dist < 0.250){                       								// run in a circular trajectory with a fixed radius
                    Velocity velocity_angular = moveAtRadius(human, radius);
                    humanX = velocity_angular.x();            
                    humanY = velocity_angular.y();
                }
                
                else{
                    align = false;
                }
            }
            else{                                       								//castaway tries to reach the radius and he is waiting for the shark to align
                if((dist < 0.225) && (dist > 0.215)){     
                    if(((angular_dist.value() < 0.001) && (angular_dist.value() > -0.001))|| (angular_dist.value() > 6.282) || (angular_dist.value() < -6.282) ){
                        humanX = 0.0;
                        humanY = 0.0;
                        align = true;
                        radius = dist;
                    }
                    else{
                        humanX = 0.0;
                        humanY = 0.0;
                    }
                }

                else if(dist < 0.215){
                    pdestino = 0.220;
                    Velocity velocity = goTo_with_PID(human, pdestino, angular_distance_lastError, &last_aux);
                    humanX = velocity.x();          
                    humanY = velocity.y();
                }

                else if(dist > 0.225){
                    pdestino = 0.210;
                    Velocity velocity2 = goTo_with_PID(human, pdestino, angular_distance_lastError, &last_aux);
                    humanX = -velocity2.x();            
                    humanY = -velocity2.y();
                }
            }   
            
        }


        c.setSpeed(1, 0, humanX, humanY, 0.0);

        QThread::msleep(100);
    }

    return app.exec();
}
