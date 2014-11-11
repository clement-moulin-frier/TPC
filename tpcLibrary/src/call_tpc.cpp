#include "call_tpc.h"



callTpc::callTpc(string architecture,int x,int y,int z,int r,bool toroidal, string neighborhood){
	M_X=x;
	M_Y=y;
	M_Z=z;


	bool torus=toroidal;

	if (architecture=="motion-sensitive"){
		 tpcmatrix= new TpcMatrixVector(x,y,6,r,torus,neighborhood);
	}
	else{
		tpcmatrix= new TpcMatrixPosition (x,y,z,r,torus,neighborhood);
	}
	recorder=new Recorder("C:\Projects\Rec");
	vis = new Visualizer (*tpcmatrix);

	status=-1;
	tau=110;


}

callTpc::~callTpc(){
	delete tpcmatrix;
	delete recorder;
	delete vis;

}


void callTpc::set_point(int a,int b,int c, bool store){
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
				tpcmatrix->stimulated.push_back(temp);
		}


	}


	else if (a<M_X && b<M_Y && c<M_Z && a >=0 && b>=0 && c>=0){
		update_matrix[a][b][c]+=input_current;
//        std::cout<<"ok stimulating "<<a<<" "<<b<<" "<<c<<std::endl;
//      std::cout<<"non torus loop"<<std::endl;
		if (store){
			Point3D temp(a,b,c) ;
			tpcmatrix->stimulated.push_back(temp);

	}
	}
	else{
		std::cout<<"not stimulating: "<<a<<" "<<b<<" "<<c<<std::endl;
	}

}

void callTpc::bresenham_3D(Point3D p1,Point3D p2){
		int x1=floor(p1.x+0.5);
		int y1=floor(p1.y+0.5);
		int x2=floor(p2.x+0.5);
		int y2=floor(p2.y+0.5);
		int z1=floor(p1.z+0.5);
		int z2=floor(p2.z+0.5);


		int xd, yd, zd;
		int x, y, z;
		int ax, ay, az;
		int sx, sy, sz;
		int dx, dy, dz;

		dx = x2 - x1;
		dy = y2 - y1;
		dz = z2 - z1;

		ax = ABS(dx) << 1;
		ay = ABS(dy) << 1;
		az = ABS(dz) << 1;

		sx = ZSGN(dx);
		sy = ZSGN(dy);
		sz = ZSGN(dz);

		x = x1;
		y = y1;
		z = z1;

		if (ax >= MAX(ay, az))            /* x dominant */
		{
			yd = ay - (ax >> 1);
			zd = az - (ax >> 1);
			for (;;)
			{
				set_point(x, y, z,true);
				if (x == x2)
				{
					return;
				}

				if (yd >= 0)
				{
					y += sy;
					yd -= ax;
				}

				if (zd >= 0)
				{
					z += sz;
					zd -= ax;
				}

				x += sx;
				yd += ay;
				zd += az;
			}
		}
		else if (ay >= MAX(ax, az))            /* y dominant */
		{
			xd = ax - (ay >> 1);
			zd = az - (ay >> 1);
			for (;;)
			{
				set_point(x, y, z,true);
				if (y == y2)
				{
					return;
				}

				if (xd >= 0)
				{
					x += sx;
					xd -= ay;
				}

				if (zd >= 0)
				{
					z += sz;
					zd -= ay;
				}

				y += sy;
				xd += ax;
				zd += az;
			}
		}
		else if (az >= MAX(ax, ay))            /* z dominant */
		{
			xd = ax - (az >> 1);
			yd = ay - (az >> 1);
			for (;;)
			{
				set_point(x, y, z,true);
				if (z == z2)
				{
					return;
				}

				if (xd >= 0)
				{
					x += sx;
					xd -= az;
				}

				if (yd >= 0)
				{
					y += sy;
					yd -= az;
				}

				z += sz;
				xd += ax;
				yd += ay;
			}
		}
	}

