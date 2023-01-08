/*
  HydroCpp
  Repository: https://github.com/btzy/nativefiledialog-extended
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/
// ===== Standards Includes ===== //
#include <utility>
#include <cmath>
// ===== External Includes ===== //
#include <OpenXLSX.hpp>
// ===== HydroCpp Includes ===== //
#include "HCLoader.hpp"
#include "HCConfig.hpp"
#include "HCLog.hpp"
#include "HCPolygonSplitter.hpp"

using namespace HydroCpp;
using namespace OpenXLSX;

HCLoader::HCLoader(const std::string& filename):m_filename(filename)
{
    XLDocument doc;
    doc.open(m_filename);
    XLWorkbook wb = doc.workbook();
    XLTable tbl = wb.table(HYDRO_TBL_NAME);

    uint16_t nx = tbl.columnIndex("x");
    uint16_t ny = tbl.columnIndex("y");
    uint16_t nz = tbl.columnIndex("z");

    std::map<double,std::vector<HCPoint>> hull;
    for(auto& row : tbl.tableRows()){
        double x = row[nx].value().getAsDouble();
        double y = row[ny].value().getAsDouble();
        double z = row[nz].value().getAsDouble();

        hull[x].push_back(HCPoint(y, z)); // step required to gather all the x
    }

    m_maxWl         = getValueFromRange(wb, MAX_WL_NAME,        MAX_WL_DEF );
    m_deltaWl       = getValueFromRange(wb, DELTA_WL_NAME,      DELTA_WL_DEF );
    m_maxAngle      = getValueFromRange(wb, MAX_ANGLE_NAME,     MAX_ANGLE_DEF );
    m_deltaAngle    = getValueFromRange(wb, DELTA_ANGLE_NAME,   DELTA_ANGLE_DEF );
    m_d_sw          = getValueFromRange(wb, D_SW_NAME,          D_SW_DEF );

    for (const auto& [key, value]: hull) {
        m_hull[key] = new HCPolygon(value);
        checkMinMax(value);
    }


    //doc.save();
    doc.close();
}

HCLoader:: ~HCLoader()
{
    m_hull.clear();
}

void HCLoader::computeHydroTable()
{
    //Hydrotable =[]
    double wl = m_deltaWl;

    while (wl <= m_maxWl) {
        // waterline form left to right
        auto waterline = std::make_pair(HCPoint(m_minMax.xmin-1, wl),
                                        HCPoint(m_minMax.xmax+1, wl));
        computeHydrotable(waterline);
        wl += m_deltaWl;
    }
    
    //return Hydrotable
}
/// @brief ///////////////////////////////////////////////////////////////////////////////////
/// @param waterline /
void HCLoader::computeHydrotable(const std::pair<HCPoint,HCPoint>& waterline )
{
    double elmtLength = 0.0;
    for (auto it = m_hull.begin(); it != m_hull.end(); ++it) {
        
        // the length of the last element will be the same as the n-1 one
        auto next = std::next(it);
        if (next != m_hull.end())
            elmtLength = abs(next->first - it->first);

        HCPolygonSplitter split(it->second->getVertices(), waterline);
        auto wetSection = split.getPolygonFromSide(LineSide::Right);
        auto wetEdge = split.getEdges();



    }
    

}

double HCLoader::getValueFromRange(const OpenXLSX::XLWorkbook& wb,
                                    const std::string& rngName, 
                                    double defaultVal)
{
    double val;
    try
    {
        val = wb.namedRange(rngName).firstCell().value().getAsDouble();
    }
    catch(const std::exception& e)
    {
        HCLogError("Named range \"" + rngName + "\" does not exist, default value {" 
                                + std::to_string(defaultVal) + "} will be used");
        val = defaultVal;
    }
    
    return val;
}

void HCLoader::checkMinMax(const std::vector<HCPoint>& section )
{
    for (const auto& pt : section){
        if ( m_minMax.xmin > pt.x)
            m_minMax.xmin = pt.x;
        if ( m_minMax.xmax < pt.x)
            m_minMax.xmax = pt.x;
        if ( m_minMax.ymin > pt.y)
            m_minMax.ymin = pt.y;
        if ( m_minMax.ymax < pt.y)
            m_minMax.ymax = pt.y;
    }
}