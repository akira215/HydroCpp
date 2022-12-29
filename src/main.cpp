#include <OpenXLSX.hpp>

using namespace OpenXLSX;

int main() {

    XLDocument doc;
    doc.open("./tablecrea.xlsx");
    auto wks = doc.workbook().worksheet("Matrix");


    doc.workbook().addTable("Matrix","Tableau2","B1:C20");

    auto matrix = wks.range("C4:G16");
    int myCell = matrix[57].value();


    auto testTable = doc.workbook().table("tbl_one");



    doc.save();
    doc.close();

    return 0;
}