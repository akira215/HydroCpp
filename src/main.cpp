#include <OpenXLSX.hpp>

using namespace OpenXLSX;

int main() {

    XLDocument doc;
    doc.open("./Testcrea.xlsx");
    //auto wks = doc.workbook().worksheet("Matrix");


    doc.workbook().addTable("Feuil1","Tableau2","J10:L16");
    //doc.workbook().addWorksheet("testCpp");



    doc.save();
    doc.close();

    return 0;
}