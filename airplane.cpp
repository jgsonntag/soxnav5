#include "airplane.h"

airplane::airplane()
{

}



double airplane::getx()
{
    return(x);
}



double airplane::gety()
{
    return(y);
}



double airplane::gethell()
{
    return(hell);
}



double airplane::getcrs()
{
    return(crs);
}



double airplane::getspeed()
{
    return(speed);
}



double airplane::getroc()
{
    return(roc);
}



void airplane::setState(double nlat, double nlon,double nhell,
                        double ncrs,double nspeed,double nroc,
                        int proj,double lon0)
{

    // Set lat, lon and the various heights
    lat = nlat;
    lon = nlat;
    hell = nhell;

    // Set course speed and roc
    crs = ncrs;
    speed = nspeed;
    roc = nroc;

    // Compute and set x and y
    if (proj==0)
    {
        geod2ps(nlat/DEG2RAD,nlon/DEG2RAD,
                70.0,315.0,1.0,AE,FLAT,&nx,&ny);
    }
    else if (proj==1)
    {
        geod2utm(nlat/DEG2RAD,nlon/DEG2RAD,
                 lon0,&ny,&nx,&zone);
    }
    else
    {
        geod2ps(nlat/DEG2RAD,nlon/DEG2RAD,
                -71.0,0.0,1.0,AE,FLAT,&nx,&ny);
    }
    x = nx;
    y = ny;

    //qDebug() << x << y;

}

