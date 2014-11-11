#ifndef VISUALIZE_H
#define VISUALIZE_H

#include "tpcmatrix.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui_c.h"

using namespace cv;




class Visualizer{

public:

	Visualizer();

    Visualizer(TpcMatrix &tpcmatrix){

//    integration_vis_big=cvCreateImage( cvSize((int)((integration_vis->width*700)/100) , (int)((integration_vis->height*700)/100) ),integration_vis->depth, integration_vis->nChannels );
    cout<<"heyo1"<<endl;
    visualization=cvCreateImage(cvSize(tpcmatrix.M_X,(tpcmatrix.M_Y)*(tpcmatrix.M_Z)),IPL_DEPTH_8U,3);
    cout<<"heyo2"<<endl;
    visualization_big = cvCreateImage( cvSize((int)((visualization->width*700)/100) , (int)((visualization->height*700)/100) ),visualization->depth, visualization->nChannels );


    cvNamedWindow("visualization",CV_WINDOW_AUTOSIZE);
    cvMoveWindow("visualization", 100, 100);
//    cvNamedWindow("integration_vis",CV_WINDOW_AUTOSIZE);
//    cvMoveWindow("integration_vis", 510, 100);


    }


    void visualize (TpcMatrix &tpcmatrix){
        //Create visualization
        int vis_y,vis_x;

        for (int i=0;i<tpcmatrix.M_X;i++){
            for (int j=0;j<tpcmatrix.M_Y;j++){
                for (int k=0;k<tpcmatrix.M_Z;k++){
                    vis_x=i;
                    vis_y=j+k*(tpcmatrix.M_Y);
                    if(tpcmatrix.spike_matrix[i][j][k]){

                        visualization->imageData[vis_y*visualization->widthStep+3*vis_x+0]=(uchar)0;//b
                        visualization->imageData[vis_y*visualization->widthStep+3*vis_x+1]=(uchar)0;//g
                        visualization->imageData[vis_y*visualization->widthStep+3*vis_x+2]=(uchar)255;//r
                    }

                    else{
                        visualization->imageData[vis_y*visualization->widthStep+3*vis_x+0]=(uchar)0;//r
                        visualization->imageData[vis_y*visualization->widthStep+3*vis_x+1]=(uchar)0;//g
                        visualization->imageData[vis_y*visualization->widthStep+3*vis_x+2]=(uchar)0;//b

                    }


                }
            }

        }


        for (unsigned int i=0;i<tpcmatrix.stimulated.size();i++){
            vis_x=tpcmatrix.stimulated[i].x;
            vis_y=tpcmatrix.stimulated[i].y+tpcmatrix.stimulated[i].z*tpcmatrix.M_Y;

            visualization->imageData[vis_y*visualization->widthStep+3*vis_x+0]=(uchar)255;//b
            visualization->imageData[vis_y*visualization->widthStep+3*vis_x+1]=(uchar)0;//g
            visualization->imageData[vis_y*visualization->widthStep+3*vis_x+2]=(uchar)0;//r


        }

    //    if (tpcmatrix.isVector()){

    //        for (int i=0;i<tpcmatrix.M_X;i++){
    //            for (int j=0;j<tpcmatrix.M_Y;j++){
    //                vis_x=i;
    //                vis_y=j;

    //                if( ( (TpcMatrixVector*) tpcmatrix)->spike_integration_field[i][j]){

    //                    integration_vis->imageData[vis_y*integration_vis->widthStep+3*vis_x+0]=(uchar)0;//b
    //                    integration_vis->imageData[vis_y*integration_vis->widthStep+3*vis_x+1]=(uchar)0;//g
    //                    integration_vis->imageData[vis_y*integration_vis->widthStep+3*vis_x+2]=(uchar)255;//r
    //                }

    //                else{
    //                    integration_vis->imageData[vis_y*integration_vis->widthStep+3*vis_x+0]=(uchar)0;//r
    //                    integration_vis->imageData[vis_y*integration_vis->widthStep+3*vis_x+1]=(uchar)0;//g
    //                    integration_vis->imageData[vis_y*integration_vis->widthStep+3*vis_x+2]=(uchar)0;//b

    //                }


    //            }

    //        }

    //    }


        cvResize(visualization, visualization_big);
        cvShowImage("visualization",visualization_big);
//        if (tpcmatrix.isVector()){
//            cvResize(integration_vis, integration_vis_big);
//            cvShowImage("integration_vis",integration_vis_big);
//        }


        cvWaitKey(1);


    }


    IplImage* visualization;
    IplImage* visualization_big;
//    IplImage* integration_vis;
//    IplImage* integration_vis_big;

    protected:

};
#endif // VISUALIZE_H
