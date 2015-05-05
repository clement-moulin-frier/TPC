#ifndef IZHIKEVICH_NEURON_H
#define IZHIKEVICH_NEURON_H

#include "tpc_synapse.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include "Point3D.h"

//#define isnan(x) _isnan(x)

using namespace std;

//struct Point3D{//3D point or vector representation
//    double x;
//    double y;
//    double z;

//    Point3D(){}

//    Point3D(int a, int b, int c){
//        x=a;
//        y=b;
//        z=c;

//    }

//    Point3D(float a, float b, float c){
//        x=a;
//        y=b;
//        z=c;

//    }




//    Point3D(Point3D a, Point3D b){
//        x= a.x-b.x;
//        y= a.y-b.y;
//        z = a.z-b.z;
//    }

//    float length() const {
//        return (sqrt(pow(x,2)+pow(y,2)+pow(z,2)));

//    }

//    bool operator<( const Point3D &p ) const {
//        return this->length() < p.length();
//    }

//};


class Izhikevich_Neuron
{
public:
    Izhikevich_Neuron(){
    a=0.05;
    b=0.25;
    c=-65.0;
    d=8.0;
    v=-65.0;
    u=b*v;
    absolute_refrac_time=0; //set to 0 for no absolute refractory time
    cur_refrac_time = absolute_refrac_time;

    //receptive field for joint velocity and joint position

    Point3D joint_velo;
    Point3D joint_pos;
    joint_velo.x=((double)rand()/(double)(RAND_MAX))-0.5;//between -0.5 and 0.5
    joint_velo.y=((double)rand()/(double)(RAND_MAX))-0.5;
    joint_velo.z=((double)rand()/(double)(RAND_MAX))-0.5;

    joint_pos.x=((double)rand()/(double)(RAND_MAX/2))-1.0;//between -1 and 1 (meters)
    joint_pos.y=((double)rand()/(double)(RAND_MAX/2))-1.0;
    joint_pos.z=((double)rand()/(double)(RAND_MAX/2))-1.0;
    receptive_field[0]=joint_velo;
    receptive_field[1]=joint_pos;


//    for (int i=0; i<6;i++){
//        std::cout<<receptive_field[i].x<<" "<<receptive_field[i].y<<" "<<receptive_field[i].z<<std::endl;

//    }
    }



    Izhikevich_Neuron(int max_pref_direction,int max_pref_tpc_count){

    //constructor for integration field neurons
    a=0.05;
    b=0.25;
    c=-65.0;
    d=8.0;
    v=-65.0;
    u=b*v;
    absolute_refrac_time=0; //set to 0 for no absolute refractory time
    cur_refrac_time = absolute_refrac_time;

    integrate_prop[0]=rand() % max_pref_direction;
    integrate_prop[1]=rand() % max_pref_tpc_count;

//    std::cout<<integrate_prop[0]<<" "<<integrate_prop[1]<<std::endl;




    }



    Izhikevich_Neuron(float vx, float vy, float vz,float ox, float oy, float oz){
    a=0.05;
    b=0.25;
    c=-65.0;
    d=8.0;
    v=-65.0;
    u=b*v;
    absolute_refrac_time=0; //set to 0 for no absolute refractory time
    cur_refrac_time = absolute_refrac_time;

    //receptive field for joint velocity and joint position

    Point3D joint_velo;
    Point3D joint_pos;
    joint_velo.x=vx;
    joint_velo.y=vy;
    joint_velo.z=vz;

    joint_pos.x=ox;
    joint_pos.y=oy;
    joint_pos.z=oz;
    receptive_field[0]=joint_velo;
    receptive_field[1]=joint_pos;


    }



    void reset(){

        a=0.05;
        b=0.25;
        c=-65.0;
        d=8.0;
        v=-65.0;
        u=b*v;
        cur_refrac_time = absolute_refrac_time;

    }

    float update(int I){//updates with current and returns membrane potential afterwards

        //std::cout<<"stimulating with "<<I<<std::endl;

        if (v>=30.0){
            v=c;
            u=u+d;
            cur_refrac_time=0;
            return v;
        }

        if (cur_refrac_time<absolute_refrac_time){
            cur_refrac_time++;
            return v;

        }


        v+=0.04*pow(v,2)+5*v+140-u+I;
        u+=a*(b*v-u);



        if (isnan(v) || v<=-150){

//            std::cout<<"resetting..."<<std::endl;
            v=c;
            u=b*v;

        }

        return v;

    }


    float get_mebrane_potential(){
        return v;
    }

    float get_recovery_variable(){
        return u;
    }

    float calculate_in_current(){ //sums all incoming currents

        float current=0.0;

        for (unsigned int i=0; i<in_synapses.size();i++){
            current+=in_synapses[i]->get_incoming();
        }

        return current;
    }


    void refill_synapse(){ //puts membrane potential into all outgoing synapses
        //return; //***
        for (unsigned int i=0; i< out_synapses.size();i++){
            if (v>=30.0){
                out_synapses[i]->insert(30.0);
            }
            else{
                out_synapses[i]->insert(0.0);

            }
        }

    }

    void normalize_synapses(){
        for (unsigned int s=0;s<in_synapses.size();s++){
//            in_synapses[s]->set_weight(0/in_synapses.size());
            in_synapses[s]->set_weight(11.0/in_synapses.size());
//            in_synapses[s]->set_weight(5.0/in_synapses.size());
        }


    }

    void show(){
        //std::cout<<"Number of in_synapses is: "<<in_synapses.size()<<std::endl;
        //std::cout<<"Number of out_synapses is: "<<out_synapses.size()<<std::endl;
        std::cout<<"Membrane potential is: "<<v<<std::endl;

    }


    std::vector<Tpc_Synapse*> in_synapses; //ingoing synaptic connections
    std::vector<Tpc_Synapse*> out_synapses;//outgoing
    Point3D receptive_field [2];
    int integrate_prop[2]; //receptive field for integrate field neurons


protected:

float u;//recovery variable
float v;//membrane potential
float a;
float b;
float c;
float d;
int absolute_refrac_time;//in timesteps
int cur_refrac_time;



};

#endif // IZHIKEVICH_NEURON_H
