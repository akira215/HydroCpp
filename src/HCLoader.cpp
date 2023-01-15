/*
  HydroCpp
  Repository: https://github.com/btzy/nativefiledialog-extended
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

#define _USE_MATH_DEFINES
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
    double wl = m_deltaWl;
    m_hydroTable.clear();
    HCLogInfo("Starting computation of hydrotable from " + std::to_string(wl) +
                " to " + std::to_string(m_maxWl) + " steps " + std::to_string(m_deltaWl));
    

    while (wl <= m_maxWl) {
        // waterline form left to right
        auto waterline = std::make_pair(HCPoint(m_minMax.xmin-1, wl),
                                        HCPoint(m_minMax.xmax+1, wl));
        Hydrodata newItem = computeHydroFromWaterline(waterline);
        if (newItem.isValid)
            m_hydroTable.push_back(newItem);
        wl += m_deltaWl;
    }
    
    //return Hydrotable
}

void HCLoader::computeKNdatas()
{
    double angle = 0.000001;
    angle = 5.0; //TODEL//////////////////////////////
    m_KNdatas.clear();
    HCLogInfo("Starting computation of KN datas from " + std::to_string(angle) +
                "° to " + std::to_string(m_maxAngle) + "° steps " + std::to_string(m_deltaAngle)+"°");
    
    while(angle <= m_maxAngle){
        double wl = m_deltaWl;
        bool finished = false;
        double tanPhi = tan(angle * M_PI/180);
        HCPoint startPt = HCPoint(m_minMax.xmin - 1, -(m_minMax.xmax - m_minMax.xmin + 1) * tanPhi);
        HCPoint endPt = HCPoint(m_minMax.xmax + 1, tanPhi);

        while (!finished){ // Loop through the waterline, stops when the waterplane is null
            //waterline from left to right
            startPt.y += m_deltaWl;
            endPt.y += m_deltaWl;
            auto waterline = std::make_pair(startPt, endPt);
            auto res = computeHydroFromWaterline(waterline);
            if (res.submerged){
                finished = true;
            } else {
                KNdata newData;
                //HCPoint evenCoB = getEvenCoBFromVolume(res.Volume);// deal with res
                newData.angle = angle;
                newData.Volume = res.Volume;
                newData.Displacement = res.Displacement;
                newData.Waterline = res.Waterline;
                double My = res.TCB / tan(angle * M_PI / 180) + res.VCB;
                newData.KNsin = My * sin (angle * M_PI / 180);
                newData.isValid = true;
                m_KNdatas.push_back(newData);
            }
            wl += m_deltaWl;
        } // Loop throuh waterlevel
        angle += m_deltaAngle;
    } // Loop through angle

}

/*
HCPoint HCLoader::getEvenCoBFromVolume(double Volume)
{
    if (Volume<m_hydroTable[0].Volume){
        // interpolation to 0
        double b = Volume  / m_hydroTable[0].Volume;
        return HCPoint( b * m_hydroTable[0].TCB,  b * m_hydroTable[0].VCB);
    }

    for (auto it = std::begin(m_hydroTable); it != std::end(m_hydroTable); ++it){
        auto next = std::next(it);
        if (next != std::end(m_hydroTable)){
            if (((*it).Volume < Volume ) && (Volume <= (*next).Volume )){
                double div = ((*next).Volume - (*it).Volume);
                if(div == 0.0){
                    HCLogError("Error, 2 waterline gave the same volume");
                    return HCPoint(0,-1); // y = -1 impossible
                }

                double a = ((*next).Volume - Volume) / div;
                double b = (Volume -  (*it).Volume) / div;
                return HCPoint( a * (*it).TCB + b * (*next).TCB, a * (*it).VCB + b * (*next).VCB);
            }
        }
    }

    return HCPoint(0,-1); // y = -1 impossible
}
*/

Hydrodata HCLoader::computeHydroFromWaterline(const std::pair<HCPoint,HCPoint>& waterline )
{
    Hydrodata hydro;
    if (waterline.second.x == waterline.first.x){
        HCLogError("Error computing table : waterline is vertical");
        return hydro;
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

        HCPolygonSplitter split(it->second, waterline);
        auto wetSection = split.getPolygonFromSide(LineSide::Right);
        auto drySection = split.getPolygonFromSide(LineSide::Left);
        
        if (drySection.empty())
            hydro.submerged = true;

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
        return hydro;
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

    hydro.isValid = true;
    return hydro; 

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