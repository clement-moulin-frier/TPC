/*
author Jan Schneider
*/

//#include "Point3D.h"
//#include "call_tpc.h"
#include "tpcLibrary/include/call_tpc.h"
#include <sstream>
#include <string>

using namespace std;

/*
int M_X =10;
int M_Y =10;
int M_Z =10;
*/
//vector< vector<vector<double> > > getInfoFromSensor()
//{
//    vector<float> mySensorValues;
//
//    vector< vector<vector<double> > > updateMatrix;
//    return updateMatrix;
//}



void parameter_space(bool motion_sens){

	cout<<"parameter space evaluation"<<endl;



	if (motion_sens){
		//motion-sensitive
		for (int xy=20; xy<=20; xy+=10){
			//for (int z=10;z<=40; z+=10){
			for(int r=4;r<=5;r++){ 
				for (int t=0;t<=1;t++){
					for (int n=0;n<=1;n++){
						bool torus = (t != 0);
						string neighbor="cubical";
						if (n==1){
							neighbor="spherical";
						}
						callTpc cTpc("motion-sensitive",xy,xy,6,r,torus,neighbor);
						
						string toroidal="bounded";
						if (torus){
							toroidal="toroidal";
						}

						std::stringstream ss;
						ss << "motion_" << xy <<"x"<<xy<<"x"<<6<<"_"<<r<<"_"<<toroidal<<"_"<<neighbor<<".mat";
						string filename = ss.str();
						cout<<"running configuration: "<<filename<<endl;
						
						
						cTpc.process_vector(filename);
						
					}
				
				}

			}
			

		}


	}

	else{
		//position-sensitive
		for (int xy=10; xy<=40; xy+=10){
			for (int z=10;z<=40; z+=10){
			//for (int z=1;z<=1; z+=10){ //bei flat wird nichts projeziert...warum?
				for(int r=3;r<=5;r++){ 
					for (int t=1;t<=1;t++){
						for (int n=0;n<=1;n++){
							bool torus = (t != 0);
							string neighbor="cubical";
							if (n==1){
								neighbor="spherical";
							}
							callTpc cTpc("position",xy,xy,z,r,torus,neighbor);
						
							string toroidal="bounded";
							if (torus){
								toroidal="toroidal";

							}

							std::stringstream ss;
							ss << "position_" << xy <<"x"<<xy<<"x"<<z<<"_"<<r<<"_"<<toroidal<<"_"<<neighbor<<".mat";
							string filename = ss.str();
							cout<<"running configuration: "<<filename<<endl;
						
						
							cTpc.process_position(filename);
						
						}
				
					}

				}
			}

		}

	}

	


	//motion-sensitive

	//DESTRUCTORS FOR TPCVECTOR!!!

	std::exit(0);
}


int main(int argc, char *argv[])
{
	
	//automatic paramater space testing
	//parameter_space(true);

	int xy=10; //width&height of neuronal population
	int z=1; //depth (for position-sensitive architectures) or number of motion-sensitive fields (then should be 6) 
	bool torus=false; //toroidal or bounded architecture
	int r=3; //neighborhood connectivity radius
	string type ="position-sensitive"; //type of architecture: "motion-sensitive" or "position-sensitive"
	string neighbor = "cubical"; //shape of neighborhood: "cubical" or "spherical"
		
	//create TPC Object
	callTpc cTpc(type,xy,xy,z,r,torus,neighbor);
	

	//some output to inform about specifications of the configuration
	string toroidal="bounded";
	if (torus){
		toroidal="toroidal";
	}
	std::stringstream ss;
	ss << "position_" << xy <<"x"<<xy<<"x"<<z<<"_"<<r<<"_"<<toroidal<<"_"<<neighbor<<".mat";
	string filename = ss.str();
	cout<<"running configuration: "<<filename<<endl;
					


	//run gesture set with the previously initialized architecture and save distance matrix and conceptual space under filename.mat and filename.cs
	if (type=="motion-sensitive"){
		cTpc.process_vector(filename);
	}
	else{
		cTpc.process_position(filename);
	}

    return 0;
}

