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
#include "HCPolygon.hpp"


namespace HydroCpp
{


    class HCPolygons 
    {
        friend class HCPolygonSplitter;

    public:
        /**
         * @brief constructor
         */
        HCPolygons();

        /**
         * @brief constructor
         * @param poys vector of polygons
         */
        HCPolygons(const std::vector<HCPolygon>& polys);

        /**
         * @brief
         */
        ~HCPolygons();
        
        /**
         * @brief Copy constructor
         * @param other The object to be copied.
         */
        HCPolygons(const HCPolygons& other);

        /**
         * @brief Move constructor
         * @param other The XLCell object to be moved
         */
        HCPolygons(HCPolygons&& other);

        /**
         * @brief Copy assignment operator
         * @param other The object to be copy assigned
         * @return A reference to the new object
         */
        HCPolygons& operator=(const HCPolygons& other);

        /**
         * @brief Move assignment operator [deleted]
         * @param other The object to be move assigned
         * @return A reference to the new object
         */
        HCPolygons& operator=(HCPolygons&& other);

        /**
         * @brief return const ref of the polygons
         * @return A reference to the mertices
         */
        const std::vector<HCPolygon>& getPolygons() const;

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

        /**
         * @brief clear the content of the vector
         * @note the status is changed to not computed
         */
        void clear();
    
    protected:

        /**
         * @brief return const ref of the polygons
         * @return A reference to the mertices
         */
        std::vector<HCPolygon>& getPolygons();

    private:
        /**
         * @brief The computation loop
         * @note should be called before accessing data
         */
        void compute();

    private:
        bool                    m_isComputed;
        std::vector<HCPolygon>  m_polys;
        double                  m_area;
        HCPoint                 m_cog;

    };

}  // namespace std