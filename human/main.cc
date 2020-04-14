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


bool timeToBorder(const Position& human, const Position& shark){
    float dist_border, tempoHumano, tempoTubarao, velocidade_tubarao, velocidade_humano;
    velocidade_humano = 1.0;
    velocidade_tubarao = 4.0;
    
    dist_border = 1-(sqrt(pow(human.x(),2)+pow(human.y(),2)));
    Angle angle = angular_distance(human,shark);
    tempoHumano = dist_border/velocidade_humano;
    tempoTubarao = angle.value()/velocidade_tubarao;
    qWarning() << "tempoHumano:" << tempoHumano;
    qWarning() << "tempoTubarao:" << tempoTubarao;
    if(tempoTubarao > tempoHumano){
        return true;
    }
    else{
        return false;
    }
}

Position pontoDestino(float x,float y) {
    
    Angle angulo(true, atan2(y, x));
    Position destino(true, cos(angulo.value()), sin(angulo.value()), 0.0);
    return(destino);
}

Velocity goTo(const Position& human, const Position& destino) {
    
    // Pega o angulo da direcao
    Angle angulo(true, atan2(destino.y() - human.y(), destino.x() - human.x()));

    // Gera a velocidade
    float v_modulo = 1.0;
    Velocity velocidade(true, v_modulo, angulo);
    return(velocidade);
}

Velocity goTo_with_PID(const Position& human, float pdestino, float angular_distance_lastError[3], float* last_aux){
    float aux, dist, dist_destino, distance;
    float dt, Kp, Kd, Td, T0, angular_distance_aux_U0, angular_distance_aux_Q0, angular_distance_aux_Q1, angular_distance_aux_Q2, angular_distance_past;
    dt = 0.1;                               // time is 100ms
    Kp = 0.5;                               //proporcional gain
    Kd = 0.01;                               //derivative gain
    //Ki = 0.0;                               //integral gain
    T0 = dt;                               
    Td = Kd/Kp;
    //Ti = Kp/Ki;
    angular_distance_aux_U0 = 1;
    angular_distance_aux_Q0 = Kp*(1+(Td/T0)); 
    angular_distance_aux_Q1 = -Kp*(1+(2*(Td/T0))); //-(T0/Ti));
    angular_distance_aux_Q2 = Kp*(Td/T0);
    
    Angle angleHuman(true, atan2(human.y(), human.x()));
    Position destino(true, (pdestino*cos(angleHuman.value())), (pdestino*sin(angleHuman.value())), 0.0);
    dist= sqrt(pow(human.x(),2)+pow(human.y(),2));
    dist_destino = sqrt(pow(destino.x(),2)+pow(destino.y(),2));
    distance = dist_destino - dist;
 
    //discrete gain
    angular_distance_lastError[2] = angular_distance_lastError[1];
    angular_distance_lastError[1] = angular_distance_lastError[0];
    angular_distance_lastError[0] = abs(distance);
    angular_distance_past = *last_aux;  //valor anterior medido
    
    //u(k) = u(k - 1) + q 0 e(k) + q 1 e(k - 1) + q 2 e(k - 2)
    aux = angular_distance_aux_U0*angular_distance_past
        + angular_distance_aux_Q0*angular_distance_lastError[0]
        + angular_distance_aux_Q1*angular_distance_lastError[1]
        + angular_distance_aux_Q2*angular_distance_lastError[2];

    *last_aux = aux;
    // Pega o angulo da direcao
    Angle angulo(true, atan2(human.y(), human.x()));

    // Gera a velocidade
    float v_modulo = aux/0.1;
    Velocity velocidade(true, v_modulo, angulo);
    return(velocidade);
}

Velocity moveAtRadius(const Position& human, float radius){
    float vx=0.0, vy=0.0, teta = 0.0;
    
    Angle angleHuman(true, atan2(human.y(), human.x()));
    teta = (angleHuman.value()+((1/radius)*(0.1/2)));
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
    
    while (true) {  //itera a cada 100ms 10hz
        shark = c.playerPosition(0, 0);
        human = c.playerPosition(1, 0);

        float humanX = 0.0; //velocidade
        float humanY = 0.0; //
        
        // INSIRA AQUI SUA LOGICA DAQUI...
        dist= (sqrt(pow(human.x(),2)+pow(human.y(),2)));
        Angle angular_dist = angular_distance(human, shark);

        if(timeToBorder(human, shark)==true){            //corra linearmente 
            Velocity velocity_linear = goTo(human, pontoDestino(human.x(), human.y()));
            humanX = velocity_linear.x();            
            humanY = velocity_linear.y();
            qWarning() << "linear";
        }
        
        else                                //corra em diracao ao raio de 0.215 e depois angularmente
        { 
            if(align == true ){              //ja alinhou e vai correr em circulo
                qWarning() << "dist:" << dist;
                qWarning() << "radius:" << radius;
                if(dist < 0.250){
                    Velocity velocity_angular = moveAtRadius(human, radius);
                    humanX = velocity_angular.x();            
                    humanY = velocity_angular.y();
                    qWarning() << "circle";
                }
                else{
                    align = false;
                }
            }
            else{
                if((dist < 0.225) && (dist > 0.215)){     //chegou no raio e vai alinhar com tubarao
                    if(((angular_dist.value() < 0.001) && (angular_dist.value() > -0.001))|| (angular_dist.value() > 6.282) || (angular_dist.value() < -6.282) ){          //parado esperando o tubarao
                        humanX = 0.0;
                        humanY = 0.0;
                        align = true;
                        radius = dist;
                        qWarning() << "radius1:" << radius;
                    }
                    else{
                        humanX = 0.0;
                        humanY = 0.0;
                    }

                }

                if(dist < 0.215){           // de a dist linear for maior corre pra fora 
                    pdestino = 0.220;
                    Velocity velocity = goTo_with_PID(human, pdestino, angular_distance_lastError, &last_aux);
                    humanX = velocity.x();          
                    humanY = velocity.y();
                }

                else if(dist > 0.225){      // se nao corre para dentro
                    pdestino = 0.210;
                    Velocity velocity2 = goTo_with_PID(human, pdestino, angular_distance_lastError, &last_aux);
                    humanX = -velocity2.x();            
                    humanY = -velocity2.y();
                }
            }   
            
        }
        // ...ATE AQUI

        c.setSpeed(1, 0, humanX, humanY, 0.0);

        QThread::msleep(100);
    }

    return app.exec();
}
