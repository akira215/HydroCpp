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
#include <sstream>
#include <iomanip>
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
    XLTable tbl = wb.table(HULL_TBL_NAME);

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

    // Get hull max values
    for (const auto& [key, value]: hull) {
        m_hull[key] = new HCPolygon(value);
        checkMinMax(value);
    }

    m_maxWl         = getValueFromRange(wb, MAX_WL_NAME,        MAX_WL_DEF );
    m_deltaWl       = getValueFromRange(wb, DELTA_WL_NAME,      DELTA_WL_DEF );

    // Length of the ship minus the step
    double DisplMax = m_hull.rbegin()->first - m_hull.begin()->first;
    DisplMax *= (m_minMax.xmax - m_minMax.xmin);
    DisplMax *= m_maxWl;

    m_maxAngle      = getValueFromRange(wb, MAX_ANGLE_NAME,     MAX_ANGLE_DEF );
    m_deltaAngle    = getValueFromRange(wb, DELTA_ANGLE_NAME,   DELTA_ANGLE_DEF );
    m_maxDispl      = getValueFromRange(wb, MAX_DISPL_NAME,     /*MAX_DISPL_DEF*/ DisplMax );
    m_deltaDispl    = getValueFromRange(wb, DELTA_DISPL_NAME,   DELTA_DISPL_DEF );
    m_d_sw          = getValueFromRange(wb, D_SW_NAME,          D_SW_DEF );

    doc.close();
}

HCLoader:: ~HCLoader()
{
    m_hull.clear();
}

void HCLoader::writeToWorkbook()
{
    XLDocument doc;
    doc.open(m_filename);
    XLWorkbook wb = doc.workbook();
    

    // Write Hydro table
    if(wb.sheetExists(HYDRO_SHEET_NAME))
        wb.deleteSheet(HYDRO_SHEET_NAME);
    
    auto wksHydro = wb.addWorksheet(HYDRO_SHEET_NAME);
    wksHydro.setTabColor(OpenXLSX::XLColor("C00000"));
    writeHydroTable(wksHydro);
    OpenXLSX::XLCellReference bl(m_hydroTable.size() + 1, 14);
    std::string ref = "A1:" + bl.address(false);
    auto tblHydro = wb.addTable(HYDRO_SHEET_NAME, HYDRO_TBL_NAME, ref );
    tblHydro.tableStyle().setStyle("TableStyleMedium2");
    tblHydro.autofilter().hideArrows();

    // Write KN table
     if(wb.sheetExists(KN_SHEET_NAME))
        wb.deleteSheet(KN_SHEET_NAME);
    
    auto wksKN = wb.addWorksheet(KN_SHEET_NAME);
    wksKN.setTabColor(OpenXLSX::XLColor("C00000"));
    uint32_t n = writeKNTable(wksKN);
    OpenXLSX::XLCellReference blk(n + 1, m_KNdatas.size() + 3);
    ref = "A1:" + blk.address(false);
    auto tblKN = wb.addTable(KN_SHEET_NAME, KN_TBL_NAME, ref );
    tblKN.tableStyle().setStyle("TableStyleMedium2");
    tblKN.autofilter().hideArrows();

    // Save and close
    doc.save();
    doc.close();
}

void HCLoader::writeHydroTable(XLWorksheet& wks) const 
{
    std::vector<XLCellValue> header;
    header.emplace_back("Draught");
    header.emplace_back("Volume");
    header.emplace_back("Displacement");
    header.emplace_back("Immersion");
    header.emplace_back("MCT");
    header.emplace_back("LCB");
    header.emplace_back("TCB");
    header.emplace_back("LCF");
    header.emplace_back("KMT");
    header.emplace_back("WaterplaneArea");
    header.emplace_back("RMT");
    header.emplace_back("RML");
    header.emplace_back("VCB");
    header.emplace_back("Lpp");

    auto headerRow = wks.row(1);
    headerRow.values() = header;

    std::vector<XLCellValue> rowValues;
    for (size_t i = 0; i < m_hydroTable.size(); ++i){
        auto row = wks.row(i+2);
        rowValues.clear();

        const Hydrodata& d = m_hydroTable[i];

        rowValues.emplace_back(d.Waterline);
        rowValues.emplace_back(d.Volume);
        rowValues.emplace_back(d.Displacement);
        rowValues.emplace_back(d.Immersion);
        rowValues.emplace_back(d.MCT);
        rowValues.emplace_back(d.LCB);
        rowValues.emplace_back(d.TCB);
        rowValues.emplace_back(d.LCF);
        rowValues.emplace_back(d.KMT);
        rowValues.emplace_back(d.WaterplaneArea);
        rowValues.emplace_back(d.RMT);
        rowValues.emplace_back(d.RML);
        rowValues.emplace_back(d.VCB);
        rowValues.emplace_back(d.Lpp);

        row.values() = rowValues;
    }

}

