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
#include <cassert>     /* assert */

// ===== External Includes ===== //

// ===== HydroCpp Includes ===== //
#include "HCPolygonSplitter.hpp"

using namespace HydroCpp;

HCPolygonSplitter::HCPolygonSplitter(const std::vector<HCPoint>& vertices, 
                                    const std::pair<HCPoint,HCPoint>& line) 
        : m_vertices(), m_line(line), m_isComputed(false)
{
    m_intersections.clear();
    for(uint16_t i=0; i < vertices.size(); ++i) {
        HCPoint startPt = vertices.at(i);
        HCPoint endPt = vertices.at(i<vertices.size() - 1 ? i+1 : 0);

        auto startSide = getSide(startPt); 
        auto endSide = getSide(endPt);

        m_vertices.push_back(Vertex(startPt, startSide));
        if (startSide == LineSide::On)
        {   // vertex on line
            m_intersections.push_back(&m_vertices.back());
        }
        else if (startSide != endSide && endSide != LineSide::On)
        {  // segment crossing the line
            auto interPt = intersection (line, startPt, endPt);
            m_vertices.push_back(Vertex(interPt, LineSide::On));
            m_intersections.push_back(&m_vertices.back());
        }

    }
    
    // connect doubly linked list, except
    // first->prev and last->next
    for (auto it = m_vertices.begin(); it!=std::prev(m_vertices.end()); it++)
    {
        auto nextIt = std::next(it);
        it->next = &(*nextIt);
        nextIt->prev = &(*it);
    }

    // connect first->prev and last->next
    m_vertices.back().next = & m_vertices.front();
    m_vertices.front().prev = & m_vertices.back();
       
}

HCPolygonSplitter::HCPolygonSplitter(const HCPolygon* polygon, 
                                    const std::pair<HCPoint,HCPoint>& line) :
    HCPolygonSplitter(polygon->getVertices(), line)
{ }


HCPolygonSplitter::~HCPolygonSplitter() = default;
        

HCPolygonSplitter::HCPolygonSplitter(const HCPolygonSplitter& other)
                : m_vertices(other.m_vertices), 
                m_intersections(other.m_intersections),
                m_line(other.m_line),
                m_isComputed(other.m_isComputed)
{ }

HCPolygonSplitter::HCPolygonSplitter(HCPolygonSplitter&& other) = default;


HCPolygonSplitter& HCPolygonSplitter::operator=(const HCPolygonSplitter& other)
{
    if (&other != this) {
            auto temp = HCPolygonSplitter(other);
            std::swap(*this, temp);
        }
        return *this;
}


HCPolygonSplitter& HCPolygonSplitter::operator=(HCPolygonSplitter&& other) 
{
    if (&other != this) {
        m_vertices = std::move(other.m_vertices);
    }
    return *this;
}

/////////////////////////////////////////////
//
// Methods
//
//////////////////////////////////////////////

HCPolygons& HCPolygonSplitter::getPolygonFromSide(LineSide side)
{
    if (!m_isComputed)
        computeIntersections();
    
    m_polys.clear();

    // Check if each poly is on the right side
    for (auto& p : m_collected){
        if(p.side == side)
            m_polys.getPolygons().push_back(HCPolygon(p.vertices));
    }
    
    return m_polys;

}

const std::vector<std::pair<HCPoint,HCPoint>>& HCPolygonSplitter::getEdges()
{
    if (!m_isComputed)
        computeIntersections();
    
    return m_edges;
}

 void HCPolygonSplitter::computeIntersections()
 {
    sortIntersections();
    splitPolygon();
    collectPolys();
    m_isComputed = true;
 }


void HCPolygonSplitter::sortIntersections()
{
    // sort edges by start position relative to
    // the start position of the split line
    std::sort(m_intersections.begin(), m_intersections.end(), [&](Vertex* e0, Vertex* e1)
    {
        return distanceFromStart(e0->pt) < distanceFromStart(e1->pt);
    });

    // compute distance between each edge's start
    // position and the first edge's start position
    for (size_t i=1; i < m_intersections.size(); i++)
        m_intersections[i]->distToStart = distanceFromStart(m_intersections[i]->pt);
}

