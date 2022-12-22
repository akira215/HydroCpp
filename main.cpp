#include <OpenXLSX.hpp>

using namespace OpenXLSX;

int main() {

    XLDocument doc;
    doc.create("Spreadsheet.xlsx");
    auto wks = doc.workbook().worksheet("Sheet1");

    wks.cell("A1").value() = "Hello, OpenXLSX ASHHHH!";

    doc.save();

    return 0;
}