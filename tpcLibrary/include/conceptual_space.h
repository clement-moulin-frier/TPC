#ifndef CONCEPTUAL_SPACE_H
#define CONCEPTUAL_SPACE_H

#include <vector>
#include <cmath>
#include <numeric>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <sstream>

struct Cluster{

    std::vector < std::vector<int> >  centroid; //centroid of cluster
    std::vector<int> members; // index of movements in movement_space that are inside this cluster

    float threshold; //placeholder for now

};

class ConceptualSpace{
public:

    ConceptualSpace(){



    }

    float mov_distance(std::vector< std::vector <int > > mov1,std::vector< std::vector <int > > mov2){
        std::vector<std::vector<int> > short_mov;
        std::vector<std::vector<int> > long_mov;


        //compare the shorter to the longer one
        if (mov1.size()<=mov2.size()){
            short_mov=mov1;
            long_mov = mov2;
        }
        else{
            long_mov=mov1;
            short_mov = mov2;
        }


        float this_min_dist=999999.0; //minimal distance only to this compared movement
        for(unsigned int s=0; s<long_mov.size()-short_mov.size()+1;s++){ //start position for comparing in long pattern

            std::vector<float> dis_vec;
            for (unsigned int w=0; w< short_mov.size();w++){

                dis_vec.push_back(compare_postures(short_mov[w],long_mov[s+w]));

            }

            float sum_of_elems =std::accumulate(dis_vec.begin(),dis_vec.end(),0)/float(short_mov.size());
            this_min_dist=std::min(this_min_dist,sum_of_elems);

        }
        return this_min_dist;

    }


    int closest_cluster(std::vector< std::vector <int > > mov){
        //returns closest cluster
        float min_dist=999999;
        int closest_cluster=9;
//        std::cout<<"Size of Clusterspace: "<<cluster_space.size()<<std::endl;
        for (int c=0; c<cluster_space.size();c++){
            float this_dist=999999;
            this_dist = distance_to_cluster(c,mov);

//            std::cout<<"distance to cluster "<<c<<": "<<this_dist<<std::endl;
            if (this_dist<min_dist){
                closest_cluster=c;
                min_dist=this_dist;
            }

        }
//        std::cout<<"min distance:"<<min_dist<<" cluster:"<<closest_cluster<<std::endl;
        return closest_cluster;

    }


    float distance_to_cluster(int cluster_index,std::vector< std::vector <int > > mov){
        //calculates distance of a movement to the indicated cluster
        float distance=0;

        for (int m=0; m<cluster_space[cluster_index].members.size();m++){
            distance+=mov_distance(mov,movement_space[cluster_space[cluster_index].members[m]]);
        }
        return (distance/float(cluster_space[cluster_index].members.size()));

    }


    std::vector<std::vector<int> > calculate_new_centroid(std::vector< std::vector <int > > mov1,std::vector< std::vector <int > > mov2){

        std::vector<std::vector<int> > short_mov;
        std::vector<std::vector<int> > long_mov;


        if (mov1.size()<=mov2.size()){
            short_mov=mov1;
            long_mov = mov2;
        }
        else{
            long_mov=mov1;
            short_mov = mov2;
        }


        float this_min_dist=999999.0; //minimal distance only to this compared movement
        int starting_point=999999;
        for(unsigned int s=0; s<long_mov.size()-short_mov.size()+1;s++){ //start position for comparing in long pattern

            std::vector<float> dis_vec;
            for (unsigned int w=0; w< short_mov.size();w++){

                dis_vec.push_back(compare_postures(short_mov[w],long_mov[s+w]));

            }

            float sum_of_elems =std::accumulate(dis_vec.begin(),dis_vec.end(),0)/float(short_mov.size());

            if (sum_of_elems<this_min_dist){
                this_min_dist=sum_of_elems;
                starting_point=s;
            }



        }


        std::vector< std::vector <int > > out_mov;
        for (unsigned int w=0; w< short_mov.size();w++){//merge movements
            std::vector<int> tempvec;
            for (unsigned int v=0;v<short_mov[w].size();v++){//merge postures
                //std::cout<<int((long_mov[starting_point+w][v]+short_mov[w][v])/2)<<",";
                tempvec.push_back(int((long_mov[starting_point+w][v]+short_mov[w][v])/2));
            }
            out_mov.push_back(tempvec);

        }



        return out_mov;
    }



