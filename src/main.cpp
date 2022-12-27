#include <OpenXLSX.hpp>

using namespace OpenXLSX;

int main() {

    XLDocument doc;
    doc.open("./testSimple.xlsx");
    auto wks = doc.workbook().worksheet("Input");


    auto testTable = doc.workbook().table("tbl_one");


    //testTable.setName("tbl_ash");
    auto testName = testTable.name();
    auto cols = testTable.columnNames();

    auto ncol = testTable.columnIndex("Col bord");
    auto sht = testTable.getSheet();
    std::string shtName = sht.name();

    //doc.workbook().definedName("nbAngles");

    //wks.cell("A1").value() = "Hello, OpenXLSX ASHHHH!";

    doc.save();
    doc.close();

    return 0;
}