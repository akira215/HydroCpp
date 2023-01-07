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

using namespace HydroCpp;
using namespace OpenXLSX;

HCLoader::HCLoader(const std::string& filename):m_filename(filename)
{
    XLDocument doc;
    doc.open(m_filename);
    XLTable tbl = doc.workbook().table(HYDRO_TBL_NAME);

    uint16_t nx = tbl.columnIndex("x");
    uint16_t ny = tbl.columnIndex("y");
    uint16_t nz = tbl.columnIndex("z");

    for(auto& row : tbl.tableRows()){
        double x = row[nx].value().getAsDouble();
        double y = row[ny].value().getAsDouble();
        double z = row[nz].value().getAsDouble();

        m_hull[x].push_back(HCPoint(y, z));
    }
    
    
    //doc.save();
    doc.close();
}

HCLoader:: ~HCLoader()
{}