    void cluster_single_linkage(int num_catergories){
    //clusters complete movement space with single linkage
        cluster_space.clear();
        std::vector <std::vector< std::vector <int > > > to_cluster =movement_space;

        for (unsigned int i=0; i<to_cluster.size();i++){
            //in the beginning everything is its own cluster
            Cluster tempCluster;
            //tempCluster.centroid = to_cluster[i];
            tempCluster.members.push_back(i);
            cluster_space.push_back(tempCluster);
        }

        while (cluster_space.size()>num_catergories){



            //calculating new distance matrix
            //int distance_matrix[cluster_space.size()][cluster_space.size()]={};
            float min_dis=99999999.0;
            int index1;
            int index2;

            for (int x=0; x<cluster_space.size();x++){
                for (int y=0; y<cluster_space.size();y++){
                    //distance_matrix[x][y]=mov_distance(cluster_space[x].centroid,cluster_space[y].centroid);

                    if (x==y){
                        continue;
                    }

                    float this_distance=9999999.0;
                    for (unsigned int a=0; a<cluster_space[x].members.size();a++){
                        for (unsigned int b=0; b<cluster_space[y].members.size();b++){
                            float cur_element_to_element_dist=mov_distance(movement_space[cluster_space[x].members[a]],movement_space[cluster_space[y].members[b]]);
                            if (cur_element_to_element_dist<this_distance){
                                this_distance=cur_element_to_element_dist;
                            }
                        }
                    }



                    if (this_distance<min_dis){
                       index1=x;
                       index2=y;
                       min_dis=this_distance;
                    }

                }
            }

        std::cout<<"merging "<<index1<<" and "<<index2<<" with distance "<<min_dis<<std::endl;
        Cluster newCluster;
        //newCluster.centroid=calculate_new_centroid(cluster_space[index1].centroid,cluster_space[index2].centroid);
        newCluster.members=cluster_space[index1].members;
        newCluster.members.insert( newCluster.members.end(), cluster_space[index2].members.begin(), cluster_space[index2].members.end() );

        if (index1<index2){

            cluster_space.erase(cluster_space.begin()+index1);
            cluster_space.erase(cluster_space.begin()+index2-1);
        }

        else{
            cluster_space.erase(cluster_space.begin()+index2);
            cluster_space.erase(cluster_space.begin()+index1-1);

        }

        cluster_space.push_back(newCluster);

        for (unsigned int c=0;c<cluster_space.size();c++ ){
            std::cout<<"In Cluster "<<c<<":";
            for (unsigned int s=0; s<cluster_space[c].members.size();s++){
                std::cout<<cluster_space[c].members[s]<<" ";

            }
            std::cout<<""<<std::endl;
        }



        }

        compute_cluster_distances2();
        silhouette();

    }



    void cluster_all(int num_catergories){
    //clusters complete movement space
        cluster_space.clear();
        std::vector <std::vector< std::vector <int > > > to_cluster =movement_space;

        for (unsigned int i=0; i<to_cluster.size();i++){
            //in the beginning everything is its own cluster
            Cluster tempCluster;
            tempCluster.centroid = to_cluster[i];
            tempCluster.members.push_back(i);
            cluster_space.push_back(tempCluster);
        }

        while (cluster_space.size()>num_catergories){



            //calculating new distance matrix
            //int distance_matrix[cluster_space.size()][cluster_space.size()]={};
            float min_dis=99999999.0;
            int index1;
            int index2;

            for (int x=0; x<cluster_space.size();x++){
                for (int y=0; y<cluster_space.size();y++){
                    //distance_matrix[x][y]=mov_distance(cluster_space[x].centroid,cluster_space[y].centroid);
                    float this_distance=mov_distance(cluster_space[x].centroid,cluster_space[y].centroid);
                    if (this_distance<min_dis && x!=y){
                       index1=x;
                       index2=y;
                       min_dis=this_distance;
                    }

                }
            }

        std::cout<<"merging "<<index1<<" and "<<index2<<" with distance "<<min_dis<<std::endl;
        Cluster newCluster;
        newCluster.centroid=calculate_new_centroid(cluster_space[index1].centroid,cluster_space[index2].centroid);
        newCluster.members=cluster_space[index1].members;
        newCluster.members.insert( newCluster.members.end(), cluster_space[index2].members.begin(), cluster_space[index2].members.end() );

        if (index1<index2){

            cluster_space.erase(cluster_space.begin()+index1);
            cluster_space.erase(cluster_space.begin()+index2-1);
        }

        else{
            cluster_space.erase(cluster_space.begin()+index2);
            cluster_space.erase(cluster_space.begin()+index1-1);

        }

        cluster_space.push_back(newCluster);

        for (unsigned int c=0;c<cluster_space.size();c++ ){
            std::cout<<"In Cluster "<<c<<":";
            for (unsigned int s=0; s<cluster_space[c].members.size();s++){
                std::cout<<cluster_space[c].members[s]<<" ";

            }
            std::cout<<""<<std::endl;
        }



        }


        compute_cluster_distances();
//        silhouette();

    }


