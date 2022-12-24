#include <OpenXLSX.hpp>

using namespace OpenXLSX;

int main() {

    XLDocument doc;
    doc.open("./CTest.xlsx");
    auto wks = doc.workbook().worksheet("Input");


    auto rng = wks.range(XLCellReference("A1"), XLCellReference("C20"));
    XLNamedRange testrg = doc.workbook().namedRange("defined_in_sheet");
    //doc.workbook().definedName("defined_in_shee");

    testrg = doc.workbook().namedRange("testRange");
    XLCellValue iteration;
    for (auto& cell : testrg) 
        iteration = cell.value();


    XLCell i = testrg.firstCell();
    //XLCellValue i = testrg.firstValue();
    XLCellValue itet = i.value();
    i.value() = "Changed By ASH";

    std::string n = testrg.name();

    doc.workbook().addNamedRange("NewFromCpp","Input!$F$4",2);

    //doc.workbook().definedName("nbAngles");

    //wks.cell("A1").value() = "Hello, OpenXLSX ASHHHH!";

    doc.save();
    doc.close();

    return 0;
}