double callTpc::calc_angle(std::vector<double> a, std::vector<double> b){

	double dot=0.0;
	double angle=0.0;
	double norm_a=0.0;
	double norm_b=0.0;

	//calculate dot product
	for (int i =0;i<3;i++){
		dot+=a[i]*b[i];

	}

	//calculate norms
	for(int i=0; i < 3; i++){
		norm_a += a[i]*a[i];
		norm_b += b[i]*b[i];
	}

	norm_a = sqrt( norm_a );
	norm_b = sqrt( norm_b );

	angle = dot/(norm_a*norm_b);

//    if (a[0]==double(1.0) && b[0]==double(1.0)){
//        cout<<"dot is "<<dot<<endl;
//        cout<<"(norm_a*norm_b) is "<<(norm_a*norm_b)<<endl;
//        cout <<"pre angle is "<<angle<<endl;
//        cout<<"radiant is "<<acos(angle)<<endl;

//    }
	angle = acos(angle)*(double(180.0)/M_PI);

	return angle;
}


void callTpc::feed_receptive_field2(std::vector<Point3D> in_field){

	for (int f=0; f<in_field.size()/2;f++){
		//trigger neurons according to the visual field
		//what are the coordinates of the triggered neurons
		int x=floor(in_field[(f*2)+1].x+0.5);
		int y=floor(in_field[(f*2)+1].y+0.5);

		if ((x>M_X || y>M_Y || x <0 || y<0)&&!torus){

			continue;
		}

		//torus
		//if not out of field, stays the same
		x = ((x)<0)?M_X+x:(x)%M_X;
		y=((y)<0)?M_Y+y:(y)%M_Y;


		//now loop through all motion sensitive fields and compare motion direction to tuning field
		for (int z=0; z<M_Z;z++){
			//angle between joint velocity and receptive field
			std::vector<double> vector_a,vector_b;
			vector_a.push_back(double(tpcmatrix->neuron_matrix[x][y][z].receptive_field[0].x));
			vector_a.push_back(double(tpcmatrix->neuron_matrix[x][y][z].receptive_field[0].y));
			vector_a.push_back(double(tpcmatrix->neuron_matrix[x][y][z].receptive_field[0].z));

			vector_b.push_back(double(in_field[(f*2)].x));
			vector_b.push_back(double(in_field[(f*2)].y));
			vector_b.push_back(double(in_field[(f*2)].z));


			double angle = std::abs(calc_angle(vector_a,vector_b));

//            std::cout<<angle<<std::endl;
			if (angle>50.0){
				continue;
			}
			else{
				//velocity
				double vec_length=sqrt(pow(vector_b[0],2)+pow(vector_b[1],2)+pow(vector_b[2],2));
//                std::cout<<x<<","<<y<<","<<z<<": "<<(input_current/10)*(90.0-angle)*vec_length<<std::endl;
				update_matrix[x][y][z]+=(input_current/10)*(90.0-angle)*vec_length;
				Point3D temp(x,y,z);
//              stimulated.push_back(temp);

			}


		}




	}



}