    void compute_cluster_distances(){
        for (unsigned int c1 = 0; c1< cluster_space.size();c1++){

            for (unsigned int c2 = 0; c2< cluster_space.size();c2++){
                std::cout<<"Distance "<<c1<<","<<c2<<": "<<mov_distance(cluster_space[c1].centroid,cluster_space[c2].centroid)<<std::endl;
            }
            float average_inner_cluster_dist=0;
            for (unsigned int i=0; i<cluster_space[c1].members.size();i++){
                int index1=cluster_space[c1].members[i];
                for (unsigned int j=0;j<cluster_space[c1].members.size()&&j!=i;j++){
                    int index2=cluster_space[c1].members[j];
                    average_inner_cluster_dist+=mov_distance(movement_space[index1],movement_space[index2]);
                }
                average_inner_cluster_dist/=(cluster_space[c1].members.size()-1);
            }
            average_inner_cluster_dist/=cluster_space[c1].members.size();
            std::cout<<"Inner Distance:"<<average_inner_cluster_dist<<std::endl;

        }


    }

    void compute_cluster_distances2(){
        //for single linkage clustering
        for (unsigned int c1 = 0; c1< cluster_space.size();c1++){

            for (unsigned int c2 = 0; c2< cluster_space.size();c2++){
                float shortest_distance = 99999999;
                for (int mov1_index=0; mov1_index<cluster_space[c1].members.size();mov1_index++){
                    for (int mov2_index=0; mov2_index<cluster_space[c2].members.size();mov2_index++){
                        float distance = mov_distance(movement_space[cluster_space[c1].members[mov1_index]], movement_space[cluster_space[c2].members[mov2_index]]);
                        if (distance< shortest_distance){
                            shortest_distance=distance;
                        }

                    }


                }



                std::cout<<"Distance "<<c1<<","<<c2<<": "<< shortest_distance<<std::endl;
            }


            float max_inner_cluster_dist=0;
            for (unsigned int i=0; i<cluster_space[c1].members.size();i++){
                int index1=cluster_space[c1].members[i];
                for (unsigned int j=0;j<cluster_space[c1].members.size()&&j!=i;j++){
                    int index2=cluster_space[c1].members[j];
                    float dist=mov_distance(movement_space[index1],movement_space[index2]);
                    if (dist>max_inner_cluster_dist){
                        max_inner_cluster_dist=dist;
                    }
                }

            }

            std::cout<<"Max Intra Distance:"<<max_inner_cluster_dist<<std::endl;

        }


    }


    void silhouette(){
        //first compute average dissimilarity to own cluster elements for each cluster
        for (int c=0; c<cluster_space.size();c++){//loop over clusters
            std::cout<<"Cluster "<<c<<":"<<std::endl;
            std::vector<float> avg_dissimilarity;
            for (int i=0; i<cluster_space[c].members.size();i++){//every element of a cluster
                float avg_dis=0;
                float low_avg_dis=0;
                float lowest_avg_dis=99999999999;
                int nearest_cluster=999;
                std::cout<<"a("<<i<<") = ";
                for (int j=0; j<cluster_space[c].members.size();j++){//to every other element in the same cluster
                    if (i==j){
                        continue;
                    }
                    avg_dis+=mov_distance(movement_space[cluster_space[c].members[i]],movement_space[cluster_space[c].members[j]]);

                }
                for (int d=0; d<cluster_space.size();d++){
                    if (d==c){
                        continue;
                    }
                    for (int k=0;k<cluster_space[d].members.size();k++){ //also over all other elements in all other clusters
                        low_avg_dis+=mov_distance(movement_space[cluster_space[c].members[i]],movement_space[cluster_space[d].members[k]]);
                    }
                    low_avg_dis/=float(cluster_space[d].members.size());
                    if (low_avg_dis<lowest_avg_dis){
                        lowest_avg_dis=low_avg_dis;
                        nearest_cluster=d;
                    }
                }
                avg_dis=avg_dis/float(cluster_space[c].members.size()-1);
                avg_dissimilarity.push_back(avg_dis);
                std::cout<<avg_dis<<"   ";
                std::cout<<"b("<<i<<") = "<<lowest_avg_dis<<"    ";
                std::cout<<"s("<<i<<") = "<<(lowest_avg_dis-avg_dis)/std::max(lowest_avg_dis,avg_dis);
                std::cout<<"    nearest cluster: "<<nearest_cluster<<std::endl;
            }


        }
        return;
    }


