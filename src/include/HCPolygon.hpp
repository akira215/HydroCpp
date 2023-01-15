/*
  HydroCpp
  Repository: https://github.com/btzy/nativefiledialog-extended
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/
#pragma once

// ===== External Includes ===== //
#include <vector>
#include <array>
// ===== HydroCpp Includes ===== //
#include "HCPoint.hpp"


namespace HydroCpp
{
    struct HCTriangle 
    {
        HCPoint    P0;     
        HCPoint    P1;     
        HCPoint    P2;   
    };

    class HCPolygon 
    {

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

        /**
         * @brief get the area of the polygon
         * @return the area
         */
        double getArea();

        /**
         * @brief get the cog of the polygon
         * @return a ref to the Cog HCPoint
         */
        const HCPoint& getCog();

    private:

        /**
         * @brief Compute the area of the triangulated polygone
         * @note shall be called before grabbing data
         */
        void compute();
        
        /**
         * @brief Search triangles and return the triangle list
         */
        void triangulatePolygon();

        /**
         * @brief Recursive function to divide polygone in 2 parts, 
         * starting by the most left vertex call recursively new polygones 
         * until getting triangles, and fill the triangle list
         */
       void triangulatePolygonRecursive(std::vector<HCPoint> polygon); 
                                   // std::vector<HCTriangle> trianglesList);

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

        /**
         * @brief Return the indice of the vertex the most on the left of the polygon.
         * If more than one vertice have the same abscisse, any of them could be return
         * @param polygon the polygon
         * @return Return the index of the corresponding vertex
         */
        static size_t mostLeftVertex(const std::vector<HCPoint>& polygon);

        /**
         * @brief Return the indice of the polygon vertex from triangle P0,P1,P2 
         * which is the farthest from segment P1,P2
         * @param polygon the polygon
         * @param 3points of the triangle
         * @param indies indices of the vertex of the selected triangle
         * @return Return the index of the corresponding vertex
         */
        static size_t farthestVertex(const std::vector<HCPoint>& polygon,
                            HCTriangle triangle,
                            std::array<size_t, 3> indices);

        /**
         * @brief check if the point is striclty inside the triangle
         * @param triangle an array of point
         * @param M point to be checked
         * @return Return True if M is striclty inside the triangle
         * @note Triangle points shall be given counterclockwise
         */
        static inline bool isInTriangle(HCTriangle triangle, HCPoint& M);

         /**
         * @brief Generate a polygone from indice start to end, 
         * considering the cyclic condition
         * @param polygon an array of indices
         * @param start
         * @param start
         * @return Return a vector of point
         */
        std::vector<HCPoint> newPolygon( std::vector<HCPoint> polygon, 
                                        size_t start, size_t end);

    private:
        std::vector<HCPoint>    m_vertices;
        bool                    m_isComputed;
        std::vector<HCTriangle> m_trianglesList;
        double                  m_area;
        HCPoint                 m_cog;

    };

    // ========== FRIEND FUNCTION IMPLEMENTATIONS ========== //
    
    
    /**
      * @brief Compute Lz from Point M relative to segment P0,P1
      * @param line 
      * @param M 
      * @return the distance from point to line
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