std::vector<Point3D> callTpc::bresenham_getPoints(Point3D p1,Point3D p2){
		//returns all grid points between p1 and p2 in a vector of Point3D

		int x1=floor(p1.x+0.5);
		int y1=floor(p1.y+0.5);
		int x2=floor(p2.x+0.5);
		int y2=floor(p2.y+0.5);
		int z1=floor(p1.z+0.5);
		int z2=floor(p2.z+0.5);


		std::vector<Point3D> out;
		Point3D temp;

		int xd, yd, zd;
		int x, y, z;
		int ax, ay, az;
		int sx, sy, sz;
		int dx, dy, dz;

		dx = x2 - x1;
		dy = y2 - y1;
		dz = z2 - z1;

		ax = ABS(dx) << 1;
		ay = ABS(dy) << 1;
		az = ABS(dz) << 1;

		sx = ZSGN(dx);
		sy = ZSGN(dy);
		sz = ZSGN(dz);

		x = x1;
		y = y1;
		z = z1;

		if (ax >= MAX(ay, az))            /* x dominant */
		{
			yd = ay - (ax >> 1);
			zd = az - (ax >> 1);
			for (;;)
			{
//              set_point(x, y, z,true);
				temp.x=x;
				temp.y=y;
				temp.z=z;
				out.push_back(temp);


				if (x == x2)
				{
					return out;
				}

				if (yd >= 0)
				{
					y += sy;
					yd -= ax;
				}

				if (zd >= 0)
				{
					z += sz;
					zd -= ax;
				}

				x += sx;
				yd += ay;
				zd += az;
			}
		}
		else if (ay >= MAX(ax, az))            /* y dominant */
		{
			xd = ax - (ay >> 1);
			zd = az - (ay >> 1);
			for (;;)
			{
//              set_point(x, y, z,true);
				temp.x=x;
				temp.y=y;
				temp.z=z;
				out.push_back(temp);


				if (y == y2)
				{
					return out;
				}

				if (xd >= 0)
				{
					x += sx;
					xd -= ay;
				}

				if (zd >= 0)
				{
					z += sz;
					zd -= ay;
				}

				y += sy;
				xd += ax;
				zd += az;
			}
		}
		else if (az >= MAX(ax, ay))            /* z dominant */
		{
			xd = ax - (az >> 1);
			yd = ay - (az >> 1);
			for (;;)
			{
//              set_point(x, y, z,true);
				temp.x=x;
				temp.y=y;
				temp.z=z;
				out.push_back(temp);



				if (z == z2)
				{
					return out;
				}

				if (xd >= 0)
				{
					x += sx;
					xd -= az;
				}

				if (yd >= 0)
				{
					y += sy;
					yd -= az;
				}

				z += sz;
				xd += ax;
				yd += ay;
			}
		}
	}



vector<string> callTpc::read_directory(string path) {

	DIR*    dir;
	dirent* pdir;
	std::vector<string> files;

	dir = opendir(path.c_str());

	while (pdir = readdir(dir)) {
		files.push_back(pdir->d_name);
	}

	return files;
}

std::vector<Point3D> callTpc::calculate_relative_pos(std::vector<Point3D> in, Point3D CoM){

		//calculates relative position of joints to the Center of Mass of the whole body

		Point3D temp;
		std::vector<Point3D> out;

		if (in.empty()){
			return out;
		}


		for (unsigned int i= 0;i< in.size();i++){

			temp.x=in[i].x-CoM.x;
			temp.y=in[i].y-CoM.y;
			temp.z=in[i].z-CoM.z;
			out.push_back(temp);

		}

		return out;


	}




std::vector<Point3D> callTpc::map_to_tpc_matrix(std::vector<Point3D> in){

	//scales the input to fit the tpc matrix

		Point3D temp;
		std::vector<Point3D> out;

		if (in.empty()){
			return out;
		}

		int field_length = (M_X<M_Y) ? M_X : M_Y;
		Point3D p (in[0]) ;
		Point3D q (in[1]) ;
		Point3D r(in[2]);
//        Point3D vector_a(p,q) ;
//        Point3D vector_b(r,q) ;

//        float conversion_factor =field_length/2*(vector_a.length()+vector_b.length());
//        float conversion_factor = 6.0;  // => 1m ~ conversion_factor *1 neurons
		float conversion_factor = (field_length/2)+1;
		for (unsigned int i= 0;i< in.size();i++){
			temp.x = (field_length/2)-1 + in[i].x*conversion_factor;
			temp.y = (field_length/2)-1 + in[i].y*conversion_factor;
			if (M_Z==1){
				temp.z=0; //field is flat; no 3rd dimension ====> flatten input
			}
			else{
				temp.z = (field_length/2)-1 + in[i].z*conversion_factor;
			}
			out.push_back(temp);
		}
//        std::cout<<" x in:"<<in[0].x<<" x out:"<<in[0].x*conversion_factor<<std::endl;
//        std::cout<<" y in:"<<in[0].y<<" y out:"<<in[0].y*conversion_factor<<std::endl;
//        std::cout<<" z in:"<<in[0].z<<" z out:"<<in[0].z*conversion_factor<<std::endl;
//        std::cout<<"conversion factor: "<<conversion_factor<<std::endl;
//        std::cout<<"length a: "<<vector_a.length()<<std::endl;
//        std::cout<<"length b: "<<vector_b.length()<<std::endl;


		return out;

}


