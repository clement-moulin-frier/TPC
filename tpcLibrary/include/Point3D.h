#ifndef POINT3D_H
#define POINT3D_H

#include <cmath>

struct Point3D{//3D point or vector representation
    double x;
    double y;
    double z;

    Point3D(){}

    Point3D(int a, int b, int c){
        x=a;
        y=b;
        z=c;

    }

    Point3D(float a, float b, float c){
        x=a;
        y=b;
        z=c;

    }




    Point3D(Point3D a, Point3D b){
        x= a.x-b.x;
        y= a.y-b.y;
        z = a.z-b.z;
    }

    float length() const {
        return (sqrt(pow(x,2)+pow(y,2)+pow(z,2)));

    }

    bool operator<( const Point3D &p ) const {
        return this->length() < p.length();
    }

};
#endif //POINT3D_H
