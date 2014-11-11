#ifndef TPCMATRIX_H
#define TPCMATRIX_H

//dimensions of tpc field
//#define M_X  10
//#define M_Y  10
//#define M_Z  10

#define input_current 30.0
//#define max_neighbor_distance 2
#define stimulus_time 0
#define record_threshold 10


/* absolute value of a */
#define ABS(a) (((a)<0) ? -(a) : (a))

/* take sign of a, either -1, 0, or 1 */
#define ZSGN(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)



#include "izhikevich_neuron.h"
//#include "lif_neuron.h"
#include <cmath>
#include <queue>
#include <vector>
#include <string>





class TpcMatrix
{



public:
	TpcMatrix();

    TpcMatrix(int m_X,int m_Y,int m_Z,int r, bool toroidal,string neighborhood){

//builds neuron,update and spike matrix
        M_X=m_X;
        M_Y=m_Y;
        M_Z=m_Z;
        radius=r;
        torus =toroidal;

        neuron_matrix.resize(M_X);
        update_matrix.resize(M_X);
        spike_matrix.resize(M_X);

        for (int x=0;x<M_X;x++){
            neuron_matrix[x].resize(M_Y);
            update_matrix[x].resize(M_Y);
            spike_matrix[x].resize(M_Y);


            for (int y=0;y<M_Y;y++){
                neuron_matrix[x][y].resize(M_Z);
                update_matrix[x][y].resize(M_Z);
                spike_matrix[x][y].resize(M_Z);

            }
        }


}


virtual ~TpcMatrix() {};

  virtual void update( std::vector<std::vector < std::vector <double> > > input_matrix,bool spike){;
    //provide body in case children don't provide function
    std::cout<<"i'm here in parent update function"<<std::endl;
  }


  virtual bool isVector()=0;


  void set_point(int a,int b,int c, bool store){
    //apply current to this cell
    // should be called by the method that converts the input to a coordinate in the tpc matrix

    if (torus){
          //place "outside coordinates" back in field
//          std::cout<<"torus loop"<<std::endl;
          a = ((a)<0)?M_X+a:(a)%M_X;
          b=((b)<0)?M_Y+b:(b)%M_Y;
          c = ((c)<0)?M_Z+c:(c)%M_Z;
          update_matrix[a][b][c]+=input_current;
          if (store){
                Point3D temp(a,b,c) ;
                stimulated.push_back(temp);
          }


      }


    else if (a<M_X && b<M_Y && c<M_Z && a >=0 && b>=0 && c>=0){
        update_matrix[a][b][c]+=input_current;
//        std::cout<<"ok stimulating "<<a<<" "<<b<<" "<<c<<std::endl;
//      std::cout<<"non torus loop"<<std::endl;
        if (store){
            Point3D temp(a,b,c) ;
            stimulated.push_back(temp);
      }
    }
    else{
        std::cout<<"not stimulating: "<<a<<" "<<b<<" "<<c<<std::endl;
    }

  }






  void show(){
      std::cout<<"==================================="<<std::endl;
      for (int x=0;x<M_X;x++){
          for (int y=0;y<M_Y;y++){
              for (int z=0;z<M_Z;z++){
                  std::cout<<"Neuron:"<<x<<","<<y<<","<<z<<" :"<<std::endl;
                  neuron_matrix[x][y][z].show();
              }
          }
      }
  }
  void which_spike(){
      for (int x=0;x<M_X;x++){
          for (int y=0;y<M_Y;y++){
              for (int z=0;z<M_Z;z++){

                  if (spike_matrix[x][y][z]==true){
                    std::cout<<x<<" "<<y<<" "<<z<<std::endl;

                  }


              }
          }
      }
  }



  virtual void reset()=0;






//    bool spike_matrix[M_X][M_Y][M_Z];
    std::vector<std::vector<std::vector<bool > > > spike_matrix;
    std::vector<int> timeline; //stores spike_count for a posture over 100ms
    std::vector<Point3D> stimulated; //stimulated neurons
    std::vector < std::pair<Point3D, int> >keep_stimulating; //stores stimulated neurons with counter on how long to keep stimulation
    int spike_count;
    float avg_mem_pot;
    int M_X,M_Y,M_Z; //matrix dimensions
    bool torus;



//    LIF_Neuron neuron_matrix [M_X][M_Y][M_Z];
//    Izhikevich_Neuron neuron_matrix [M_X][M_Y][M_Z];
    std::vector<std::vector<std::vector<Izhikevich_Neuron > > > neuron_matrix;
    //double update_matrix[M_X][M_Y][M_Z];
    std::vector<std::vector<std::vector<double > > > update_matrix;
    int radius;
    bool record; //over threshhold => saving spike count?  old...not in use anymore?
protected:

};