void callTpc::playback(std::string file_path){

	update_matrix.resize(M_X);
	for (int x=0;x<M_X;x++){
		update_matrix[x].resize(M_Y);
		for (int y=0;y<M_Y;y++){
			update_matrix[x][y].resize(M_Z,0.0);
//            for (int z=0;z<M_Z;z++){

//                update_matrix[x][y][z]=0.0;

//            }
		}
	}


	std::vector<Point3D> kinectInput;

	bool repeat =false;
	std::vector<float> out;

	std::vector<Point3D> temp_vec =recorder->playback_from_file(file_path,false,repeat);

	//if vector empty ---> durchgelaufen...wieder aufnehmen
	if (temp_vec.empty()){
		status=-1;
		//std::cout<<"vector empty"<<std::endl;
		out=conceptualspace.integrate_movement();
		hand_pos.clear();
		tpcmatrix->reset();
		return;

	}


	kinectInput.push_back(temp_vec[0]);
	kinectInput.push_back(temp_vec[1]);
	kinectInput.push_back(temp_vec[2]);
	kinectInput=calculate_relative_pos(kinectInput, temp_vec[3]);
	kinectInput=map_to_tpc_matrix(kinectInput);

	bresenham_3D(kinectInput[0],kinectInput[1]);//sets points in the update matrix
	bresenham_3D(kinectInput[1],kinectInput[2]);

	for (int t=0;t<tau-1;t++){
		if (t==10){//only stimulate 10 ms
			for (int x=0;x<M_X;x++){
				for (int y=0;y<M_Y;y++){
					for (int z=0;z<M_Z;z++){
						update_matrix[x][y][z]=0.0;
					}
				}
			}
		}


		tpcmatrix->update(update_matrix,true);
		vis->visualize(*tpcmatrix);
		//std::cout<<tpcmatrix->avg_mem_pot<<std::endl;
		//kinectInput.clear();
	}

//    if(tpcmatrix->timeline.size()!=100){
//        std::cout<<"Oh oh...timeline:"<<tpcmatrix->timeline.size()<<std::endl;
//    }
	conceptualspace.a_movement.push_back(tpcmatrix->timeline);

	//RESET HERE!
	tpcmatrix->reset();
	return ;

}


Point3D callTpc::getVelocity(Point3D a, Point3D b, Point3D a_velo,Point3D b_velo, Point3D x){

	float d_ax = Point3D(a,x).length();
	float d_ab = Point3D(a,b).length();
	Point3D x_velo;

	double ratio = d_ax/d_ab;

	x_velo.x = (1.0 - ratio)* a_velo.x + ratio*b_velo.x;
	x_velo.y = (1.0 - ratio)* a_velo.y + ratio*b_velo.y;
	x_velo.z = (1.0 - ratio)* a_velo.z + ratio*b_velo.z;

	return x_velo;

}

