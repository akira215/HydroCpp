/*
  HydroCpp
  Repository: https://github.com/btzy/nativefiledialog-extended
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

#pragma once
// ===== External Includes ===== //
#include <string>
#include <vector>
#include <map>
#include <limits>
// ===== HydroCpp Includes ===== //
#include "HCPoint.hpp"
#include "HCPolygon.hpp"



namespace HydroCpp
{

     /**
     * @brief struct to hold min max value of the hull
     */
    struct MinMax
    {
        double xmin;
        double xmax;
        double ymin;
        double ymax;
    };

    /**
     * @brief struct to hold hydrostatic data
     */
    struct Hydrodata
    {
        double RMT;
        double RML;
        double Lpp;
        double LCF;
        double MCT;
        double LCB;
        double TCB;
        double VCB;
        double KMT;
        double Waterline;
        double Volume;
        double Displacement;
        double WaterplaneArea;
        double Immersion;
    };


    class HCLoader
    {
    public:
        /**
         * @brief constructor
         * @param filename of the excel filename containing the data
         * @param loc
         */
        HCLoader(const std::string& filename);

        /**
         * @brief destructor
         */
        ~HCLoader();

         /**
         * @brief Compute Volume, LCB, VCB for each waterline step.Feed the member variable
         */
        void computeHydroTable();

    private:

         /**
         * @brief read data from name range in workbook
         * @param wb the excel workbook
         * @param rngName the range name
         * @param defautVal which will be returned if named range doesn't exist
         */
        double getValueFromRange(const OpenXLSX::XLWorkbook& wb,
                                const std::string& rngName, 
                                double defautVal );
        
        /**
         * @brief check that the xmin xmax ymin ymax of the section
         * and ajust the corresponding member variable (struct MinMax)
         */
        void checkMinMax(const std::vector<HCPoint>& section );

        /**
         * @brief compute the hydrodata for a given waterline
         */
        void computeHydrotable(const std::pair<HCPoint,HCPoint>& waterline );
    private:
        std::string                 m_filename;
        std::map<double,HCPolygon*>  m_hull;
        std::vector<Hydrodata*>     m_hydroTable;
        MinMax                      m_minMax {
                    std::numeric_limits<double>::max(), //xmin
                    std::numeric_limits<double>::min(), //xmax
                    std::numeric_limits<double>::max(), //ymin
                    std::numeric_limits<double>::min()  //ymax
                    };
        double                      m_maxWl;
        double                      m_deltaWl;
        double                      m_maxAngle;
        double                      m_deltaAngle;
        double                      m_d_sw;


    };

}  // namespace std