class TpcMatrixPosition: public TpcMatrix
{
public:
    TpcMatrixPosition(int m_X, int m_Y, int m_Z,int r,bool toroidal,string neighborhood):TpcMatrix(m_X,m_Y,m_Z,r, toroidal, neighborhood){



        for (int x=0;x<M_X;x++){

            for (int y=0;y<M_Y;y++){

                for (int z=0;z<M_Z;z++){



                    //connect neurons via synapses


                    for (int a=-radius;a<=radius;a++){
                        for (int b=-radius;b<=radius;b++){
                            for (int c=-radius;c<=radius;c++){
                                if (a==0&&b==0&&c==0){
                                    continue;
                                }


                                if (toroidal){
                                    int dist = int(sqrt(pow(a,2.0)+pow(b,2.0)+pow(c,2.0)));
                                    if (neighborhood=="spherical"&&dist>radius){//for spherical neighborhoods
                                        continue;
                                    }
                                    Tpc_Synapse * temp_synapse = new Tpc_Synapse(dist,0.1);

                                    neuron_matrix[x][y][z].out_synapses.push_back(temp_synapse);
                                    //toroidal:

									if (M_Z+c<0){
										continue;
									}

                                    neuron_matrix[((x+a)<0)?M_X+x+a:(x+a)%M_X][((y+b)<0)?M_Y+y+b:(y+b)%M_Y][((z+c)<0)?M_Z+z+c:(z+c)%M_Z].in_synapses.push_back(temp_synapse);

                                }


                                else if (!(x+a<0)&&!(x+a>=M_X)&&!(y+b<0)&&!(y+b>=M_Y)&&!(z+c<0)&&!(z+c>=M_Z)){ //not outside the field
//
                                    int dist = int(sqrt(pow(a,2.0)+pow(b,2.0)+pow(c,2.0)));
//                                    if (dist>radius){//for spherical neighborhoods
//                                        continue;
//                                    }
                                    Tpc_Synapse * temp_synapse = new Tpc_Synapse(dist,0.1);

                                    neuron_matrix[x][y][z].out_synapses.push_back(temp_synapse);
                                    neuron_matrix[x+a][y+b][z+c].in_synapses.push_back(temp_synapse);
                                    //toroidal:
//                                    neuron_matrix[((x+a)<0)?M_X+x+a:(x+a)%M_X][((y+b)<0)?M_Y+y+b:(y+b)%M_Y][((z+c)<0)?M_Z+z+c:(z+c)%M_Z].in_synapses.push_back(temp_synapse);
                                }
                            }
                        }

                    }




                }
            }
        }


   //normalize synaptic weight
   for (int x=0;x<M_X;x++){
        for (int y=0;y<M_Y;y++){
            for (int z=0;z<M_Z;z++){
                neuron_matrix[x][y][z].normalize_synapses();
            }
        }
    }


    }


	~TpcMatrixPosition(){
		cout<<"calling destructor for TpcMatrixPosition"<<endl;
		for (int x=0;x<M_X;x++){
			for (int y=0;y<M_Y;y++){
				for (int z=0;z<M_Z;z++){
					for (int o=0;o<neuron_matrix[x][y][z].out_synapses.size();o++){
						delete neuron_matrix[x][y][z].out_synapses[o];
					}
					neuron_matrix[x][y][z].out_synapses.clear();
				}
			}
		}	
		
		

	}

    bool isVector(){
        return false;
    }

