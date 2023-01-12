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
    Hydrodata hydro;
    if (waterline.second.x == waterline.first.x){
        XLLogError("Error computing table : waterline is vertical");
        return;
    }
    hydro.Waterline = waterline.first.y - (waterline.second.y - waterline.first.y) / 
                        (waterline.second.x - waterline.first.x) * waterline.first.x;
    double elmtLength = 0.0;
    uint32_t nLCF = 0;
    for (auto it = m_hull.begin(); it != m_hull.end(); ++it) {
        
        // the length of the last element will be the same as the n-1 one
        auto next = std::next(it);
        if (next != m_hull.end())
            elmtLength = abs(next->first - it->first);

        HCPolygonSplitter split(it->second->getVertices(), waterline);
        auto wetSection = split.getPolygonFromSide(LineSide::Right);
        auto wetEdge = split.getEdges();

        double eltVol = wetSection.getArea() * elmtLength;
        double xelt = (*it).first + elmtLength / 2;

        hydro.LCB += xelt * eltVol;
        hydro.TCB += wetSection.getCog().x * eltVol;
        hydro.VCB += wetSection.getCog().y * eltVol;

        hydro.Volume += eltVol;
        hydro.Lpp += elmtLength;

        // Compute for each edge of the waterline cut the lentgh and the inertia
        double interLength = 0.0;
        for(const auto& s : wetEdge){
            interLength += s.first.distanceTo(s.second);
            HCPoint midSectionPt = HCPoint( (s.first.x + s.second.x) / 2,
                                           (s.first.y + s.second.y) / 2 );
            // Transport the inertia at x = 0 waterline
            double dt = midSectionPt.distanceTo(HCPoint( 0.0, hydro.Waterline ));
            hydro.RMT += elmtLength * pow(interLength, 3) /12 +
                        (interLength * elmtLength) * pow(dt, 2);
        }

        hydro.RML += interLength * pow(elmtLength, 3) / 12 + (interLength * elmtLength) * pow(xelt, 2);
        hydro.WaterplaneArea += interLength * elmtLength;

        if (wetSection.getArea() != 0){
            if (nLCF == 0){
                hydro.LCF += (*it).first;
                nLCF += 1;
            }
            hydro.LCF += (*it).first + elmtLength;
            nLCF +=1;
        }

    } // Section Loop

    // If null don't save the data
    if ((nLCF == 0 )|| (hydro.Volume == 0.0))
        return;
    hydro.LCF /= nLCF;
    hydro.LCB /= hydro.Volume;
    hydro.TCB /= hydro.Volume;
    hydro.VCB /= hydro.Volume;
    hydro.Displacement = hydro.Volume  * m_d_sw;
    hydro.Immersion = hydro.WaterplaneArea * m_d_sw / 100; // in t/cm
    hydro.RMT -= hydro.WaterplaneArea * pow (hydro.TCB,2); // Transport RMT to CoB
    hydro.RMT /= hydro.Displacement;
    hydro.RML -= hydro.WaterplaneArea * pow (hydro.LCB,2); // Transport RML to CoB  
    hydro.RML /= hydro.Displacement; 
    hydro.MCT = hydro.Displacement * hydro.RML / (100 * hydro.Lpp);
    hydro.KMT = hydro.RMT + hydro.VCB;  

    m_hydroTable.push_back(hydro); 

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