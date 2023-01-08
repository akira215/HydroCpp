/*
  HydroCpp
  Repository: https://github.com/btzy/nativefiledialog-extended
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/
// ===== Standards Includes ===== //

// ===== External Includes ===== //
#include <OpenXLSX.hpp>
// ===== HydroCpp Includes ===== //
#include "HCLoader.hpp"
#include "HCConfig.hpp"
#include "HCLog.hpp"

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
    m_deltaxWl      = getValueFromRange(wb, DELTA_WL_NAME,      DELTA_WL_DEF );
    m_maxAngle      = getValueFromRange(wb, MAX_ANGLE_NAME,     MAX_ANGLE_DEF );
    m_deltaAngle    = getValueFromRange(wb, DELTA_ANGLE_NAME,   DELTA_ANGLE_DEF );
    m_d_sw          = getValueFromRange(wb, D_SW_NAME,          D_SW_DEF );

    for (const auto& [key, value]: hull) {
        m_hull[key] = new HCPolygon(value);
    }


    //doc.save();
    doc.close();
}

HCLoader:: ~HCLoader()
{
    m_hull.clear();
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