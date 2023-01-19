# HydroCpp

HydroCpp is a C++ software to compute hydrostatic and KN tables of a hull
using Microsoft Excel® files, with the .xlsx format, to exchange data.
It has no user interface, only an open file dialog box and message in
the commandline.

## Table of Contents

- [Motivation](#motivation)
- [Ambition](#ambition)
- [Build Instructions](#build-instructions)
- [Current Status](#current-status)
- [Performance](#performance)
- [Caveats](#caveats)
- [Usage and Examples](#usage-and-examples)
- [Changes](#changes)

## Motivation

All software that are used to compute hydrostatic data are huge soft,
with heavy 3D user interface, and require to model the hull using its format.

Because some kind of lightweighted interface is sufficient to get ideas of
stability of fit for purposed hull (mainly barges and modular pontoon), I
decided to develop this HydroCpp software.

I previsously made some trials using Excel, VBA and Python, but for 
performances reasons, none of this solution was able to get quick results

## Ambition

To reduce as few as possible the dependencies, I decided to remove any user interface. The ambition is that HydroCpp should be able to read hull data directly in a Microsoft Excel® files, and write the results in it.  Currently, HydroCpp depends on the following
3rd party libraries:

- OpenXLSX (great developpement work on it to get the table management)
- nfd (to get access to a light open file dialog)

## Build Instructions
HydroCpp uses CMake as the build system (or build system generator, to be exact). Therefore, you must install CMake first, in order to build HydroCpp. You can find installation instructions on www.cmake.org.

The libraries are located in the HydroCpp subdirectory to this repo, and 
are managed as git submodules. To install them, simply type in the root path of the project:

```
$ git submodule add https://github.com/akira215/OpenXLSX.git OpenXLSX
$ git submodule add https://github.com/akira215/nativefiledialog-extended.git nfd
```

## Current Status

HydroCpp is still work in progress, but is completely running. All bugs 
could be reported to try to improve it.

## Performance

No much tests have been performed, but to compute all the data with default steps on a 60 section hull form, it typically require 3s, including writing results in the file

## Caveats

### To be developped



## Usage and Examples

In the 'Examples' folder, you will find a typical xlsx input file, and a typical output file. 

The minimum required info in the xlsx file to be able to run HydroCpp is:
 * a table named `tbl_Hullform`, containting at least 3 columns named x,y, z

The system of coordinates of the hull is:
 - x along the length of the ship, from aft to fore, x=0 at the aft perpendicular (no negative values)
 - y, transverse to the ship, from portside to starboard y=0 at the centerline of the ship
 - z, perpendicular of the xy plane, z positive upward, z=0 at keel (no negative value)

Note that each section will be applied to length corresponding a x(n+1)-x(n),
except the last one that will be applied to the previous one (as ther is no n+1). So the (n) section shall be the section of the ship located between x(n) and x(n+1).

To control the behavior of the computation, additional data could be supplied using named range in the excel file:
 * `max_wl`, max waterline level (draught) for hydrostatic computation
 * `Δwl`, step of computation of draught for both hydrostatic and KN computation
 * `φMax`, max list angle for KN computation
 * `Δφ`, step of computation for KN computation
 * `max_Disp`, max displacement for gathering info in KN computation
 * `ΔDisp`, step displacement for gathering info in KN computation
 * `ρsw`, sea water density

Each one of this field has a default value that will be used if the named range is not find.

The sofware then generate 3 sheets:
 * `Hydrostatics`, containing the `tbl_Hydrostatics` table with hydrostatic datas
 * `KNTable`, containing the  `tbl_KNTable` table with KN datas
 * `Notes`, containing the names, units and values that are provided

Note that all the name of this fields could be tweaked at compile time, by editing the HCConfig.hpp file

```cpp
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

```

Also the soft compute for each waterline step KN datas, until completely submerging the hull (it don't stop at the max waterline level). Then it gather this datas in ship displacement distribution, by linear interpolation previous calculated results. To increase the accuracy of this interpolation, the global waterline step could be increase (but the experience is that it is not critical).

## Changes

### New in version 0.0.0


## Credits

- Thanks to [@troldal](https://github.com/troldal/OpenXLSX) for providing the OpenXLSX library (and merging my pull requests !).

