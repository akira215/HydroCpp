/*
  HydroCpp
  Repository: https://github.com/btzy/nativefiledialog-extended
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

// ===== Standards Includes ===== //
#include <cstdint>
#include <math.h>
#include <algorithm>
//TODEL
#include <iostream>
// ===== External Includes ===== //

// ===== HydroCpp Includes ===== //
#include "HCPolygon.hpp"

using namespace HydroCpp;

HCPolygon::HCPolygon(const std::vector<HCPoint>& vertexVect)
                   : m_vertices(vertexVect)
{ 
    // Orient the polygon in a safe manner
    setCounterclockwise();

}


HCPolygon::~HCPolygon() = default;
        

HCPolygon::HCPolygon(const HCPolygon& other)
            :m_vertices(other.m_vertices)
{ }

HCPolygon::HCPolygon(HCPolygon&& other) = default;


HCPolygon& HCPolygon::operator=(const HCPolygon& other)
{
    if (&other != this) {
            auto temp = HCPolygon(other);
            std::swap(*this, temp);
        }
        return *this;
}


HCPolygon& HCPolygon::operator=(HCPolygon&& other) 
{
    if (&other != this) {
        m_vertices = std::move(other.m_vertices);
    }
    return *this;
}

const std::vector<HCPoint>& HCPolygon::getVertices() const
{
    return m_vertices;
}

/////////////////////////////////////////////
//
// Private
//
//////////////////////////////////////////////

bool HCPolygon::isCounterclockwise() const
{
    // Find the lowest point the most left side to check angle
    std::vector<int> shortList; // shortlist of the most left vertices
    shortList.push_back(0);
    double xc = m_vertices.at(0).x;

    uint16_t index = 0; // Find the list of the most left
    for (uint16_t i=1; i < m_vertices.size(); ++i){
        if (m_vertices.at(i).x == xc)
            shortList.push_back(i);
        else if (m_vertices.at(i).x < xc) {
            xc = m_vertices.at(i).x;
            shortList.clear();
            index = i;
        }
    } 

    index = shortList.at(0); // find the lower of the most left
    double yc = m_vertices.at(index).y;
    for(uint16_t i : shortList){
        if (m_vertices.at(i).y < yc){
            yc = m_vertices.at(i).y;
            index = i;
        }
    }

    HCPoint A = m_vertices.at(next(index,-1));
    HCPoint B = m_vertices.at(index);
    HCPoint C = m_vertices.at(next(index,+1));

    double res = (B.x - A.x) * (C.y - A.y) - (C.x - A.x) * (B.y - A.y);

    if (res > 0)
        return true;
    
    return false;
}

void HCPolygon::setCounterclockwise()
{
    if(isCounterclockwise())
        return;
    
    std::reverse(m_vertices.begin(), m_vertices.end());
}

 uint16_t HCPolygon::next(uint16_t index, int step) const
 {
    int mod = ((int)index + step)%(int)m_vertices.size();
    if(mod<0)
        mod += m_vertices.size();
    return mod;
 }