void callTpc::playback_vector(std::string file_path){


	update_matrix.resize(M_X);
	for (int x=0;x<M_X;x++){
		update_matrix[x].resize(M_Y);
		for (int y=0;y<M_Y;y++){
			update_matrix[x][y].resize(M_Z,0.0);
		}
	}


	bool repeat =false;
	std::vector<float> out;

	std::vector<Point3D> temp_vec =recorder->playback_from_file(file_path,false,repeat);

	//if vector empty ---> durchgelaufen...wieder aufnehmen
	if (temp_vec.empty()){
		status=-1;
		vector<Point3D> rf_input;

//        tpcmatrix->update(rf_input,true);
//        tpcmatrix->update(rf_input,true);
//        tpcmatrix->update(rf_input,true);
//        tpcmatrix->update(rf_input,true);
//        tpcmatrix->update(rf_input,true);

		//std::cout<<"vector empty"<<std::endl;

//        vector<int>::const_iterator first = tpcmatrix->timeline.begin() + 6;
//        vector<int>::const_iterator last = tpcmatrix->timeline.end();
//        vector<int> newVec(first, last);

//        conceptualspace->a_movement.push_back(tpcmatrix->timeline);
		out = conceptualspace.integrate_movement();

		tpcmatrix->reset();
		hand_pos.clear();
		elbow_pos.clear();
		shoulder_pos.clear();
		return;

	}




	vector<Point3D> temp;
	temp.push_back(temp_vec[0]);
	temp.push_back(temp_vec[1]);
	temp.push_back(temp_vec[2]);

	temp= calculate_relative_pos(temp,temp_vec[3]);
	temp= map_to_tpc_matrix(temp);
	Point3D hand (temp[0]);
	Point3D elbow (temp[1]);
	Point3D shoulder (temp[2]);

	hand_pos.push_back(hand);
	elbow_pos.push_back(elbow);
	shoulder_pos.push_back(shoulder);

	vector<Point3D> rf_input;


	if (hand_pos.size()>10){
		hand_pos.pop_front();
	}
	if (elbow_pos.size()>10){
		elbow_pos.pop_front();
	}
	if (elbow_pos.size()>10){
		elbow_pos.pop_front();
	}


//    bool no_zeros  = hand_pos.back().x+hand_pos.back().y+hand_pos.back().z+elbow_pos.back().x+elbow_pos.back().y+elbow_pos.back().z!=0;


	if (hand_pos.size()>1 && elbow_pos.size()>1 && shoulder_pos.size()>1 ){

		Point3D change_hand(hand_pos[hand_pos.size()-2],hand_pos[hand_pos.size()-1]);
		Point3D change_elbow(elbow_pos[elbow_pos.size()-2],elbow_pos[elbow_pos.size()-1]);
		Point3D change_shoulder(shoulder_pos[shoulder_pos.size()-2],shoulder_pos[shoulder_pos.size()-1]);


		Point3D hand_velo;
		Point3D elbow_velo;
		Point3D shoulder_velo;

		Point3D h_pos;
		Point3D e_pos;
		Point3D s_pos;

		//that was just for conversion from kinect joint to Point3D ...right?
		hand_velo.x=change_hand.x;
		hand_velo.y=change_hand.y;
		hand_velo.z=change_hand.z;
//        cout<<hand_velo.x<<" "<<hand_velo.y<<" "<<hand_velo.z<<endl;
		elbow_velo.x=change_elbow.x;
		elbow_velo.y=change_elbow.y;
		elbow_velo.z=change_elbow.z;

		shoulder_velo.x=change_shoulder.x;
		shoulder_velo.y=change_shoulder.y;
		shoulder_velo.z=change_shoulder.z;


		if (hand_velo.length()==0 && elbow_velo.length()==0 &&shoulder.length()==0){ // skip if no movements between two frames
			return;

		}

//        if ((abs(hand_velo.x)>=0.01||abs(hand_velo.y)>=0.01||abs(hand_velo.z)>=0.01)||(abs(elbow_velo.x)>=0.01||abs(elbow_velo.y)>=0.01||abs(elbow_velo.z)>=0.01)){

			h_pos.x=hand_pos[hand_pos.size()-2].x;
			h_pos.y=hand_pos[hand_pos.size()-2].y;
			h_pos.z=hand_pos[hand_pos.size()-2].z;

			e_pos.x=elbow_pos[elbow_pos.size()-2].x;
			e_pos.y=elbow_pos[elbow_pos.size()-2].y;
			e_pos.z=elbow_pos[elbow_pos.size()-2].z;

			s_pos.x=shoulder_pos[shoulder_pos.size()-2].x;
			s_pos.y=shoulder_pos[shoulder_pos.size()-2].y;
			s_pos.z=shoulder_pos[shoulder_pos.size()-2].z;

//            cout<<h_pos.x<<" "<<h_pos.y<<" "<<h_pos.z<<endl;
//            cout<<hand_velo.x<<" "<<hand_velo.y<<" "<<hand_velo.z<<endl;

			//velocity vectors
			rf_input.push_back(hand_velo);
			//position vectors
			rf_input.push_back(h_pos);
			rf_input.push_back(elbow_velo);
			rf_input.push_back(e_pos);
			rf_input.push_back(shoulder_velo);
			rf_input.push_back(s_pos);



			// get all points on the line between the joints and calculate their velocity
			std::vector<Point3D> new_points_lower=bresenham_getPoints(h_pos,e_pos);
			for (int p=0;p<new_points_lower.size();p++){
				Point3D temp_velo=getVelocity(h_pos,e_pos,hand_velo,elbow_velo,new_points_lower[p]);
				rf_input.push_back(temp_velo);
				rf_input.push_back(new_points_lower[p]);

			}


			std::vector<Point3D> new_points_upper=bresenham_getPoints(e_pos,s_pos);
			for (int p=0;p<new_points_upper.size();p++){
				Point3D temp_velo=getVelocity(e_pos,s_pos,hand_velo,elbow_velo,new_points_upper[p]);
				rf_input.push_back(temp_velo);
				rf_input.push_back(new_points_upper[p]);

			}


			//update update matrix
			feed_receptive_field2(rf_input);




			for (int t=0;t<tau-1;t++){

				if (t==10){
					//clear update matrix
					for (int x=0;x<M_X;x++){
						for (int y=0;y<M_Y;y++){
							for (int z=0;z<M_Z;z++){
								update_matrix[x][y][z]=0.0;
							}
						}
					}
				}



				tpcmatrix->update(update_matrix,true);


				vis->visualize(*tpcmatrix);


			}




//            conceptualspace->a_movement.push_back(tpcmatrix->timeline);

			//push concatenated motion fields into a movement
			std::vector<int> concat_tpc;



            for (int t=0; t< ( (TpcMatrixVector*)tpcmatrix)->motion_field_code.size();t++){

                //cout<<((TpcMatrixVector*)tpcmatrix)->motion_field_code.size()<<endl;
                //cout<<((TpcMatrixVector*)tpcmatrix)->motion_field_code[t].size()<<endl;
                concat_tpc.insert(concat_tpc.end(),((TpcMatrixVector*)tpcmatrix)->motion_field_code[t].begin(),((TpcMatrixVector*)tpcmatrix)->motion_field_code[t].end());

                //for (int l =0;l< ((TpcMatrixVector*)tpcmatrix)->motion_field_code[t].size();l++){

                //    cout<<((TpcMatrixVector*)tpcmatrix)->motion_field_code[t][l]<<" ";
                //}


              
            }
            conceptualspace.a_movement.push_back(concat_tpc);




		//    mirrornetwork->a_movement.push_back(tpcmatrix->timeline);

			// RESET

			tpcmatrix->reset();


			//        }


	}
}

