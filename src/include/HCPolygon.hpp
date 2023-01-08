/*
  HydroCpp
  Repository: https://github.com/btzy/nativefiledialog-extended
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/
#pragma once

// ===== External Includes ===== //
#include <vector>
// ===== HydroCpp Includes ===== //
#include "HCPoint.hpp"


namespace HydroCpp
{
    class HCVertex 
    {
        public:
            HCPoint     vertex;     // Vertex
            HCPoint*    next;       // next vertex of the poly
            HCPoint*    prev;       // prev vertex of the poly
        
        public:
            HCVertex(const HCPoint& vertex) :
                vertex(vertex),
                next(nullptr),
                prev(nullptr)
            {}

            ~HCVertex() = default;
    };

    class HCPolygon 
    {
        friend class HCPolygonSplitter;
        
    public:
        /**
         * @brief constructor
         * @param x
         * @param y
         */
        HCPolygon(const std::vector<HCPoint>& vertexVect);

        /**
         * @brief
         */
        ~HCPolygon();
        
        /**
         * @brief Copy constructor
         * @param other The object to be copied.
         */
        HCPolygon(const HCPolygon& other);

        /**
         * @brief Move constructor
         * @param other The XLCell object to be moved
         */
        HCPolygon(HCPolygon&& other);

        /**
         * @brief Copy assignment operator
         * @param other The object to be copy assigned
         * @return A reference to the new object
         */
        HCPolygon& operator=(const HCPolygon& other);

        /**
         * @brief Move assignment operator [deleted]
         * @param other The object to be move assigned
         * @return A reference to the new object
         */
        HCPolygon& operator=(HCPolygon&& other);

        /**
         * @brief return const ref of the vertices
         * @return A reference to the mertices
         */
        const std::vector<HCPoint>& getVertices() const;

    private:

        /**
         * @brief check the orientation of the polygon
         * @return Return true if counterclockwise false otherwise
         */
        bool isCounterclockwise() const;

        /**
         * @brief arrange the polygon vertices conterclockwise
         *  for safe triangulation operations
         */
        void setCounterclockwise();

         /**
         * @brief check the circular next vertex
         * @param index current index
         * @param step of the offset
         * @return Return the corresponding vertex
         */
        uint16_t next(uint16_t index, int step) const;



    protected:
        std::vector<HCPoint> m_vertices;

    };

    // ========== FRIEND FUNCTION IMPLEMENTATIONS ========== //
    
    
    /**
      * @brief Compute Lz from Point M relative to segment P0,P1
      * @param line 
      * @param M 
      * @return the distance from point to linz
      * @note This allow to know from which side from the segment is M and 
      * compare distances of several points to this segment
      */
    inline double distPtToSegment(const std::pair<HCPoint,HCPoint>& line, 
                            const HCPoint& M)
    {
        const HCPoint& P0 = line.first;
        const HCPoint& P1 = line.second;
        
        return (P1.x-P0.x)*(M.y-P0.y)-(P1.y-P0.y)*(M.x-P0.x);
    }

}  // namespace std