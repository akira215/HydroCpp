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
// ===== HydroCpp Includes ===== //
#include "HCPoint.hpp"
#include "HCPolygon.hpp"



namespace HydroCpp
{
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
         * @brief
         */
        ~HCLoader();
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
    private:
        std::string                 m_filename;
        std::map<double,HCPolygon*>  m_hull;
        double                      m_maxWl;
        double                      m_deltaxWl;
        double                      m_maxAngle;
        double                      m_deltaAngle;
        double                      m_d_sw;

    };

}  // namespace std