    void save_distance_matrix(std::string file_path){
        std::ofstream outfile;
        outfile.open(file_path.c_str());
        for (int m1=0; m1<movement_space.size();m1++){//each movement
            for (int m2=0;m2<movement_space.size();m2++){
                float dist=mov_distance(movement_space[m1],movement_space[m2]);
                outfile<<dist;
                if (m2!=movement_space.size()-1){
                    outfile<<"  ";
                }

            }
            outfile<<"\n";
        }
        outfile.close();
    }

    void cluster_new(float threshold,std::vector<float> distances){
    //adds last movement to a cluster
        if (distances.empty()){

            Cluster new_Cluster;
            new_Cluster.centroid=movement_space.back();
            new_Cluster.members.push_back(distances.size()); //push back index of new movement
            cluster_space.push_back(new_Cluster);

            return;

        }


        float min = *std::min_element(distances.begin(), distances.end());


        if (min<=threshold){
            //find out which cluster min distance to and assign to the same
            std::vector<float>::iterator it = std::find(distances.begin(), distances.end(), min);
            int index = std::distance(distances.begin(),it);

            for (unsigned int c=0;c<cluster_space.size();c++){
                if(std::find(cluster_space[c].members.begin(), cluster_space[c].members.end(), index) != cluster_space[c].members.end()){

                    cluster_space[c].members.push_back(distances.size());//put this element in the same cluster
                    //cluster_space[c].compute_centroid();
                    break;
                }

            }



        }
        else{
            Cluster new_Cluster;
            new_Cluster.centroid=movement_space.back();
            new_Cluster.members.push_back(distances.size()); //push back index of new movement
            cluster_space.push_back(new_Cluster);
        }



    }


    void save(std::string file_path){

        std::ofstream outfile;
        outfile.open(file_path.c_str());

        for (unsigned int i=0;i<movement_space.size();i++){//each movement in movement_space

            for (unsigned int j=0;j<movement_space[i].size();j++){//each posture in one movement

                for (unsigned int k=0;k<movement_space[i][j].size();k++){
                    outfile << movement_space[i][j][k];
                    if (k!=movement_space[i][j].size()-1){
                        outfile<<",";
                    }

                }
                outfile<<"\n";
            }
            outfile<<"####\n";
        }


        outfile.close();

    }

    void load(std::string file_path){
        movement_space.clear();

        std::ifstream infile;
        infile.open(file_path.c_str());
        std::cout<<"loading conceptual space"<<std::endl;

        std::vector<std::vector<int> >  new_movement;
        std::string line;
        while (std::getline(infile, line)){ //Line by Line

            if (line=="####"){
                movement_space.push_back(new_movement);
                new_movement.clear();
                continue;
            }

            std::vector<int> new_posture;
            std::string segment;
            std::stringstream ss(line);

            while(std::getline(ss, segment, ',')){ //seperating each line
                new_posture.push_back(atoi(segment.c_str()));

            }
            new_movement.push_back(new_posture);
        //std::cout<<".";
        }
        std::cout<<movement_space.size()<<std::endl;
    }



