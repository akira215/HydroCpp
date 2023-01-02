#include <OpenXLSX.hpp>
#include <vector>

using namespace std;
using namespace OpenXLSX;

int main() {

    XLDocument doc;
    
        // ...and reopen it (just to make sure that it is a valid .xlsx file)
    cout << "Re-opening spreadsheet ..." << endl << endl;
    doc.open("./Demo10.xlsx");
    auto wks = doc.workbook().sheet(1).get<XLWorksheet>();
    //wks = doc.workbook().worksheet(sheetName);

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
    for(const auto& row : tbl.tableRows()){
        row[tbl.columnIndex("#")].value() = j;
        row[1].value() = "Data" + to_string(j);
        row[tbl.columnIndex("Table")].value() = (float)j * 2.0f / 3.0f;
        j++;
    }

    // Also show the total with selected function
    tbl.autofilter().hideArrows();
    //tbl.setHeaderVisible(false);
    tbl.setTotalVisible(true);
    //tbl.column("Table")->setTotalsRowFunction("sum");
    tbl.column("Table").setTotalsRowFunction("");
    tbl.column("Table").setTotalsRowFunction("count");

    cout << "Table Style : " << tbl.tableStyle().style() << endl;
    tbl.tableStyle().setStyle("test"); 
    

    doc.save();
    doc.close();

    return 0;
}