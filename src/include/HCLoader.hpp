/*
  HydroCpp
  Repository: https://github.com/akira215/HydroCpp
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
        double RMT              {0.0};
        double RML              {0.0};
        double Lpp              {0.0};
        double LCF              {0.0};
        double MCT              {0.0};
        double LCB              {0.0};
        double TCB              {0.0};
        double VCB              {0.0};
        double KMT              {0.0};
        double Waterline        {0.0};
        double Volume           {0.0};
        double Displacement     {0.0};
        double WaterplaneArea   {0.0};
        double Immersion        {0.0};
        bool   isValid          {false};
        bool   submerged        {false};
    };

    struct KNdata
    {
        double angle            {0.0};
        double Waterline        {0.0};
        double Volume           {0.0};
        double Displacement     {0.0};
        double KNsin            {0.0};
        bool   isValid          {false};
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


        /**
         * @brief Compute Volume, Zc, Hmeta, KN for each waterline step and each angle
         */
        void computeKNdatas();

        /**
         * @brief write data to workbook
         */
        void writeToWorkbook();

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
         * @note return HCPoint(0,-1) in case of error
         */
        Hydrodata computeHydroFromWaterline(const std::pair<HCPoint,HCPoint>& waterline );
        
        /**
         * @brief retrieve Waterline and Volume from linear interpolation
         * in the KN datas stored
         * @param Displ displacement to be considered
         * @return a pair first is Waterline, second is Volume
         */
        std::pair<double,double> getWlandVol(double displ) const;

        /**
         * @brief retrieve KNsin from linear interpolation
         * in the KN datas stored
         * @param angle angle be considered
         * @param Displ displacement to be considered
         * @return the interpolated KNsin value or 
         * std::numeric_limits<double>::min() if not available
         */
        double getKNsin(double angle, double displ) const;

        /**
         * @brief write Hydrotable on the corresponding sheet
         * @param wks the worksheet to write on 
         */
        void writeHydroTable(OpenXLSX::XLWorksheet& wks) const;

         /**
         * @brief write KN on the corresponding sheet
         * @param wks the worksheet to write on 
         * @return the number of written lines
         */
        uint32_t writeKNTable(OpenXLSX::XLWorksheet& wks) const;
        
        /**
         * @brief write notes in the dedicated worksheet
         * @param wks the worksheet to write on 
         * @note the const std::string const std::string notes[][]
         * shall be available in the namespace HydroCpp
         */
        void writeNotes(OpenXLSX::XLWorksheet& wks) const;

    private:
        std::string                 m_filename;

         /**
         * @brief key:x, value: pointer HCpolygon of cross section
         */
        std::map<double,HCPolygon*>  m_hull;
        std::vector<Hydrodata>      m_hydroTable;

        /**
         * @brief key:angle, value: vector of KN data
         */
        std::map<double,std::vector<KNdata>>     m_KNdatas; 
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
        double                      m_maxDispl;
        double                      m_deltaDispl;
        double                      m_d_sw;


    };

}  // namespace std