void HCPolygonSplitter::splitPolygon()
{
    
    Vertex* useSrc = nullptr;

    for (size_t i=0; i<m_intersections.size(); i++)
    {
        // find source
        Vertex* srcPt = useSrc;
        useSrc = nullptr;

        for (; !srcPt && i<m_intersections.size(); )
        {
            Vertex* curPt =m_intersections[i];
            const auto curSide = curPt->side;
            const auto prevSide = curPt->prev->side;
            const auto nextSide = curPt->next->side;
            assert(curSide == LineSide::On);

            if ((prevSide == LineSide::Left && nextSide == LineSide::Right) ||
                (prevSide == LineSide::Left && nextSide == LineSide::On && curPt->next->distToStart < curPt->distToStart) ||
                (prevSide == LineSide::On && nextSide == LineSide::Right && curPt->prev->distToStart < curPt->distToStart))
            {
                srcPt = curPt;
                srcPt->isSrc = true;
            }
            else
                i++;
        }
        // We get a src here
        // find destination
        Vertex* dstPt = nullptr;

        for (; !dstPt && i<m_intersections.size(); )
        {
            Vertex* curPt = m_intersections[i];
            const auto curSide = curPt->side;
            const auto prevSide = curPt->prev->side;
            const auto nextSide = curPt->next->side;
            assert(curSide == LineSide::On);

            if ((prevSide == LineSide::Right && nextSide == LineSide::Left)  ||
                (prevSide == LineSide::On && nextSide == LineSide::Left)     ||
                (prevSide == LineSide::Right && nextSide == LineSide::On)    ||
                (prevSide == LineSide::Right && nextSide == LineSide::Right) ||
                (prevSide == LineSide::Left && nextSide == LineSide::Left))
            {
                dstPt = curPt;
                dstPt->isDest = true;
            }
            else
                i++;
        }

        // bridge source and destination
        if (srcPt && dstPt)
        {
            createBridge(srcPt, dstPt);
            m_edges.push_back(std::make_pair(srcPt->pt, dstPt->pt));
            //VerifyCycles();

            // is it a configuration in which a vertex
            // needs to be reused as source vertex?
            if (srcPt->prev->prev->side == LineSide::Left)
            {
                useSrc = srcPt->prev;
                useSrc->isSrc = true;
            }
            else if (dstPt->next->side == LineSide::Right)
            {
                useSrc = dstPt;
                useSrc->isSrc = true;
            }
        }

    } // loop through intersections
}

void HCPolygonSplitter::collectPolys()
{
    m_collected.clear();
    for (auto &e : m_vertices)
    {
        if (!e.visited)
        {
            SplitPoly sPoly;
            auto *curPt = &e;

            do
            {
                curPt->visited = true;
                sPoly.vertices.push_back(curPt->pt);
                if (curPt->side != LineSide::On)
                    sPoly.side = curPt->side;
                curPt = curPt->next;
            }
            while (curPt != &e);

            m_collected.push_back(sPoly);
        }
    }

}

void HCPolygonSplitter::createBridge(Vertex* srcPt, Vertex* dstPt)
{
    m_vertices.push_back(*srcPt);
    Vertex* a =  &m_vertices.back();

    m_vertices.push_back(*dstPt);
    Vertex* b =  &m_vertices.back();

    a->next = dstPt;
    a->prev = srcPt->prev;
    b->next = srcPt;
    b->prev = dstPt->prev;

    srcPt->prev->next = a;
    srcPt->prev = b;
    dstPt->prev->next = b;
    dstPt->prev = a;
}


double HCPolygonSplitter::distanceFromStart(const HCPoint& M) const
{
    return (M.x-m_line.first.x)*(m_line.second.x-m_line.first.x)
            + (M.y-m_line.first.y)*(m_line.second.y-m_line.first.y);
}


LineSide HCPolygonSplitter::getSide( const HCPoint& M) const
{
    double dist = distPtToSegment(m_line, M);

    if (std::abs(dist) < 1e-8)
        return LineSide::On;

    if (dist < 0)
        return LineSide::Right;
    
    return LineSide::Left;
}

HCPoint HCPolygonSplitter::intersection(const std::pair<HCPoint,HCPoint>& line, 
                    const HCPoint& A, const HCPoint& B) const
{
    const HCPoint& C = line.first;
    const HCPoint& D = line.second;

    HCPoint vAB = B; vAB = vAB - A; // vector AB
    HCPoint vDC = D; vDC = vDC - C; // vector DC
    double m, k;

    double div = vAB.x * vDC.y - vAB.y * vDC.x;
    if (div !=0.0){
        m = (vAB.x * A.y - vAB.x * C.y - vAB.y * A.x + vAB.y * C.x ) / div;
        k = (vDC.x * A.y - vDC.x * C.y - vDC.y * A.x + vDC.y * C.x ) / div;
        if ((0.0<=m) && (m<1) && (0<=k) && (k<1)){
            return ( vAB * k + A );
        }
    }

    // No intersection
    return HCPoint(0.0,0.0);
}