void callTpc::process_position(string filename){

	//TODO: make this one big loop with filenames taken as a vector of strings
	// and decision if vector or position tpc


	//-------------------------------------------

	int count=1;

	//string file_path= "C:/FinalBackup/recordings/shifts/vor/";
	string file_path= "C:/FinalBackup/recordings/vor/";


	vector<string> files = read_directory(file_path);
	status=0;
//    std::vector<float> distances;
    

	//to load previously processed saved gestures
	//string cs_load="C:/FinalBackup/newcs/";
	//cs_load = cs_load+filename;
	//conceptualspace.load(cs_load);

	//loop through files in first directory

	for (unsigned int i = 0; i< files.size();i++){

		if (files[i]=="." || files[i]==".."){
			continue;
		}

		std::cout<<count<< " in "<<file_path<<std::endl;
		count++;

		while(status!=-1){


			std::stringstream ss;
			ss << file_path << files[i];
			playback(ss.str());

		}
		status++;

		cout<<endl;
	}
	count=1;
	//file_path = "C:/FinalBackup/recordings/shifts/wink/";
	file_path = "C:/FinalBackup/recordings/wink/";

	status=0;
	files= read_directory(file_path);

	//loop through second directory

	for (unsigned int i = 0; i< files.size();i++){

		if (files[i]=="." || files[i]==".."){
			continue;
		}
		std::cout<<count<< " in "<<file_path<<std::endl;
		count++;

		while(status!=-1){

			std::stringstream ss;
			ss << file_path << files[i];
			playback(ss.str());
		}
		status++;


	}

	//file_path = "C:/FinalBackup/recordings/shifts/P/";
	file_path = "C:/FinalBackup/recordings/P/";
	status=0;
	files= read_directory(file_path);
	count=1;
	//loop through second directory

	for (unsigned int i = 0; i< files.size();i++){

		if (files[i]=="." || files[i]==".."){
			continue;
		}

		std::cout<<count<< " in "<<file_path<<std::endl;
		count++;
		while(status!=-1){

			std::stringstream ss;
			ss << file_path << files[i];
			playback(ss.str());
		}
		status++;

	}

	//file_path = "C:/FinalBackup/recordings/shifts/beugen/";
	file_path = "C:/FinalBackup/recordings/beugen/";
	status=0;
	files= read_directory(file_path);
	count=1;
	//loop through second directory

	for (unsigned int i = 0; i< files.size();i++){

		if (files[i]=="." || files[i]==".."){
			continue;
		}
		std::cout<<count<< " in "<<file_path<<std::endl;
		count++;

		while(status!=-1){

			std::stringstream ss;
			ss << file_path << files[i];
			playback(ss.str());
		}
		status++;

	}


	//---------------------------------------------------

	

	//    conceptualspace->cluster_single_linkage(4);
	string pathname= "C:/FinalBackup/newdists/";
	string cs_path = "C:/FinalBackup/newcs/";
	string shifts ="shifts_";
	//pathname=pathname+shifts+filename;
	pathname=pathname+filename;
	conceptualspace.save_distance_matrix(pathname);
	//pathname=cs_path+shifts+filename;
	pathname=cs_path+filename;
	conceptualspace.save(pathname);
	
	//    cout<<"Clusterspace size: "<<conceptualspace->cluster_space.size()<<endl;
//    for (unsigned int c=0;c<conceptualspace->cluster_space.size();c++ ){
//        cout<<"In Cluster "<<c<<":";
//        for (unsigned int s=0; s<conceptualspace->cluster_space[c].members.size();s++){
//            cout<<conceptualspace->cluster_space[c].members[s]<<" ";

//        }
//        cout<<""<<endl;
//    }

	
	//std::exit(0);
}



