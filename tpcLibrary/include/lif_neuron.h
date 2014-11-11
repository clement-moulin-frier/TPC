#ifndef LIF_NEURON_H
#define LIF_NEURON_H

class LIF_Neuron
{
public:
    LIF_Neuron(){
        R=1;
        C=10;
        tau_m   = R*C;
        tau_ref = 4;
        tau_count=0;
        spike_delta=80.0;
        v_init=-70.0;
        v=-65.0;
        thresh=-50.0;
        dt=10;
    }



    float update(int I){//updates with current and returns membrane potential afterwards

        //std::cout<<"stimulating with "<<I<<std::endl;

        if (v>=10.0){
            tau_count=1;
            v=v_init;
        }

        else if (tau_count>=tau_ref){

            v+=(1/tau_m)*(-v+R*I);

            if (v>=thresh){
                v+=spike_delta;
            }

        }

        else{
            v+=0;
            tau_count+=1;
        }


        return v;

    }

    float get_mebrane_potential(){
        return v;
    }

    float calculate_in_current(){ //sums all incoming currents

        float current=0.0;

        for (unsigned int i=0; i<in_synapses.size();i++){
            current+=in_synapses[i]->get_incoming();
        }

        return current;
    }


    void refill_synapse(){ //puts membrane potential into all outgoing synapses

        for (unsigned int i=0; i< out_synapses.size();i++){
            if (v>=30.0){
                out_synapses[i]->insert(30.0);
            }
            else{
                out_synapses[i]->insert(0.0);

            }
        }

    }

    void show(){
        //std::cout<<"Number of in_synapses is: "<<in_synapses.size()<<std::endl;
        //std::cout<<"Number of out_synapses is: "<<out_synapses.size()<<std::endl;
        std::cout<<"Membrane potential is: "<<v<<std::endl;

    }



    float tau;
    float v; //membrane potential
    float v_init;
    float R; // membrane resistance
    float C; //membrane capacity
    float tau_m ; // time constant (ms)
    float tau_count;
    float tau_ref;//refractory period (ms)
    float spike_delta;
    float thresh;
    float dt;

    std::vector<Tpc_Synapse*> in_synapses; //ingoing synaptic connections
    std::vector<Tpc_Synapse*> out_synapses;//outgoing


};

#endif // LIF_NEURON_H
