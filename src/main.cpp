#include <OpenXLSX.hpp>
#include <vector>

using namespace std;
using namespace OpenXLSX;

int main() {

    
    // First, create a new document and access the 1st sheet.
    cout << "\nGenerating spreadsheet ..." << endl;
    XLDocument doc;
    doc.create("./HydroCpp.xlsx");
    auto wks = doc.workbook().sheet(1).get<XLWorksheet>();
    string sheetName = wks.name();

    // Fill the column header and create the table
    // If the table header is not set, default name will be setup
    // Also columns name shall be unique, a increment marks is added if required
    cout << "Creating table ..." << endl;
    wks.cell("B2").value() = "#";
    wks.cell("C2").value() = "One";
    wks.cell("D2").value() = "Col";
    wks.cell("E2").value() = "With";
    wks.cell("F2").value() = "Table";

    XLTable myTable = doc.workbook().addTable(sheetName,"MyTable","B2:F28");

    // Save the sheet...
    cout << "Saving spreadsheet ..." << endl;
    doc.save();
    doc.close();

    // ...and reopen it (just to make sure that it is a valid .xlsx file)
    cout << "Re-opening spreadsheet ..." << endl << endl;
    doc.open("./HydroCpp.xlsx");
    wks = doc.workbook().worksheet(sheetName);

    // We can retrieve the headers name:
    XLTable tbl = doc.workbook().table("MyTable");
    vector<std::string> headers = tbl.columnNames();

    cout << "Table "<< tbl.name() << " header names:" << endl;
    int i = 0;
    for(const auto& h : headers){
        cout << i++ << " - " << h << endl;
    }

    // We can also access different items
    auto ncol       = tbl.columnIndex("Col"); // Retrieve the colum index by the name
    auto sht        = tbl.getWorksheet();      // Retrieve and object worksheet
    auto rang       = tbl.tableRange();       // whole table range including total & headers
    auto bodyRange  = tbl.dataBodyRange();    // Only data body table range (excl. total & headers)
    auto nRow       = tbl.rowsCount();    // Only data body table range (excl. total & headers)
    // And iterate throught the table rows
    auto nCol       = tbl.columnsCount();    // Only data body table range (excl. total & headers)
    
    int j = 0;
/*
    for (auto row = tbl.tableRows().begin(); row != tbl.tableRows().end(); ++row){
        (*row)[tbl.columnIndex("#")].value() = j;
        (*row)[1].value() = "Data" + to_string(j);
        (*row)[tbl.columnIndex("Table")].value() = (float)j * 2.0f / 3.0f;
        j++;

    }

*/
    for(auto& row : tbl.tableRows()){
        row[tbl.columnIndex("#")].value() = j;
        row[1].value() = "Data" + to_string(j);
        row[tbl.columnIndex("Table")].value() = (float)j * 2.0f / 3.0f;
        j++;
    }


    tbl.autofilter().hideArrows();
    //tbl.setHeaderVisible(false);
    tbl.setTotalVisible(true);
    //tbl.column("Table")->setTotalsRowFunction("sum");
    //tbl.column("Table")->setTotalsRowFunction("count");
    tbl.column("Table").setTotalsRowFunction("count");
    


    doc.save();
    doc.close();

    return 0;
}