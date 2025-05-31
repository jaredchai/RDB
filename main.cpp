//  Created by Jared Chai on 3/18/20.
//  Copyright © 2020 Tianyi Chai. All rights reserved.
//

#include <iostream>
#include "getopt.h"
#include <unordered_map>
#include <map>
#include "TableEntry.h"
#include <vector>
#include <stdio.h>
#include <string>
#include <algorithm>

using namespace std;

struct  Tables {
    enum class Types { String, Double, Int, Bool };
    size_t cols;
    vector<Types> colTypes;
    vector<string> colNames;
    map<string, size_t> colIndices;
    vector<vector<TableEntry>> content;
    string hashCol;
    unordered_map<TableEntry, vector<size_t>> hash;
    map<TableEntry, vector<size_t>, less<TableEntry>> bst;
};

enum class Operators {Lesser, Equal, Greater};

class Comparators{
private:
    Operators inputOperator;
    TableEntry ReferenceValue;
    size_t columnNumber;
public:
    Comparators(const Operators &input_op, const TableEntry  &input_TableEntry, const size_t &input_column): inputOperator(input_op), ReferenceValue(input_TableEntry),columnNumber(input_column){}
    bool operator() (vector<TableEntry>const &inputRow){
        if(inputOperator == Operators::Lesser){
            return ReferenceValue > inputRow[columnNumber];
        }
        else if(inputOperator == Operators::Equal){
            return ReferenceValue == inputRow[columnNumber];
        }
        else{
            return ReferenceValue < inputRow[columnNumber];
        }
    }
};

TableEntry declareTableEntry(const Tables::Types inputTypes){
    switch (inputTypes) {
        case Tables::Types::String:{
            string tempOne;
            cin >> tempOne;
            return TableEntry(tempOne);
            break;
        }
        case Tables::Types::Double:{
            double tempTwo;
            cin >> tempTwo;
            return TableEntry(tempTwo);
            break;
        }
        case Tables::Types::Int:{
            int tempThree;
            cin >> tempThree;
            return TableEntry(tempThree);
            break;
        }
        case Tables::Types::Bool:{
            bool tempFour;
            cin >> tempFour;
            return TableEntry(tempFour);
            break;
        }
        default:
            return TableEntry(0);
            break;
    }
}

void join(unordered_map<string, Tables> &DataBase, bool quietCheck){
    string junk;
    string tableNameOne;
    cin >> tableNameOne;
    if (DataBase.find(tableNameOne) == DataBase.end()){
        cout << "Error: "<<tableNameOne<<" does not name a table in the database\n";
        getline(cin, junk);
        return;
    }
    cin >> junk ;
    string tableNameTwo;
    cin >> tableNameTwo >> junk;
    if (DataBase.find(tableNameTwo) == DataBase.end()){
        cout << "Error: "<<tableNameTwo<<" does not name a table in the database\n";
        getline(cin, junk);
        return;
    }
    string colNameOne;
    cin >> colNameOne >> junk;
    if (find(DataBase[tableNameOne].colNames.begin(), DataBase[tableNameOne].colNames.end(), colNameOne) == DataBase[tableNameOne].colNames.end()) {
        cout << "Error: " << colNameOne << " does not name a column in " << tableNameOne << "\n";
        getline(cin, junk);
        return;
    }
    string colNameTwo;
    cin >> colNameTwo >> junk >> junk;
    if (find(DataBase[tableNameTwo].colNames.begin(), DataBase[tableNameTwo].colNames.end(), colNameTwo) == DataBase[tableNameTwo].colNames.end()) {
        cout << "Error: " << colNameTwo << " does not name a column in " << tableNameTwo << "\n";
        getline(cin, junk);
        return;
    }
    size_t numCol;
    cin >> numCol;
    vector<string> colNames;
    vector<int> colTables;
    colNames.reserve(numCol);
    colTables.reserve(numCol);
    string tempTableReference;
    for (size_t i = 0; i < numCol; i++){
        string colName;
        cin >> colName;
        int tableNum;
        cin >> tableNum;
        switch (tableNum) {
            case 1:
                tempTableReference = tableNameOne;
                break;
            case 2:
                tempTableReference = tableNameTwo;
                break;
        }
        if (find(DataBase[tempTableReference].colNames.begin(), DataBase[tempTableReference].colNames.end(), colName) == DataBase[tempTableReference].colNames.end()) {
            cout << "Error: " << colName << " does not name a column in " << tempTableReference << "\n";
            getline(cin, junk);
            return;
        }
        colNames.push_back(colName);
        colTables.push_back(tableNum);
    }
    unordered_map<TableEntry, vector<size_t>> localHash;
    for (size_t i = 0; i < DataBase[tableNameTwo].content.size(); i++) {
        size_t colIndex = unsigned(distance(DataBase[tableNameTwo].colNames.begin(),find(DataBase[tableNameTwo].colNames.begin(), DataBase[tableNameTwo].colNames.end(), colNameTwo)));
        localHash[DataBase[tableNameTwo].content[i][colIndex]].push_back(i);
    }
    if(quietCheck == false){
        for (size_t i = 0; i < colNames.size(); i++) {
            cout << colNames[i] << " ";
        }
        cout << "\n";
    }
    int rowsPrinted = 0;
    for (size_t i = 0; i < DataBase[tableNameOne].content.size(); i++) {
        auto rowinTableTwo = localHash.find((DataBase[tableNameOne].content[i][DataBase[tableNameOne].colIndices[colNameOne]]));
        if (rowinTableTwo != localHash.end()) {
            for (auto k : rowinTableTwo->second) {
                if (quietCheck == false) {
                    for (size_t j = 0; j < colNames.size(); j++) {
                        if (colTables[j] == 1) {
                            size_t index =DataBase[tableNameOne].colIndices[colNames[j]];
                            cout << DataBase[tableNameOne].content[i][index] << " ";
                        }
                        else {
                            size_t index = DataBase[tableNameTwo].colIndices[colNames[j]];
                            cout << DataBase[tableNameTwo].content[k][index] << " ";
                        }
                    }
                    cout << "\n";
                }
                rowsPrinted++;
            }
        }
    }
    cout << "Printed " << rowsPrinted << " rows from joining " << tableNameOne << " to " << tableNameTwo << "\n";
    return;
}

