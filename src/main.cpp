
/*
  HydroCpp
  Repository: https://github.com/btzy/nativefiledialog-extended
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

// ===== Standards Includes ===== //
#include <stdio.h>
#include <stdlib.h>
#include <string>

// ===== External Includes ===== //
#include <OpenXLSX.hpp>
#include <vector>
#include <nfd.hpp>

// ===== HydroCpp Includes ===== //
#include "HCLog.hpp"
#include "HCConfig.hpp"
#include "HCLoader.hpp"

using namespace std;
using namespace OpenXLSX;
using namespace HydroCpp;

int main() {

    HCLogInfo(std::string("Workbook shall contain the hull hydro data in a table named \"") + HYDRO_TBL_NAME +"\"" );
    HCLogInfo("All data shall be x: longitudinal forward y: transveral starboard z: vertical upward" );
    HCLogInfo("x=0: aft perpendicular y=0: centerline z=0: keel" );
    HCLogInfo("==========" );
    HCLogInfo("Additional named range could be provided: max_wl, Δwl, φMax, Δφ, ρsw" );
    
    
    // initialize NFD
    NFD::Guard nfdGuard;

    // auto-freeing memory
    NFD::UniquePath outPath;

    // prepare filters for the dialog
    nfdfilteritem_t filterItem[1] = { {"Excel file", "xls,xlsx"} };

    // show the dialog
    nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 1);
    if (result == NFD_OKAY) {
        string file = string(outPath.get());
        HCLogInfo("Opening the file " + file + "..." );
        HCLoader ld(file);
        
    } else if (result == NFD_CANCEL)
        HCLogInfo("No file was selected, user pressed cancel.");
    else 
        HCLogError("Error: " + string(NFD::GetError()) );
    

    // NFD::Guard will automatically quit NFD.

    return 0;
}