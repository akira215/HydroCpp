/*
  HydroCpp
  Repository: https://github.com/btzy/nativefiledialog-extended
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/
#pragma once

// ===== External Includes ===== //
#include <vector>
#include <list>
// ===== HydroCpp Includes ===== //
#include "HCPolygon.hpp"
#include "HCPolygons.hpp"

namespace HydroCpp
{
    enum class LineSide
    {
        On,
        Left,
        Right,
        Undef
    };

    class Vertex
    {
    public:
        HCPoint     pt          { 0.0, 0.0 };
        LineSide    side        { LineSide::Undef};
        Vertex*     next        { nullptr };
        Vertex*     prev        { nullptr };
        double      distToStart { 0.0 }; 
        bool        isSrc       { false };
        bool        isDest      { false };
        bool        visited     { false };
    public:
        Vertex(const HCPoint& pt, LineSide side) : pt(pt), side(side)
        {}

        ~Vertex() = default;
    };

    class HCPolygonSplitter 
    {
         //---------- Friend Declarations ----------//

        friend double distPtToSegment(const std::pair<HCPoint,HCPoint>& line, 
                    const HCPoint& M);

    public:
        /**
         * @brief constructor
         * @param vertices
         * @param line
         * @note initialize the double linked list of vertices, including intersections points
         */
        HCPolygonSplitter(const std::vector<HCPoint>& vertices,
                            const std::pair<HCPoint,HCPoint>& line );

        /**
         * @brief
         */
        ~HCPolygonSplitter();
        
        /**
         * @brief Copy constructor
         * @param other The object to be copied.
         */
        HCPolygonSplitter(const HCPolygonSplitter& other);

        /**
         * @brief Move constructor
         * @param other The XLCell object to be moved
         */
        HCPolygonSplitter(HCPolygonSplitter&& other);

        /**
         * @brief Copy assignment operator
         * @param other The object to be copy assigned
         * @return A reference to the new object
         */
        HCPolygonSplitter& operator=(const HCPolygonSplitter& other);

        /**
         * @brief Move assignment operator [deleted]
         * @param other The object to be move assigned
         * @return A reference to the new object
         */
        HCPolygonSplitter& operator=(HCPolygonSplitter&& other);


        /**
         * @brief split the polygon in 2 part 
         * @param side The object to be move assigned
         * @return the splitted polygon on the requested side of the line
         */
        HCPolygons& getPolygonFromSide(LineSide side);

        /**
         * @brief split the polygon in 2 part and get the intersected segments
         * @return a vector of the intersected segments
         */
        const std::vector<std::pair<HCPoint,HCPoint>>& getEdges();
    private:

         /**
         * @brief trigger the computation
         * @note this shall be call prior to grabbing any data
         */
        void computeIntersections();

        /**
         * @brief check the orientation of the polygon
         * @return Return true if counterclockwise false otherwise
         */
        void sortIntersections();

        /**
         * @brief Split polygon creating new edges
         */
        void splitPolygon();
        
        /**
         * @brief collect the splitted polygon
         */
        void collectPolys();

        /**
         * @brief creates the new edges along the split line
         */
        void createBridge(Vertex* srcPt, Vertex* dstPt);

        /**
         * @brief scalar projection on line. in case of co-linear
         * vectors this is equal to the signed distance.
         * @param M point 
         * @return Return the signed distance
         * @note it's important to take the signed distance here,
         * because it can happen that the split line starts/ends
         * inside the polygon. in that case intersection points
         * can fall on both sides of the split line and taking
         * an unsigned distance metric will result in wrongly
         * ordered points in EdgesOnLine.
         */
        double distanceFromStart(const HCPoint& M) const;

        /**
         * @brief return the side of the point from oriented segment
         * @param M
         * @return LineSide enum
         * @note could be static
         */
        LineSide getSide( const HCPoint& M) const;
        
        /**
         * @brief Compute the intersection between the line and the segment
         * @param line
         * @param start the start point of the segment
         * @param end the end point of the segment
         * @return The intersection point
         * @note could be static
         */
        HCPoint intersection(const std::pair<HCPoint,HCPoint>& line, 
                            const HCPoint& A, const HCPoint& B) const;

    private:
        std::list<Vertex>       m_vertices;       // polygons vertices
        std::vector<Vertex*>    m_intersections; // pointers of vertex along the line
        std::vector<std::pair<HCPoint,HCPoint>> m_edges; //  segments along the line
        const std::pair<HCPoint,HCPoint>& m_line;
        std::vector<HCPolygon>  m_result;
        HCPolygons              m_polys;
        bool                    m_isComputed;

    };

}  // namespace std