/*
  HydroCpp
  Repository: https://github.com/akira215/HydroCpp
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
                   : m_vertices(vertexVect), m_isComputed(false), 
                     m_area(0.0), m_cog(HCPoint(0.0,0.0))
{ 
    // Orient the polygon in a safe manner
    setCounterclockwise();

}


HCPolygon::~HCPolygon() = default;
        

HCPolygon::HCPolygon(const HCPolygon& other)
            : m_vertices(other.m_vertices),
            m_isComputed(other.m_isComputed),
            m_area(other.m_area),
            m_cog(other.m_cog)
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

double HCPolygon::getArea()
{
    if(!m_isComputed)
        compute();
    
    return m_area;
}

const HCPoint& HCPolygon::getCog()
{
    if(!m_isComputed)
        compute();
    
    return m_cog;
}

/////////////////////////////////////////////
//
// Private
//
//////////////////////////////////////////////

void HCPolygon::compute()
{
    triangulatePolygon();
    m_area = 0.0;
    double Mx = 0.0; 
    double My = 0.0;
    for (auto& tr : m_trianglesList){
        double xcog_tr = (tr.P0.x +  tr.P1.x +  tr.P2.x) / 3.0;
        double ycog_tr = (tr.P0.y +  tr.P1.y +  tr.P2.y) / 3.0;

        double area_tr = 0.5 *(tr.P0.x * (tr.P1.y - tr.P2.y) +
                              tr.P1.x * (tr.P2.y - tr.P0.y) +
                              tr.P2.x * (tr.P0.y - tr.P1.y));
        Mx += xcog_tr *area_tr;
        My += ycog_tr *area_tr;
        
        m_area += area_tr;
    }
    //m_area = abs(m_area);
    if (m_area > 0){
        m_cog.x = Mx / m_area;
        m_cog.y = My / m_area;
    } else {
        m_cog.x = 0.0;
        m_cog.y = 0.0;
    }

    m_isComputed = true;
}

size_t HCPolygon::mostLeftVertex(const std::vector<HCPoint>& polygon)
{
    double xc = polygon[0].x;
    size_t j =0;
    for (size_t i = 1; i < polygon.size();++i )
        if (polygon[i].x < xc) {
            xc = polygon[i].x;
            j = i;
        }
    return j;
}

size_t HCPolygon::farthestVertex(const std::vector<HCPoint>& polygon,
                            HCTriangle triangle,
                            std::array<size_t, 3> indices)
{
    //n = len(polygon)
    double distance = 0.0;
    size_t j = SIZE_MAX;
    for (size_t i = 0; i < polygon.size(); ++i){
        auto it = std::find(std::begin(indices), std::end(indices), i);
        if (it == std::end(indices)){
            HCPoint M = polygon[i];
            if (isInTriangle(triangle ,M)){
                double d = abs(distPtToSegment(std::make_pair(triangle.P1, triangle.P2),M));
                if (d > distance){
                    distance = d;
                    j = i;
                }
            }
               
        }
    }

    return j;
}

bool HCPolygon::isInTriangle(HCTriangle triangle, HCPoint& M)
{
    HCPoint& P0 = triangle.P0;
    HCPoint& P1 = triangle.P1;
    HCPoint& P2 = triangle.P2;

    return (distPtToSegment(std::make_pair(P0,P1),M) > 0)
        && (distPtToSegment(std::make_pair(P1,P2),M) > 0)
        && (distPtToSegment(std::make_pair(P2,P0),M) > 0);
}

std::vector<HCPoint> HCPolygon::newPolygon(std::vector<HCPoint> polygon,
                                             size_t start, size_t end)
{
    std::vector<HCPoint> p;
    size_t i = start;
    while (i != end){
        p.push_back(polygon[i]);
        i = ( i + 1 > polygon.size() -1 ) ? 0 : i+1;
    }
    p.push_back(polygon[end]);
    return p;
}


void HCPolygon::triangulatePolygonRecursive(std::vector<HCPoint> polygon) 
                                        //std::vector<HCTriangle> trianglesList)
{
    size_t j0 = mostLeftVertex(polygon);
    size_t j1 = ((j0 + 1) > (polygon.size() - 1)) ? 0 : j0 + 1;
    size_t j2 = (j0 == 0) ? polygon.size() - 1 : j0 - 1;
    
    HCPoint& P0 = polygon[j0];
    HCPoint& P1 = polygon[j1];
    HCPoint& P2 = polygon[j2];

    size_t j = farthestVertex(polygon, {P0,P1,P2} ,{j0,j1,j2});
    if (j == SIZE_MAX){
        m_trianglesList.push_back({P0,P1,P2});
        std::vector<HCPoint> poly1 = newPolygon(polygon,j1,j2);
        if (poly1.size() == 3)
            m_trianglesList.push_back({poly1[0],poly1[1],poly1[2]});
        else if (poly1.size() == 2) // the polygon was a triangle not more to do
            return;
        else
            triangulatePolygonRecursive(poly1);
    } else {
        auto poly1 = newPolygon(polygon, j0,j);
        auto poly2 = newPolygon(polygon, j,j0);
        
        if (poly1.size() == 3)
            m_trianglesList.push_back({poly1[0],poly1[1],poly1[2]});
        else
            triangulatePolygonRecursive(poly1);
        
        if (poly2.size() == 3)
            m_trianglesList.push_back({poly2[0],poly2[1],poly2[2]});
        else
            triangulatePolygonRecursive(poly2);
    }
}

void HCPolygon::triangulatePolygon()
{
    m_trianglesList.clear();

    triangulatePolygonRecursive(m_vertices);
}

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