    /**
    * Update wathever.
    * @param joints description of param
    * @param timestamp when the depth image has been retrieved.
    * @return nothing.
    */
    void update( std::vector<std::vector < std::vector <double> > > input_matrix ,bool spike){

        //update function for the tpc matrix
        //NEEDS to be called every timestep
        //only spiking if spike=true


        //written for kinect joint input
        //gets vector of joints and projects it onto 3d TPC neuron matrix
        //also handles synaptic input from neighboring spiking neurons

        //saves amount of spiking cells for every timestep



        spike_count=0;
        avg_mem_pot=0;

        //clean update and spike matrix
        for (int x=0;x<M_X;x++){
            for (int y=0;y<M_Y;y++){
                for (int z=0;z<M_Z;z++){
                    update_matrix[x][y][z]=0.0;
                    spike_matrix[x][y][z]=false;

                }
            }
        }

//prolonged stimulation.
//only important if stimulus_time > 0
//If stimulus_time > 0 the stimulus will be kept even if you remove input.


        //for (std::vector<std::pair<Point3D,int> >::iterator p_iter=keep_stimulating.begin();p_iter!=keep_stimulating.end();){
        //    p_iter->second-=1;
        //    if (p_iter->second<=0){
        //        keep_stimulating.erase(p_iter);
        //    }
        //    else{
        //        set_point(p_iter->first.x,p_iter->first.y,p_iter->first.z,false);
        //        p_iter++;
        //    }

        //}

        //for (unsigned int v=0;v<stimulated.size();v++){

        //    std::pair<Point3D,int> temp_pair(stimulated[v],stimulus_time);
        //    keep_stimulating.push_back(temp_pair);

        //}
        stimulated.clear();









        for (int x=0;x<M_X;x++){
            for (int y=0;y<M_Y;y++){
                for (int z=0;z<M_Z;z++){

                    //projection input


                    update_matrix[x][y][z]+=input_matrix[x][y][z];
                    if (input_matrix[x][y][z]>0.0){
                        Point3D stim (x,y,z);
                        stimulated.push_back(stim);
                        timeline.clear(); //no recording while projecting
                    }






                    //which neurons get input from neighboring neurons?-->in update matrix
                    update_matrix[x][y][z]+=neuron_matrix[x][y][z].calculate_in_current();

                    //also refill synapses here
                    neuron_matrix[x][y][z].refill_synapse();

                    //now apply update matrix with all calculated currents
                    if(neuron_matrix[x][y][z].update(update_matrix[x][y][z])>=30.0&& spike){
                        spike_matrix[x][y][z]=true;
                        spike_count+=1;
                    }
                    avg_mem_pot+=neuron_matrix[x][y][z].get_mebrane_potential();

                }
            }
        }


        timeline.push_back(spike_count);
        avg_mem_pot/=(M_X*M_Y*M_Z); //average membrane potential of the whole TPC field
    }
    void reset(){

        //resets all neurons to initial conditions.
        //You can (should!) call this after every projection of a new stimulus
        //to avoid spill-over activation from the previous stimulus.
        std::cout<<"calling reset for position TPC"<<std::endl;
        for (int x=0;x<M_X;x++){
            for (int y=0;y<M_Y;y++){
                for (int z=0;z<M_Z;z++){
                     neuron_matrix[x][y][z].reset();
                     for (unsigned int s=0;s<neuron_matrix[x][y][z].out_synapses.size();s++){
                         neuron_matrix[x][y][z].out_synapses[s]->reset();
                     }

                     update_matrix[x][y][z]=0.0;
                     spike_matrix[x][y][z]=false;
                }
            }
        }
    timeline.clear();
    }


};

