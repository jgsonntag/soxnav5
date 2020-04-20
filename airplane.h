#ifndef AIRPLANE_H
#define AIRPLANE_H

#include "utility.h"
#include "constants.h"


class airplane
{

  public:
    airplane();
    void setState(double nlat, double nlon,double nhell,
                  double ncrs,double speed,double nroc,
                  int proj,double lon0);
    double getx();
    double gety();
    double gethell();
    double getcrs();
    double getspeed();
    double getroc();

  private:
    double lat;
    double lon;
    double hell;
    double crs;
    double speed;
    double roc;
    double nx;
    double ny;
    double x;
    double y;
    int zone;

};

#endif // AIRPLANE_H