void updateHash(Tables &tableName, const string &colName){
    if (!tableName.hash.empty()){
        tableName.hash.clear();
        for (size_t i = 0; i < tableName.content.size();i++){
            size_t colIndex = unsigned(distance(tableName.colNames.begin(),find(tableName.colNames.begin(), tableName.colNames.end(), colName)));
            tableName.hash[tableName.content[i][colIndex]].emplace_back(i);
        }
    }
    if (!tableName.bst.empty()){
        tableName.bst.clear();
        for (size_t i = 0; i < tableName.content.size();i++){
            size_t colIndex = unsigned(distance(tableName.colNames.begin(),find(tableName.colNames.begin(), tableName.colNames.end(), colName)));
            tableName.bst[tableName.content[i][colIndex]].emplace_back(i);
        }
    }
    tableName.hashCol = colName;
}

int main(int argc, char *argv[]) {
    ios_base::sync_with_stdio(false);
    cin >> std::boolalpha;
    cout << std::boolalpha;
    int gotopt;
    int option_index = 0;
    bool quietCheck = false;
    bool finishCheck = false;
    option long_opts[] = {
        {"help", no_argument, nullptr, 'h'},
        {"quiet", no_argument, nullptr, 'q'},
        {"nullptr", 0, nullptr, '\0'},
    };
    while ((gotopt  = getopt_long(argc, argv, "hq", long_opts, &option_index)) != -1){
        switch (gotopt){
            case 'h':
                cout << "This is a help message\n";
                exit(0);
                break;
            case 'q':
                quietCheck = true;
                break;
            default:
                cerr << "Error: Oh no! I didn't recognize your flag.\n";
                return(1);
                break;
        }
    }
    //cout << "\nCompleted: getopt\n\n";
    unordered_map<string, Tables> DataBase;
    while (!finishCheck){
        cout << "% ";
        string command;
        string junk;
        cin >>command;
        if (command[0]== '#'){
            getline(cin, junk);
        }
        else if (command == "CREATE"){
            string tableName;
            cin >> tableName;
            if (DataBase.find(tableName) != DataBase.end()){
                cout << "Error: Cannot create already existing table "<< tableName <<"\n";
                getline(cin, junk);
            }
            else{
                size_t tableSize;
                cin>>tableSize;
                Tables Temp;
                Temp.cols = tableSize;
                DataBase.emplace(tableName, Temp);
                DataBase[tableName].colNames.reserve(tableSize);
                DataBase[tableName].colTypes.reserve(tableSize);
                DataBase[tableName].content.reserve(tableSize);
                DataBase[tableName].hash.reserve(tableSize);
                string cols;
                for (size_t  i = 0; i < tableSize; i++){
                    cin >> cols;
                    if (cols == "string"){
                        DataBase[tableName].colTypes.push_back(Tables::Types::String);
                    }
                    else if (cols == "double"){
                        DataBase[tableName].colTypes.push_back(Tables::Types::Double);
                    }
                    else if (cols == "int"){
                        DataBase[tableName].colTypes.push_back(Tables::Types::Int);
                    }
                    else{
                        DataBase[tableName].colTypes.push_back(Tables::Types::Bool);
                    }
                }
                cout << "New table "<<tableName<<" with column(s) ";
                for (size_t  i = 0; i < tableSize; i++){
                    cin >> cols;
                    cout << cols << " ";
                    DataBase[tableName].colNames.push_back(cols);
                    DataBase[tableName].colIndices[cols] = i;
                }
                cout <<"created\n";
            }
        }
        else if(command == "INSERT"){
            cin >> junk;
            string tableName;
            size_t newRows;
            cin >> tableName;
            cin >> newRows;
            getline(cin, junk);
            if (DataBase.find(tableName) == DataBase.end()){
                cout << "Error: "<<tableName<<" does not name a table in the database\n";
                for (size_t i = 0; i < newRows; i++){
                    getline(cin, junk);
                }
            }
            else{
                size_t initialSize = DataBase[tableName].content.size();
                vector<TableEntry> row;
                row.reserve(newRows);
                for (size_t i = 0; i < newRows; i++){
                    for (size_t j = 0; j < DataBase[tableName].cols; j++){
                        switch (DataBase[tableName].colTypes[j]) {
                            case Tables::Types::String:{
                                string tempOne;
                                cin >> tempOne;
                                row.push_back(TableEntry(tempOne));
                                break;
                            }
                            case Tables::Types::Double:{
                                double tempTwo;
                                cin >> tempTwo;
                                row.push_back(TableEntry(tempTwo));
                                break;
                            }
                            case Tables::Types::Int:{
                                int tempThree;
                                cin >> tempThree;
                                row.push_back(TableEntry(tempThree));
                                break;
                            }
                            case Tables::Types::Bool:{
                                bool tempFour;
                                cin >> tempFour;
                                row.push_back(TableEntry(tempFour));
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    DataBase[tableName].content.push_back(row);
                    row.clear();
                }
                cout << "Added " << newRows << " rows to " << tableName << " from position " << initialSize << " to " << initialSize + newRows - 1 << "\n";
                if(DataBase[tableName].hashCol != ""){
                    updateHash(DataBase[tableName], DataBase[tableName].hashCol);
                }
            }
        }
        else if(command == "DELETE"){
            cin >> junk;
            string tableName;
            cin >> tableName;
            if (DataBase.find(tableName) == DataBase.end()){
                cout << "Error: "<<tableName<<" does not name a table in the database\n";
                getline(cin, junk);
            }
            else{
                cin >> junk;
                string colName;
                cin >> colName;
                if (find(DataBase[tableName].colNames.begin(), DataBase[tableName].colNames.end(), colName) == DataBase[tableName].colNames.end()) {
                    cout << "Error: " << colName << " does not name a column in " << tableName << "\n";
                    getline(cin, junk);
                }
                else{
                    char inputOperater;
                    cin >> inputOperater;
                    Operators inputOperator_enum;
                    switch (inputOperater) {
                        case '<':
                            inputOperator_enum = Operators::Lesser;
                            break;
                        case '=':
                            inputOperator_enum = Operators::Equal;
                            break;
                        case '>':
                            inputOperator_enum = Operators::Greater;
                        break;
                        default:
                            inputOperator_enum = Operators::Equal;
                            break;
                    }
                    size_t initialSize= DataBase[tableName].content.size();
                    size_t colIndex = unsigned(distance(DataBase[tableName].colNames.begin(),find(DataBase[tableName].colNames.begin(), DataBase[tableName].colNames.end(), colName)));
                    DataBase[tableName].content.erase(
                                                      remove_if(
                                                                DataBase[tableName].content.begin(),
                                                                DataBase[tableName].content.end(),
                                                                Comparators(inputOperator_enum,
                                                                            declareTableEntry(DataBase[tableName].colTypes[colIndex]),
                                                                            colIndex)),
                                                      DataBase[tableName].content.end());
                    cout << "Deleted " << initialSize - DataBase[tableName].content.size() << " rows from " << tableName << "\n";
                    if(DataBase[tableName].hashCol != ""){
                        updateHash(DataBase[tableName], DataBase[tableName].hashCol);
                    }
                }
            }
        }
        else if(command == "GENERATE"){
            cin >> junk;
            string tableName;
            cin >> tableName;
            if (DataBase.find(tableName) == DataBase.end()){
                cout << "Error: "<<tableName<<" does not name a table in the database\n";
                getline(cin, junk);
            }
            else{
                string hashType;
                cin >> hashType;
                cin >> junk;
                cin >> junk;
                string colName;
                cin >> colName;
                if (find(DataBase[tableName].colNames.begin(), DataBase[tableName].colNames.end(), colName) == DataBase[tableName].colNames.end()) {
                    cout << "Error: " << colName << " does not name a column in " << tableName << "\n";
                    getline(cin, junk);
                }
                else{
                    if (!DataBase[tableName].hash.empty()){
                        DataBase[tableName].hash.clear();
                    }
                    if (!DataBase[tableName].bst.empty()){
                        DataBase[tableName].bst.clear();
                    }
                    if(DataBase[tableName].hashCol != ""){
                        DataBase[tableName].hashCol = "";
                    }
                    for (size_t i = 0; i < DataBase[tableName].content.size();i++){
                        if(hashType == "hash"){
                            size_t colIndex = unsigned(distance(DataBase[tableName].colNames.begin(),find(DataBase[tableName].colNames.begin(), DataBase[tableName].colNames.end(), colName)));
                            DataBase[tableName].hash[DataBase[tableName].content[i][colIndex]].emplace_back(i);
                        }
                        else{
                            size_t colIndex = unsigned(distance(DataBase[tableName].colNames.begin(),find(DataBase[tableName].colNames.begin(), DataBase[tableName].colNames.end(), colName)));
                            DataBase[tableName].bst[DataBase[tableName].content[i][colIndex]].emplace_back(i);
                        }
                    }
                    DataBase[tableName].hashCol = colName;
                    cout << "Created " << hashType << " index for table " << tableName << " on column " << colName << "\n";
                }
            }
        }
        else if(command == "PRINT"){
            cin >> junk;
            string tableName;
            cin >> tableName;
            if (DataBase.find(tableName) == DataBase.end()){
                cout << "Error: "<<tableName<<" does not name a table in the database\n";
                getline(cin, junk);
            }
            else{
                size_t numCols;
                cin >> numCols;
                vector<string> columns;
                columns.reserve(numCols);
                vector<size_t> columnIndices;
                columnIndices.reserve(numCols);
                bool columnNameCheck = true;
                size_t rowsPrinted = 0;
                for (size_t i = 0; i < numCols; i++){
                    string colName;
                    cin >> colName;
                    if (find(DataBase[tableName].colNames.begin(), DataBase[tableName].colNames.end(), colName) == DataBase[tableName].colNames.end()) {
                        cout << "Error: " << colName << " does not name a column in " << tableName << "\n";
                        getline(cin, junk);
                        columnNameCheck = false;
                        break;
                    }
                    else{
                        size_t colIndex = unsigned(distance(DataBase[tableName].colNames.begin(),find(DataBase[tableName].colNames.begin(), DataBase[tableName].colNames.end(), colName)));
                        columns.push_back(colName);
                        columnIndices.push_back(colIndex);
                    }
                }
                if (columnNameCheck == true){
                    if (quietCheck == false){
                        string allWhereCheck = "";
                        cin >> allWhereCheck;
                        if (allWhereCheck == "ALL"){
                            for (size_t i = 0; i < numCols; i++){
                                cout << columns[i] << " ";
                            }
                            cout << "\n";
                            rowsPrinted = DataBase[tableName].content.size();
                            for (size_t i = 0; i < DataBase[tableName].content .size();i++){
                                for (size_t j = 0; j < numCols; j++){
                                    cout << DataBase[tableName].content[i][columnIndices[j]]<< " ";
                                }
                                cout << "\n";
                            }
                            cout << "Printed " << rowsPrinted << " matching rows from " << tableName << "\n";
                        }
                        else {
                            string referenceColumn;
                            char input_operator;
                            Operators input_operator_enum;
                            cin >> referenceColumn >> input_operator;
                            switch (input_operator) {
                                case '<':
                                    input_operator_enum = Operators::Lesser;
                                    break;
                                case '=':
                                    input_operator_enum = Operators::Equal;
                                    break;
                                case '>':
                                    input_operator_enum = Operators::Greater;
                                    break;
                                default:
                                    input_operator_enum = Operators::Equal;
                                    break;
                            }
                            if (find(DataBase[tableName].colNames.begin(), DataBase[tableName].colNames.end(), referenceColumn) == DataBase[tableName].colNames.end()) {
                                cout << "Error: " << referenceColumn << " does not name a column in " << tableName << "\n";
                                getline(cin, junk);
                                columnNameCheck = false;
                            }
                            else{
                                for (size_t i = 0; i < numCols; i++){
                                    cout << columns[i] << " ";
                                }
                                cout << "\n";
                                size_t colIndex = unsigned(distance(DataBase[tableName].colNames.begin(),find(DataBase[tableName].colNames.begin(), DataBase[tableName].colNames.end(), referenceColumn)));
                                TableEntry referenceValue(declareTableEntry(DataBase[tableName].colTypes[colIndex]));
                                if (!DataBase[tableName].bst.empty() && DataBase[tableName].hashCol == referenceColumn){
                                    if (input_operator_enum == Operators::Lesser){
                                        auto low = DataBase[tableName].bst.lower_bound(referenceValue);
                                        for (auto i = DataBase[tableName].bst.begin(); i != low; i++) {
                                            rowsPrinted += i->second.size();
                                            for (auto j : i->second) {
                                                for (auto k : columnIndices) {
                                                    cout << DataBase[tableName].content[size_t(j)][size_t(k)] << " ";
                                                }
                                                cout << "\n";
                                            }
                                        }
                                    }
                                    else if (input_operator_enum == Operators::Equal){
                                        auto equal = DataBase[tableName].bst.find(referenceValue);
                                        if (equal != DataBase[tableName].bst.end()) {
                                            for (size_t i = 0; i < equal->second.size(); i++) {
                                                rowsPrinted++;
                                                for (size_t j = 0; j < numCols; j++) {
                                                    cout << DataBase[tableName].content[size_t(equal->second[i])][columnIndices[j]] << " ";
                                                }
                                                cout << "\n";
                                            }
                                        }
                                    }
                                    else{
                                        auto up = DataBase[tableName].bst.upper_bound(referenceValue);
                                        for (auto i = up; i != DataBase[tableName].bst.end(); i++) {
                                            rowsPrinted += i->second.size();
                                            for (auto j : i->second) {
                                                for (auto k : columnIndices) {
                                                    cout << DataBase[tableName].content[size_t(j)][size_t(k)]  << " ";
                                                }
                                                cout << "\n";
                                            }
                                        }
                                    }
                                }
                                else {
                                    vector<size_t> rowsToPrint;
                                    for (size_t i = 0; i < DataBase[tableName].content.size(); i++) {
                                            switch (input_operator_enum) {
                                                case (Operators::Lesser): {
                                                    if (DataBase[tableName].content[i][colIndex] < referenceValue) {
                                                        rowsToPrint.push_back(i);
                                                    }
                                                    break;
                                                }
                                                case (Operators::Equal): {
                                                    if (DataBase[tableName].content[i][colIndex] == referenceValue) {
                                                        rowsToPrint.push_back(i);
                                                    }
                                                    break;
                                                }
                                                case (Operators::Greater): {
                                                    if (DataBase[tableName].content[i][colIndex] > referenceValue) {
                                                        rowsToPrint.push_back(i);
                                                    }
                                                    break;
                                                }
                                            }
                                    }
                                    rowsPrinted = rowsToPrint.size();
                                    for (size_t i = 0; i < rowsToPrint.size(); i++) {
                                        for (size_t j = 0; j < columnIndices.size(); j++) {
                                            cout << DataBase[tableName].content[rowsToPrint[i]][columnIndices[j]] << " ";
                                        }
                                        cout << "\n";
                                    }
                                }
                                cout << "Printed " << rowsPrinted << " matching rows from " << tableName << "\n";
                            }
                        }
                    }
                    else{
                        string allWhereCheck = "";
                        cin >> allWhereCheck;
                        if (allWhereCheck == "ALL"){
                            rowsPrinted = DataBase[tableName].content.size();
                            cout << "Printed " << rowsPrinted << " matching rows from " << tableName << "\n";
                        }
                        else {
                            string referenceColumn;
                            char input_operator;
                            Operators input_operator_enum;
                            cin >> referenceColumn >> input_operator;
                            switch (input_operator) {
                                case '<':
                                    input_operator_enum = Operators::Lesser;
                                    break;
                                case '=':
                                    input_operator_enum = Operators::Equal;
                                    break;
                                case '>':
                                    input_operator_enum = Operators::Greater;
                                    break;
                                default:
                                    input_operator_enum = Operators::Equal;
                                    break;
                            }
                            if (find(DataBase[tableName].colNames.begin(), DataBase[tableName].colNames.end(), referenceColumn) == DataBase[tableName].colNames.end()) {
                                cout << "Error: " << referenceColumn << " does not name a column in " << tableName << "\n";
                                getline(cin, junk);
                                columnNameCheck = false;
                            }
                            else{
                                size_t colIndex = unsigned(distance(DataBase[tableName].colNames.begin(),find(DataBase[tableName].colNames.begin(), DataBase[tableName].colNames.end(), referenceColumn)));
                                TableEntry referenceValue(declareTableEntry(DataBase[tableName].colTypes[colIndex]));
                                if (!DataBase[tableName].bst.empty() && DataBase[tableName].hashCol == referenceColumn){
                                    if (input_operator_enum == Operators::Lesser){
                                        auto low = DataBase[tableName].bst.lower_bound(referenceValue);
                                        for (auto i = DataBase[tableName].bst.begin(); i != low; i++) {
                                            rowsPrinted += i->second.size();
                                        }
                                    }
                                    else if (input_operator_enum == Operators::Equal){
                                        if (DataBase[tableName].bst.find(referenceValue) != DataBase[tableName].bst.end()) {
                                            rowsPrinted+= DataBase[tableName].bst.find(referenceValue)->second.size();
                                        }
                                    }
                                    else{
                                        auto up = DataBase[tableName].bst.upper_bound(referenceValue);
                                        for (auto i = up; i != DataBase[tableName].bst.end(); i++) {
                                            rowsPrinted += i->second.size();
                                        }
                                    }
                                }
                                else {
                                    for (size_t i = 0; i < DataBase[tableName].content.size(); i++) {
                                        switch (input_operator_enum) {
                                            case (Operators::Lesser): {
                                                if (DataBase[tableName].content[i][colIndex] < referenceValue) {
                                                    rowsPrinted++;
                                                }
                                                break;
                                            }
                                            case (Operators::Equal): {
                                                if (DataBase[tableName].content[i][colIndex] == referenceValue) {
                                                    rowsPrinted++;
                                                }
                                                break;
                                            }
                                            case (Operators::Greater): {
                                                if (DataBase[tableName].content[i][colIndex] > referenceValue) {
                                                    rowsPrinted++;
                                                }
                                                break;
                                            }
                                        }
                                    }
                                }
                                cout << "Printed " << rowsPrinted << " matching rows from " << tableName << "\n";
                            }
                        }
                    }
                }
            }
        }
        else if(command == "JOIN"){
            join(DataBase, quietCheck);
        }
        else if(command == "REMOVE"){
            string tableName;
            cin >> tableName;
            if(DataBase.find(tableName) ==  DataBase.end()){
                cout << "Error: " <<tableName<< " does not name a table in the database\n";
            }
            else{
                DataBase.erase(DataBase.find(tableName));
                cout<<"Table "<<tableName<< " deleted"<<"\n";
            }
        }
        else if(command == "QUIT"){
            cout << "Thanks for being silly!\n" ;
            finishCheck = true;
            return(0);
        }
        else{
            cout << "Error: unrecognized command\n";
            getline(cin, junk);
            continue;
        }
    }
}