class TpcMatrixVector: public TpcMatrix
{
public:
    TpcMatrixVector(int m_X,int m_Y,int m_Z,int r, bool toroidal,string neighborhood): TpcMatrix(m_X,m_Y,m_Z,r,toroidal,neighborhood){

        Point3D  directions[6];

        for (int d=0; d<3;d++){
            Point3D temp1;
            Point3D temp2;
            float temp_arr1 []={0.0,0.0,0.0};
            float temp_arr2 []={0.0,0.0,0.0};
            temp_arr1[d]=1.0;
            temp_arr2[d]=-1.0;
            temp1.x=temp_arr1[0];
            temp1.y=temp_arr1[1];
            temp1.z=temp_arr1[2];

            temp2.x=temp_arr2[0];
            temp2.y=temp_arr2[1];
            temp2.z=temp_arr2[2];
            directions[d*2]=temp1;
            directions[d*2+1]=temp2;

        }

        integration_field.resize(M_X);
        update_integration_field.resize(M_X);
        spike_integration_field.resize(M_X);
        motion_field_code.resize(M_Z);
        for (int x=0;x<M_X;x++){
//            std::cout<<"..."<<std::endl;
            integration_field[x].resize(M_Y);
            spike_integration_field[x].resize(M_Y);
            update_integration_field[x].resize(M_Y);
            for (int y=0;y<M_Y;y++){
                integration_field[x][y]=Izhikevich_Neuron(M_Z,M_X*M_Y);

                for (int z=0;z<M_Z;z++){

                    //assign receptive field
                    Point3D pos;
                    pos.x=x;
                    pos.y=y;
                    pos.z=z;
                    neuron_matrix[x][y][z].receptive_field[0]=directions[z]; //velocities/directions
                    neuron_matrix[x][y][z].receptive_field[1]=pos; //position

                    //connect motion field neurons via synapses


                    for (int a=-radius;a<=radius;a++){
                        for (int b=-radius;b<=radius;b++){

                            if (a==0&&b==0){
                                continue;
                            }

                            if (toroidal){
                                int dist = int(sqrt(pow(a,2.0)+pow(b,2.0)));
                                    if (neighborhood=="spherical"&&dist>radius){//for spherical neighborhoods
                                        continue;
                                    }
                                Tpc_Synapse * temp_synapse = new Tpc_Synapse(dist,0.1);

                                neuron_matrix[x][y][z].out_synapses.push_back(temp_synapse);
                                //toroidal:
                                neuron_matrix[((x+a)<0)?M_X+x+a:(x+a)%M_X][((y+b)<0)?M_Y+y+b:(y+b)%M_Y][z].in_synapses.push_back(temp_synapse);

                            }
							


                            else if (!(x+a<0)&&!(x+a>=M_X)&&!(y+b<0)&&!(y+b>=M_Y)){ //not toroidal and not outside field
                                int dist = int(sqrt(pow(a,2.0)+pow(b,2.0)));
                                Tpc_Synapse * temp_synapse = new Tpc_Synapse(dist,0.1);
//                                std::cout<<(x+a)<<" "<<(y+b)<<std::endl;
//                                std::cout<<(x+a)%M_X<<" "<<(y+b)%M_Y<<std::endl;
                                neuron_matrix[x][y][z].out_synapses.push_back(temp_synapse);
                                neuron_matrix[x+a][y+b][z].in_synapses.push_back(temp_synapse);


                            }

                        }

                    }




                }
            }
        }


     //connect integration field via synapses
    for (int x=0;x<M_X;x++){
        for (int y=0;y<M_Y;y++){
            for (int a=-radius;a<=radius;a++){
                for (int b=-radius;b<=radius;b++){

                    if (a==0&&b==0){
                        continue;
                    }
                    else if (!(x+a<0)&&!(x+a>=M_X)&&!(y+b<0)&&!(y+b>=M_Y)){

                        int dist = int(sqrt(pow(a,2.0)+pow(b,2.0)));

                        Tpc_Synapse * temp_synapse = new Tpc_Synapse(dist,0.1);

                        integration_field[x][y].out_synapses.push_back(temp_synapse);
                        integration_field[x+a][y+b].in_synapses.push_back(temp_synapse);

                    }

                }

            }
       }
  }


   //normalize synaptic weight
   for (int x=0;x<M_X;x++){
        for (int y=0;y<M_Y;y++){
            integration_field[x][y].normalize_synapses();
            for (int z=0;z<M_Z;z++){
                neuron_matrix[x][y][z].normalize_synapses();
            }
        }
    }


    }


	~TpcMatrixVector(){
	cout<<"calling destructor for TpcMatrixVector"<<endl;
	for (int x=0;x<M_X;x++){
		for (int y=0;y<M_Y;y++){
			for (int z=0;z<M_Z;z++){
				for (int o=0;o<neuron_matrix[x][y][z].out_synapses.size();o++){
					delete neuron_matrix[x][y][z].out_synapses[o];
				}
				neuron_matrix[x][y][z].out_synapses.clear();
			}
		}
	}	
	}
    bool isVector(){
        return true;
    }

