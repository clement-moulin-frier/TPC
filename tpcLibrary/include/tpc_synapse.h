#ifndef TPC_SYNAPSE_H
#define TPC_SYNAPSE_H
#include <cmath>
#include <queue>
#include <iostream>
class Izhikevich_Neuron;

class Tpc_Synapse
{
public:
    Tpc_Synapse(int length, float syn_w){
        for (int i=0;i<length;i++){
            spike_train.push_back(0.0);
        }
        weight=syn_w;

    }

	~Tpc_Synapse(){
		//std::cout<<"synapse destructor"<<std::endl; 
	}

    float get_incoming(){//incoming: towards taget neuron
        //return 0.0f;//***
        float out;
        out = spike_train.front();//ankommender Spike
        spike_train.pop_front();
        return out*weight;

    }

    void insert(float cur){
        spike_train.push_back(cur);
    }

    void set_weight(float w){

        weight=w;

    }

    void reset(){
        int len=spike_train.size();
        spike_train.clear();
        for (int i=0;i<len;i++){
            spike_train.push_back(0.0);
        }
    }

protected:
    //Izhikevich_Neuron *target_neuron;
    std::deque<float> spike_train;
    float weight;

};

#endif // TPC_SYNAPSE_H