void callTpc::process_vector(string filename){

	//TODO: make this one big loop with filenames taken as a vector of strings
	// and decision if vector or position tpc

	int count=1;

	string file_path= "C:/FinalBackup/recordings/vor/";
	//string file_path= "C:/FinalBackup/recordings/shifts/vor/";

	//to load previously saved gestures
	//string cs_load="C:/FinalBackup/newcs/";
	//cs_load = cs_load+filename;
	//conceptualspace.load(cs_load);


	vector<string> files = read_directory(file_path);
	status=0;
//    std::vector<float> distances;
//    conceptualspace->load("/home/icub/recordings/cs/4movs_vector_torus_20x20field.cs");

	//loop through files in first directory

	for (unsigned int i = 0; i< files.size();i++){

		if (files[i]=="." || files[i]==".."){
			continue;
		}

		std::cout<<count<< " in "<<file_path<<std::endl;
		count++;

		while(status!=-1){


			std::stringstream ss;
			ss << file_path << files[i];
			playback_vector(ss.str());

		}
		status++;

		cout<<endl;
	}
	count=1;
	file_path = "C:/FinalBackup/recordings/wink/";
	//file_path = "C:/FinalBackup/recordings/shifts/wink/";

	status=0;
	files= read_directory(file_path);

	//loop through second directory

	for (unsigned int i = 0; i< files.size();i++){

		if (files[i]=="." || files[i]==".."){
			continue;
		}
		std::cout<<count<< " in "<<file_path<<std::endl;
		count++;

		while(status!=-1){

			std::stringstream ss;
			ss << file_path << files[i];
			playback_vector(ss.str());
		}
		status++;


	}

	file_path = "C:/FinalBackup/recordings/P/";
	//file_path = "C:/FinalBackup/recordings/shifts/P/";
	
	status=0;
	files= read_directory(file_path);
	count=1;
	//loop through second directory

	for (unsigned int i = 0; i< files.size();i++){

		if (files[i]=="." || files[i]==".."){
			continue;
		}

		std::cout<<count<< " in "<<file_path<<std::endl;
		count++;
		while(status!=-1){

			std::stringstream ss;
			ss << file_path << files[i];
			playback_vector(ss.str());
		}
		status++;

	}

	file_path = "C:/FinalBackup/recordings/beugen/";
	//file_path = "C:/FinalBackup/recordings/shifts/beugen/";

	status=0;
	files= read_directory(file_path);
	count=1;
	//loop through second directory

	for (unsigned int i = 0; i< files.size();i++){

		if (files[i]=="." || files[i]==".."){
			continue;
		}
		std::cout<<count<< " in "<<file_path<<std::endl;
		count++;

		while(status!=-1){

			std::stringstream ss;
			ss << file_path << files[i];
			playback_vector(ss.str());
		}
		status++;

	}


	//conceptualspace.save("C:/Projects/TpcOutput/cs/sa_vector_torus_nospace.cs");
//    conceptualspace->cluster_single_linkage(4);
	//conceptualspace.save_distance_matrix("C:/Projects/TpcOutput/distanceMatrices/sa_vector_torus_nospace.mat");
	//    cout<<"Clusterspace size: "<<conceptualspace->cluster_space.size()<<endl;
//    for (unsigned int c=0;c<conceptualspace->cluster_space.size();c++ ){
//        cout<<"In Cluster "<<c<<":";
//        for (unsigned int s=0; s<conceptualspace->cluster_space[c].members.size();s++){
//            cout<<conceptualspace->cluster_space[c].members[s]<<" ";

//        }
//        cout<<""<<endl;
//    }


	//string pathname= "C:/FinalBackup/newdists/";
	//string cs_path = "C:/FinalBackup/newcs/";
	//pathname=pathname+filename;
	//conceptualspace.save_distance_matrix(pathname);
	//pathname=cs_path+filename;
	//conceptualspace.save(pathname);


	string pathname= "C:/FinalBackup/newdists/";
	string cs_path = "C:/FinalBackup/newcs/";
	string shifts ="shifts_";
	//pathname=pathname+shifts+filename;
	pathname=pathname+filename;
	conceptualspace.save_distance_matrix(pathname);
	//pathname=cs_path+shifts+filename;
	pathname=cs_path+filename;
	conceptualspace.save(pathname);

}
