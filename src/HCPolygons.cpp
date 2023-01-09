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

// ===== External Includes ===== //

// ===== HydroCpp Includes ===== //
#include "HCPolygons.hpp"

using namespace HydroCpp;

HCPolygons::HCPolygons()
                   : m_polys(), m_isComputed(false), 
                     m_area(0.0), m_cog(HCPoint(0.0,0.0))
{ 
}

HCPolygons::HCPolygons(const std::vector<HCPolygon>& polys)
                   : m_polys(polys), m_isComputed(false), 
                     m_area(0.0), m_cog(HCPoint(0.0,0.0))
{ 
}


HCPolygons::~HCPolygons() = default;
        

HCPolygons::HCPolygons(const HCPolygons& other)
            : m_polys(other.m_polys),
            m_isComputed(other.m_isComputed),
            m_area(other.m_area),
            m_cog(other.m_cog)
{ }

HCPolygons::HCPolygons(HCPolygons&& other) = default;


HCPolygons& HCPolygons::operator=(const HCPolygons& other)
{
    if (&other != this) {
            auto temp = HCPolygons(other);
            std::swap(*this, temp);
        }
        return *this;
}


HCPolygons& HCPolygons::operator=(HCPolygons&& other) 
{
    if (&other != this) {
        m_polys = std::move(other.m_polys);
    }
    return *this;
}

const std::vector<HCPolygon>& HCPolygons::getPolygons() const
{
    return m_polys;
}

std::vector<HCPolygon>& HCPolygons::getPolygons()
{
    return m_polys;
}


double HCPolygons::getArea()
{
    if(!m_isComputed)
        compute();
    
    return m_area;
}

const HCPoint& HCPolygons::getCog()
{
    if(!m_isComputed)
        compute();
    
    return m_cog;
}

void HCPolygons::clear()
{
    m_polys.clear();
    m_isComputed = false;
}

/////////////////////////////////////////////
//
// Private
//
//////////////////////////////////////////////

void HCPolygons::compute()
{
    m_area = 0.0;
    m_cog = HCPoint(0.0, 0.0);

    for (auto& p : m_polys){
        double area = p.getArea();
        HCPoint cog = p.getCog();

        m_area += area;
        m_cog = m_cog + (cog * area);
    }

    m_cog.x = m_cog.x /  m_area;
    m_cog.y = m_cog.y /  m_area;
            
    m_isComputed = true;
}
