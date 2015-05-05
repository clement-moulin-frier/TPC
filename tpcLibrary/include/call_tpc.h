#ifndef CALL_TPC_H
#define CALL_TPC_H


#include "tpcmatrix.h"
#include "conceptual_space.h"
#include "record_standalone.h"
#include "visualize.h"

#define M_PI 3.14159265359
#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <dirent.h>

#ifndef WIN32
    #include <sys/types.h>
#endif

/* absolute value of a */
#define ABS(a) (((a)<0) ? -(a) : (a))

/* take sign of a, either -1, 0, or 1 */
#define ZSGN(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)

#define MAX(a,b) (max(a,b))


//This file handles all the calculations that have to be done before kinect data
//can be handed to a tpcmatrix object

using namespace std;
//namespace tpc
//{

class callTpc{

	
	

public:

	callTpc(string architecture,int x,int y,int z,int r,bool toroidal,string neighborhood);

	~callTpc();


	void set_point(int a,int b,int c, bool store);

	void bresenham_3D(Point3D p1,Point3D p2);

	double calc_angle(std::vector<double> a, std::vector<double> b);


	void feed_receptive_field2(std::vector<Point3D> in_field);

	std::vector<Point3D> bresenham_getPoints(Point3D p1,Point3D p2);



	vector<string> read_directory(string path) ;

	std::vector<Point3D> calculate_relative_pos(std::vector<Point3D> in, Point3D CoM);




	std::vector<Point3D> map_to_tpc_matrix(std::vector<Point3D> in);

	void playback(std::string file_path);
	void playback_vector(std::string file_path);
	Point3D getVelocity(Point3D a, Point3D b, Point3D a_velo,Point3D b_velo, Point3D x);

	void process_position(string filename);


	void process_vector(string filename);


	int M_X;
    int M_Y;
	int M_Z;

	int X_v;
	int Y_v;
	int Z_v;

	bool torus;

	std::vector<std::vector<std::vector<double > > > update_matrix;
	ConceptualSpace conceptualspace;

	TpcMatrix* tpcmatrix;
	
	Recorder* recorder;
	Visualizer* vis;

	int status;
	int tau;

	deque<Point3D> hand_pos; //stores last 10 hand positions
	deque<Point3D> elbow_pos;
	deque<Point3D> shoulder_pos;
	deque<Point3D> com_pos;

};


#endif // CALL_TPC_H
