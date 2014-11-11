#ifndef RECORD_STANDALONE_H
#define RECORD_STANDALONE_H
#include <iostream>
#include <fstream>

#include <sstream>
#include <string>
#include "Point3D.h"

using namespace std;

class Recorder{

public:

	Recorder();

    Recorder(std::string p){
        movement_on=false;
        path=p;
        actual_line=0;

    }



    void record_angles(std::string filename, std::vector<float> angles){
        if (movement_on&&recording_file_angles.is_open()){
            recording_file_angles<<angles[0]<<" "<<angles[1]<<" "<<angles[2]<<"\n";
        }
        else if (movement_on&&!recording_file_angles.is_open()){
            std::stringstream ss;
            ss << path << filename<<"_angles";
            std::string filepath = ss.str();
            recording_file_angles.open(filepath.c_str(),std::ofstream::out);
            std::cout<<"opening new file :"<< filepath<<std::endl;
            recording_file_angles<<angles[0]<<" "<<angles[1]<<" "<<angles[2]<<"\n";
        }

        else if (!movement_on&&recording_file_angles.is_open()){
            recording_file_angles.close();

        }

        else if (!movement_on&&!recording_file_angles.is_open()){
            recording_file_angles.close();

        }



    }






    void set_movement_on(){
        movement_on=true;
    }

    void set_movement_off(){
        movement_on=false;
        if (recording_file.is_open()){
            recording_file.close();
        }

    }

    void gen_random(char *s, const int len) {
        static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        for (int i = 0; i < len; ++i) {
            s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
        }

        s[len] = 0;
    }

    std::vector<Point3D> playback_from_file(std::string file_path,bool loop,bool repeat){

        if (play_me.empty()){ // if playback file not stored already, store it in vector
            std::cout<<"reading from "<<file_path<<std::endl;

            if (infile.is_open()){
                infile.close();
            }

            infile.open(file_path.c_str(),std::ifstream::in);
            std::string line;
            while (std::getline(infile, line)) //Line by Line; each line contains 3joints + COM = (4 * x,y,z)
            {
               Point3D temp;
               std::string segment;
               std::stringstream ss(line);
               int counter = 0;
               while(std::getline(ss, segment, ' ')) //seperating each line and creating new joints
               {
                   double segment_d = ::atof(segment.c_str());

                   if (counter==0){
                       temp.x=segment_d;
                       counter++;
                   }
                   else if (counter ==1){
                       temp.y=segment_d;
                       counter++;
                   }
                   else {
                       temp.z=segment_d;
                       play_me.push_back(temp);
                       counter=0;
                   }



               }

            }

        }
        std::cout<<actual_line/4<<"/"<<play_me.size()/4<<std::endl;
        std::vector<Point3D> out;

        if (actual_line>=play_me.size()&&loop){
            actual_line=0;
            repeat=true;

        }
        else if(actual_line>=play_me.size()&&!loop){
            //std::cout<<"a l:"<<actual_line<<std::endl;
            actual_line=0;
            play_me.clear();
            std::cout<<std::endl;
            return out;
        }



        out.push_back(play_me[actual_line]);
        actual_line++;
        out.push_back(play_me[actual_line]);
        actual_line++;
        out.push_back(play_me[actual_line]);
        actual_line++;
        out.push_back(play_me[actual_line]);

        actual_line++;
        return out;


    }

    std::string get_file_path(){
        return file_path;
    }


protected:

    bool movement_on;
    std::ofstream recording_file;
    std::ofstream recording_file_angles;
    std::string path;
    std::string file_path;

    //playback
    std::ifstream infile;
    std::vector<Point3D> play_me;
    int actual_line;


};
#endif // RECORD_STANDALONE_H