uint32_t HCLoader::writeKNTable(XLWorksheet& wks) const 
{
    // Setup and write header list
    std::vector<XLCellValue> header;
    header.emplace_back("Draught");
    header.emplace_back("Volume");
    header.emplace_back("Displacement");
    // loop through angles
    for(auto const& angle: m_KNdatas){
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << angle.first;
        header.emplace_back(ss.str());
    }
        
    auto headerRow = wks.row(1);
    headerRow.values() = header;

    uint32_t i = 0;
    std::vector<XLCellValue> rowValues;
    for (double displ = 10 * m_deltaDispl ; displ < m_maxDispl; displ += m_deltaDispl ){
        auto row = wks.row(i+2);
        rowValues.clear();

        auto wV = getWlandVol(displ);

        if(wV.first != -1)
            rowValues.emplace_back(wV.first);
        else
            rowValues.emplace_back("");
    
        if(wV.second != -1)
            rowValues.emplace_back(wV.second);
        else
            rowValues.emplace_back("");

        rowValues.emplace_back(displ);
        
        for(auto const& angle: m_KNdatas){
            double KNval = getKNsin(angle.first, displ);
            if(KNval == std::numeric_limits<double>::min())
                rowValues.emplace_back("");
            else
                rowValues.emplace_back(KNval);
        }

        row.values() = rowValues;
        ++i;
    }
    return i;
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
    
}

void HCLoader::computeKNdatas()
{
    double angle = ANGLE0;
    std::vector<KNdata> KNdatas;

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
                KNdatas.push_back(newData);
            }
            wl += m_deltaWl;
        } // Loop throuh waterlevel
        if (angle == ANGLE0)
            angle = m_deltaAngle;
        else
            angle += m_deltaAngle;
    } // Loop through angle

    //  ====== Reorganise the datas
    for(auto& d: KNdatas){
        m_KNdatas[d.angle].push_back(d);
    }

}


std::pair<double,double> HCLoader::getWlandVol(double displ) const
{
    if (displ < m_KNdatas.at(ANGLE0)[0].Displacement){
        // interpolation to 0
        double b = displ  / m_KNdatas.at(ANGLE0)[0].Displacement;
        return std::make_pair( b * m_KNdatas.at(ANGLE0)[0].Waterline, 
                             b * m_KNdatas.at(ANGLE0)[0].Volume);
    }

    for (auto it = std::begin(m_KNdatas.at(ANGLE0)); it != std::end(m_KNdatas.at(ANGLE0)); ++it){
        auto next = std::next(it);
        if (next != std::end(m_KNdatas.at(ANGLE0))){
            if (((*it).Displacement < displ ) && (displ <= (*next).Displacement )){
                double div = ((*next).Displacement - (*it).Displacement);
                if(div == 0.0){
                    HCLogError("Error, 2 waterline gave the same Displacement");
                    return std::make_pair(-1.0,-1.0); // y = -1 impossible
                }

                double a = ((*next).Displacement - displ) / div;
                double b = (displ -  (*it).Displacement) / div;
                return std::make_pair( a * (*it).Waterline + b * (*next).Waterline, 
                                        a * (*it).Volume + b * (*next).Volume);
            }
        }
    }

    return std::make_pair(-1.0,-1.0); // y = -1 impossible
}

double HCLoader::getKNsin(double angle, double displ) const
{
    const auto& vect = m_KNdatas.at(angle);

    // Angle value don't exists
    if (vect.empty())
        return std::numeric_limits<double>::min();
    
    if (displ < vect[0].Displacement){
        // interpolation to 0
        double b = displ  / vect[0].Displacement;
        return  b * vect[0].KNsin;
    }

    // Interpolation
    for (auto it = std::begin(vect); it != std::end(vect); ++it){
        auto next = std::next(it);
        if (next != std::end(vect)){
            if (((*it).Displacement < displ ) && (displ <= (*next).Displacement )){
                double div = ((*next).Displacement - (*it).Displacement);
                if(div == 0.0){
                    HCLogError("Error, 2 waterline gave the same Displacement");
                    return std::numeric_limits<double>::min();
                }

                double a = ((*next).Displacement - displ) / div;
                double b = (displ -  (*it).Displacement) / div;
                return (a * (*it).KNsin + b * (*next).KNsin);
            }
        }
    }

    // Not found
    return std::numeric_limits<double>::min();
}


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