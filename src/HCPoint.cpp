/*
  HydroCpp
  Repository: https://github.com/akira215/HydroCpp
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

// ===== Standards Includes ===== //
#include <algorithm>
#include <math.h>

// ===== External Includes ===== //

// ===== HydroCpp Includes ===== //
#include "HCPoint.hpp"

using namespace HydroCpp;



HCPoint::HCPoint(double x, double y):x(x),y(y)
{ }


HCPoint::~HCPoint() = default;
        

HCPoint::HCPoint(const HCPoint& other):x(other.x), y(other.y)
{ }

HCPoint::HCPoint(HCPoint&& other) = default;


HCPoint& HCPoint::operator=(const HCPoint& other)
{
    if (&other != this) {
            auto temp = HCPoint(other);
            std::swap(*this, temp);
        }
        return *this;
}


HCPoint& HCPoint::operator=(HCPoint&& other) 
{
    if (&other != this) {
        x = std::move(other.x);
        y = std::move(other.y);
    }
    return *this;
}


HCPoint& HCPoint::operator+(const HCPoint& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}


HCPoint& HCPoint::operator-(const HCPoint& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

bool HCPoint::operator==(const HCPoint& other) const
{
    if((x == other.x)&&(y == other.y))
        return true;
    return false;
}

bool HCPoint::operator!=(const HCPoint& other) const
{
    return !(*this == other);
}

double HCPoint::operator*(const HCPoint& rhs) const
{
    return x * rhs.x + y * rhs.y;
}

HCPoint& HCPoint::operator*(const double m)
{
    x *= m;
    y *= m;

    return *this;
}


double HCPoint::distanceToOrigin() const
{
    HCPoint origin = HCPoint(0.0, 0.0);
    return distanceTo(origin);
}


double HCPoint::distanceTo(const HCPoint& rhs) const
{
    return (sqrt(pow(x-rhs.x , 2) + pow(y-rhs.y , 2)));
}