    float compare_postures(std::vector<int>v1,std::vector<int>v2){

//        std::cout<<"p1:"<<p1.size()<<" p2:"<<p2.size()<<std::endl;

        if (v1.size()==0 || v2.size()==0){
            std::cout<<"Vector empty---no comparison possible"<<std::endl;
        }

        std::vector<int> p1;
        std::vector<int> p2;

        if (v1.size()<=v2.size()){
            p1=v1;
            p2=v2;
        }
        else{
            p1=v2;
            p2=v1;
        }

        float out=0;

        for (unsigned int i=0; i<p1.size();i++){
            out+=pow(p1[i]-p2[i],2.0);
//            std::cout<<"("<<p1[i]<<"-"<<p2[i]<<")^2=";
//            std::cout<<pow(p1[i]-p2[i],2)<<std::endl;
        }

//        if (out==0){
//            std::cout<<"compare postures=0"<<std::endl;
//            std::cout<<p1.size()<<" "<<p2.size()<<std::endl;

//        }

//        if (p1.size()!=p2.size()){
//            std::cout<<"Scheisse!!!! p1"<<p1.size()<<"p2:"<<p2.size()<<std::endl;

//        }
//        else{
//            std::cout<<p1.size()<<"p2:"<<p2.size()<<std::endl;
//        }

        //std::cout<<"==="<<sqrt(out)<<std::endl;
        return sqrt(out);

    }





    std::vector<float> integrate_movement(){

        //integrates the current movement in the cp
        //compares the movements postures to the postures of the previously
        //stored movements and decides which one is closest
        //then stores it

        float min_dist=999999.0;
        int min_index=999999; //index of closest movement

        std::vector<float> distances_to_others;


        std::vector<std::vector<int> > short_mov;
        std::vector<std::vector<int> > long_mov;
        //loop over all movements in space
        for (unsigned int v=0;v<movement_space.size();v++){
            //compare the shorter to the longer one
            if (a_movement.size()<=movement_space[v].size()){
                short_mov=a_movement;
                long_mov = movement_space[v];
            }
            else{
                long_mov=a_movement;
                short_mov = movement_space[v];
            }


            float this_min_dist=999999.0; //minimal distance only to this compared movement
            for(unsigned int s=0; s<long_mov.size()-short_mov.size()+1;s++){ //start position for comparing in long pattern
                //float distance =0;
                std::vector<float> dis_vec;
                for (unsigned int w=0; w< short_mov.size();w++){
//                    std::cout<<s<<","<<w<<std::endl;
//                    std::cout<<short_mov[w].size()<<","<<long_mov[s+w].size()<<std::endl;
                    //std::cout<<"before adding "<<distance<<std::endl;
                    //distance+=compare_postures(short_mov[w],long_mov[s+w]);
                    dis_vec.push_back(compare_postures(short_mov[w],long_mov[s+w]));
                    //std::cout<<"inner loop "<<distance<<std::endl;
                }
//                std::cout<<"after inner loop "<<distance<<std::endl;
//                std::cout<<"after inner loop vec "<<dis_vec.back()<<std::endl;
                float sum_of_elems =std::accumulate(dis_vec.begin(),dis_vec.end(),0)/float(short_mov.size());
                this_min_dist=std::min(this_min_dist,sum_of_elems);

            }
            if (this_min_dist<min_dist){
                min_dist=this_min_dist;
                min_index=v;
            }

            std::cout<<"min distance to "<<v<<" "<<this_min_dist<<std::endl;
            distances_to_others.push_back(this_min_dist);

        }
        movement_space.push_back(a_movement);
        std::cout<<"Integrated movement: "<<std::endl;
        std::cout<<"closest stored movement was index "<<min_index<<" ("<<min_dist<<")"<<std::endl;

        if (!cluster_space.empty()){
            float min_clus_dist=99999;
            int cluster_index=999;
            for (unsigned int c =0; c<cluster_space.size();c++){
                float this_clus_dist=mov_distance(a_movement,cluster_space[c].centroid);
                if (this_clus_dist<min_clus_dist){
                    min_clus_dist=this_clus_dist;
                    cluster_index = c;
                }
            }
            std::cout<<"Closest Cluster is "<<cluster_index<<std::endl;
        }

        a_movement.clear();
        //cluster_new(1250.0,distances_to_others);
        return distances_to_others;
    }


    std::vector <std::vector< std::vector <int > > > movement_space;
    std::vector < std::vector<int> > a_movement;
    std::vector<int> a_posture;
    bool processing; // true= movement is fed to conceptual space
    std::vector<Cluster> cluster_space;

protected:





};

#endif // CONCEPTUAL_SPACE_H