    void update( std::vector<std::vector < std::vector <double> > > input_matrix,bool spike){

        //update function for the tpc matrix
        //NEEDS to be called every timestep
        //only spiking if spike=true


        //written for kinect joint input
        //gets vector of joints and projects it onto 3d TPC neuron matrix
        //also handles synaptic input from neighboring spiking neurons

        //saves amount of spiking cells for every timestep



        spike_count=0;
        avg_mem_pot=0;

        for (int z=0;z<M_Z;z++){
            motion_field_code[z].push_back(0);
        }


        //clean update and spike matrix
        for (int x=0;x<M_X;x++){
            for (int y=0;y<M_Y;y++){
                update_integration_field[x][y]=0.0;
                spike_integration_field[x][y]=false;
                for (int z=0;z<M_Z;z++){
                    update_matrix[x][y][z]=0.0;
                    spike_matrix[x][y][z]=false;

                }
            }
        }

        //prolonged stimulation.
        //only important if stimulus_time > 0
        //If stimulus_time > 0 the stimulus will be kept even if you remove input.


        //for (std::vector<std::pair<Point3D,int> >::iterator p_iter=keep_stimulating.begin();p_iter!=keep_stimulating.end();){
        //    p_iter->second-=1;
        //    if (p_iter->second<=0){
        //        keep_stimulating.erase(p_iter);
        //    }
        //    else{
        //        set_point(p_iter->first.x,p_iter->first.y,p_iter->first.z,false);
        //        p_iter++;
        //    }

        //}

        //for (unsigned int v=0;v<stimulated.size();v++){

        //    std::pair<Point3D,int> temp_pair(stimulated[v],stimulus_time);
        //    keep_stimulating.push_back(temp_pair);

        //}
        stimulated.clear();









        for (int x=0;x<M_X;x++){
            for (int y=0;y<M_Y;y++){
                for (int z=0;z<M_Z;z++){

                    //projection input
//                    if (input_matrix.size()!=1){//if matrix not empty
//                        update_matrix[x][y][z]+=input_matrix[x][y][z];
//                        timeline.clear(); //no recording while projecting
//                    }


                    //projection input
                    //this needs to be changed for the motion fields

                    update_matrix[x][y][z]+=input_matrix[x][y][z];


                    if (input_matrix[x][y][z]>0.0){


                        Point3D stim (x,y,z);
                        stimulated.push_back(stim);

                        for (int z2=0;z2<M_Z;z2++){
                            motion_field_code[z2].clear();
							motion_field_code[z2].push_back(0);
						}

                    }


                    //which neurons get input from neighboring neurons?-->in update matrix
                    update_matrix[x][y][z]+=neuron_matrix[x][y][z].calculate_in_current();

                    //also refill synapses here
                    neuron_matrix[x][y][z].refill_synapse();

                    //now apply update matrix with all calculated currents
                    if(neuron_matrix[x][y][z].update(update_matrix[x][y][z])>=30.0&& spike){
                        spike_matrix[x][y][z]=true;
                        motion_field_code[z].back()+=1;

//                        //inhibit this position in other fields
//                        for (int z_pos=0;z_pos<M_Z&&z_pos!=z;z_pos++){
//                            neuron_matrix[x][y][z_pos].update(-input_current);
//                        }


                    }
                    avg_mem_pot+=neuron_matrix[x][y][z].get_mebrane_potential();



                }
            }
        }


        //now integrate all motion sensitive fields in integration field

        for (int x=0;x<M_X;x++){
            for (int y=0;y<M_Y;y++){

                //projection to integration field from motion maps
                float distance_from_rf=std::abs(integration_field[x][y].integrate_prop[1]-motion_field_code[integration_field[x][y].integrate_prop[0]].back());
                distance_from_rf/=float(M_X*M_Y); //norm to (0-1)
//                std::cout<<x<<","<<y<<","<<z<<" "<<distance_from_rf<<std::endl;
                float input = (input_current/25)*(1-distance_from_rf);
//                std::cout<<input<<std::endl;
                update_integration_field[x][y]+=input ;



                //which neurons get input from neighboring neurons?-->in update matrix
                update_integration_field[x][y]+=integration_field[x][y].calculate_in_current();

                //also refill synapses here
                integration_field[x][y].refill_synapse();

                //now apply update matrix with all calculated currents
                if(integration_field[x][y].update(update_integration_field[x][y])>=30.0&& spike){
                    spike_integration_field[x][y]=true;
                    spike_count+=1;
                }


            }
        }



//        std::cout<<"Spikecount: "<<spike_count<<std::endl;
        timeline.push_back(spike_count);
        avg_mem_pot/=(M_X*M_Y*M_Z); //average membrane potential of the whole TPC field



    }



    void reset(){

        //resets all neurons to initial conditions.
        //You can (should!) call this after every projection of a new stimulus
        //to avoid spill-over activation from the previous stimulus.
        std::cout<<"calling reset for vector tpc"<<std::endl;
        for (int x=0;x<M_X;x++){

            for (int y=0;y<M_Y;y++){
                integration_field[x][y].reset();
                update_integration_field[x][y]=0.0;
                spike_integration_field[x][y]=false;

                for (unsigned int s=0;s<integration_field[x][y].out_synapses.size();s++){
                    integration_field[x][y].out_synapses[s]->reset();
                }

                for (int z=0;z<M_Z;z++){
                     neuron_matrix[x][y][z].reset();
                     for (unsigned int s=0;s<neuron_matrix[x][y][z].out_synapses.size();s++){
                         neuron_matrix[x][y][z].out_synapses[s]->reset();
                     }
                     motion_field_code[z].clear();
                     update_matrix[x][y][z]=0.0;
                     spike_matrix[x][y][z]=false;
                }
            }
        }
    timeline.clear();
    }

    std::vector<std::vector<Izhikevich_Neuron> >integration_field;
    std::vector<std::vector<bool> >spike_integration_field;
    std::vector<std::vector<float> > update_integration_field;
    std::vector<std::vector<int> > motion_field_code; //tp code from motion fields

};
#endif // TPCMATRIX_H
