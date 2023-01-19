/*
  HydroCpp
  Repository: https://github.com/akira215/HydroCpp
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/
#pragma once
#include <string>

#define HULL_TBL_NAME       "tbl_Hullform"

#define HYDRO_SHEET_NAME    "Hydrostatics"
#define HYDRO_TBL_NAME      "tbl_Hydrostatics"

#define KN_SHEET_NAME       "KNTable"
#define KN_TBL_NAME         "tbl_KNTable"

#define NOTES_SHEET_NAME     "Notes"

#define MAX_WL_NAME         "max_wl"
#define DELTA_WL_NAME       "Δwl"
#define MAX_ANGLE_NAME      "φMax"
#define DELTA_ANGLE_NAME    "Δφ"
#define MAX_DISPL_NAME      "max_Disp"
#define DELTA_DISPL_NAME    "ΔDisp"
#define D_SW_NAME           "ρsw"

#define MAX_WL_DEF          2.0
#define DELTA_WL_DEF        0.01
#define MAX_ANGLE_DEF       60.0
#define DELTA_ANGLE_DEF     5.0
#define MAX_DISPL_DEF       2000.0 // Not used, calculated
#define DELTA_DISPL_DEF     10.0
#define D_SW_DEF            1.025

#define ANGLE0              0.00000001

namespace HydroCpp
{
    const std::string notes[][3] = {
        { "Hydrostatics","","" },
        {"Data", "Unit", "Comment"},
        {"Draught", "m","water height from keel + upward"},
        {"Volume", "m3","Immerged hull volume"},
        {"Displacement", "t","Displacement considering the density below"},
        {"Immersion", "t/cm","Weight to sink 1cm"},
        {"MCT", "t.m/cm","Moment to Change Trim by 1cm"},
        {"LCB", "m","Longitudinal position of Center Of Buoyancy from Aft"},
        {"TCB", "m","Transversal position of Center Of Buoyancy from centerline, always 0"},
        {"LCF", "m","Longitudinal position of Center Of Flotation from Aft"},
        {"KMT", "m","Transversal Metacentric Height above Keel"},
        {"WaterplaneArea", "m2","Area of the hull @ waterline"},
        {"RML", "m","Longitudinal Metacentric Radius above keel"},
        {"RMT", "m","Transversal Metacentric Radius above keel"},
        {"Lpp", "m","Length between perpendicular"},
        {"VCB", "m","Vertical position of Center Of Buoyancy from Keel"},
        {"----------","",""},
        {"KNTable"},
        {"Data", "Unit", "Comment"},
        {"Draught", "m","water height from keel + upward"},
        {"Volume", "m3","Immerged hull volume"},
        {"Displacement", "t","Displacement considering the density below"},
        {"Phi", "°","List angle (headers row)"},
        {"KNsin", "m","Projected distance of K on the inclined centerline"},
        {"----------","",""},
        {"Generals","",""},
        {"Data", "Unit", "Comment"},
        {"Lpp", "%L","m"},
        {"ρsw", "%D","t/m3 sea water density"},
        {"----------","",""},
        {"Repo:","https://github.com/akira215/HydroCpp",""},
    };
}