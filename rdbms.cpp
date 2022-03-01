#include<iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <numeric>
#include <iomanip>
#include<bits/stdc++.h>
using namespace std;

fstream schemas,schemasJ;
fstream table,table1,help;
fstream tableConstraints;

string tableName,tableNameJ;
vector<string> parsed_command;
vector<string> subCreateStat;
vector<string> constraints;
vector<pair<string,string>> tableAttributes,tableAttributesJ;
vector<string> insertData;

int noOfRecords;
const char separator    = ' ';
const int nameWidth     = 12;
const int numWidth      = 12;

bool is_number(const string& );
vector<string> splitString(string, string);
string mergeIntoString(vector<string>, string);
bool isValidIdentifier(string);
vector<string> getAttrType(string, int);
vector<string> getAllConstraints(string);
bool isPresent(string);
void createSchema();
void createTable();
int cmp(pair<string,int>, pair<string,int>);
int checkN(string, string, double, double);
int checkS(string, string, string, string);
void insertIntoTable();
vector<vector<string>> getFk(string);
vector<vector<string>> getRt(string);
void deleteFromTable();
void updateTable();
int getColNo(string,string);
vector<string> getCol(string, string);
vector<int> relationalOperationN(string, string, double);
vector<int> relationalOperationSV(string, string, string);
vector<int> relationalOperationS(string, string, string);
vector<int> relationalOperationN_J(string, string, double);
vector<int> relationalOperationSV_J(string, string, string);
vector<int> relationalOperationS_J(string, string, string);
vector<int> logicalOperation(vector<int>, vector<int>, string);
int getColNo_J(string,string);
vector<string> getCol_J(string, string);
void selectFromTable();
void getSchema(string);
void getNoOfRecords();
void describeTable();
void dropTable();
void helpTable();
void helpCmd();
void quit();

bool is_number(const string& s)
{
    return( strspn( s.c_str(), "-.0123456789" ) == s.size() );
}

vector<string> splitString(string s , string delimiter){
    vector<string> ans;

    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        ans.push_back(token);
        s.erase(0, pos + 1);
    }
    ans.push_back(s);
    return ans;
}

string mergeIntoString(vector<string> v , string delimiter){
    string ans;
    for(int i = 0 ; i < v.size() ; i++){
        if(i == v.size() - 1){
            ans = ans + v[i];
        }else{
            ans = ans + v[i] + delimiter;
        }
    }
    return ans;
} 

bool isValidIdentifier(string str){
    if (!((str[0] >= 'a' && str[0] <= 'z') || (str[0] >= 'A' && str[0] <= 'Z') || str[0] == '_'))
        return false;
 
    for (int i = 1; i < str.length(); i++) {
        if (!((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= '0' && str[i] <= '9') || str[i] == '_'))
            return false;
    }
 
    return true;
}


vector<string> getAttrType(string tName, int attrNo){
    vector<string> ans;
    getSchema(tName);
    if(tableAttributes[attrNo].second == "INT"){
        ans.push_back("INT");
    }else if(tableAttributes[attrNo].second == "DECIMAL"){
        ans.push_back("DECIMAL");
    }else if(tableAttributes[attrNo].second == "STR"){
        ans.push_back("STR");
    }else if(tableAttributes[attrNo].second[0] == 'C'){
        string a = tableAttributes[attrNo].second.substr(0,4);
        string b = tableAttributes[attrNo].second.substr(5,tableAttributes[attrNo].second.size()-6);
        ans.push_back(a);
        ans.push_back(b);
    }
    return ans;
}

vector<string> getAllConstraints(string tName){
    vector<string> ans;
    string c = "db/tableContraints/" + tName + ".txt";
    tableConstraints.open(c, ios::in);
    if(tableConstraints){
        string cons;
        int count = 0;
        while(getline(tableConstraints,cons)){
            if(cons[2] == ':'){
                if(cons[0] == 'P'){
                    vector<string> pk = splitString(cons.substr(4,cons.size()-4),"#");
                    for(int i = 0 ; i < pk.size(); i++){
                        int colNo = getColNo(tName,pk[i]);
                        if(ans[colNo] == "-"){
                            ans[colNo] = "PRIMARY KEY";
                        }else{
                            ans[colNo] = ans[colNo] + ", PRIMARY KEY";
                        }
                    }
                }else if(cons[0] == 'F'){
                    vector<string> fk = splitString(cons.substr(4,cons.size()-4)," ");
                    int colNo = getColNo(tName,fk[0]);
                    if(ans[colNo] == "-"){
                        ans[colNo] = "FOREIGN KEY REFERENCES " + fk[1] + " (" + fk[2] +") ";
                    }else{
                        ans[colNo] = ans[colNo] + "FOREIGN KEY REFERENCES" + fk[1] + " (" + fk[2] +") ";
                    }
                }else if(cons[0] == 'R'){
                    continue;
                }
            }
            ans.push_back(cons);
        }
        tableConstraints.close();
        return ans;
    }else{
        tableConstraints.close();
        cout<<"Unable to access the constraints!"<<endl;
    }
}


bool isPresent(string tName){
    schemas.open("db/schemas.txt",ios::in);
    string schema;
    while(getline(schemas,schema)){
        int i = 0;
        string tempTableName;
        while(schema[i] != '#'){
            tempTableName = tempTableName + schema[i];
            i++;
        }
        //cout<<tempTableName<<endl;
        if(tempTableName == tName){
            schemas.close();
            return true;
        }
    }
    schemas.close();
    return false;
}

void createSchema(){
    string tempSchema;

    schemas.open("db/schemas.txt" , ios::app);
    tempSchema = tempSchema + tableName + "#";
    for(int j=0 ; j<subCreateStat.size() ; j++){
        if(j == subCreateStat.size() - 1){
            tempSchema = tempSchema + subCreateStat[j];
        }else{
            tempSchema = tempSchema + subCreateStat[j] + "#";
        }
    }
    schemas<<tempSchema;
    schemas<<"\n";
    schemas.close();
    
    tableAttributes.clear();
    vector<string> splitStr = splitString(tempSchema , "#");
    for(int i = 1 ; i < splitStr.size() ; i++){
        tableAttributes.push_back(make_pair(splitStr[i],splitStr[i+1]));
        i++;
    }
    // for(auto k : tableAttributes){
    //     cout<<k.first<<"->"<<k.second<<" ";
    // }
    // cout<<endl;
}

void createTable(){
    if(parsed_command[1] == "TABLE"){
        tableName = parsed_command[2];

        if(isValidIdentifier(tableName)){
            if(parsed_command[3] == "("){
                subCreateStat.clear();
                constraints.clear();
                string s;
                int end = 0;
                for(int i = 4 ; i < parsed_command.size() ; ){
                    if(parsed_command[i] == "PRIMARY" && parsed_command[i+1] == "KEY"){
                        vector<string> subpk = splitString(parsed_command[i+2].substr(1,parsed_command[i+2].size()-2), ",");
                        string pk = "PK: ";
                        for(int j = 0 ; j < subpk.size() ; j++){
                            if(j == subpk.size() - 1){
                                pk = pk + subpk[j];
                            }else{
                                pk = pk + subpk[j] + "#";
                            }
                        }
                        constraints.push_back(pk);
                        //cout<<pk<<endl;
                        i = i+3;
                        end = 1;
                    }else if(parsed_command[i] == "FOREIGN" && parsed_command[i+1] == "KEY"){
                        string fk = "FK: " + parsed_command[i+2].substr(1,parsed_command[i+2].size()-2) + " " + parsed_command[i+4] + " " + parsed_command[i+5].substr(1,parsed_command[i+5].size()-2);
                        if(!isPresent(parsed_command[i+4])){
                            cout<<"ERROR: Referenced table '"<<parsed_command[i+4]<<"' does not exist!"<<endl;
                            return;
                        }
                        constraints.push_back(fk);

                        string c = "db/tableContraints/" + parsed_command[i+4] + ".txt";
                        tableConstraints.open(c, ios::app);
                        if(tableConstraints){
                            tableConstraints<<"RT: "<<parsed_command[i+5].substr(1,parsed_command[i+5].size()-2)<<" "<<tableName<<" "<<parsed_command[i+2].substr(1,parsed_command[i+2].size()-2)<<endl;
                            
                        }
                        tableConstraints.close();

                        //cout<<fk<<endl;
                        i = i+6;
                        end = 1;
                    }else if(parsed_command[i] == "CHECK"){
                        subCreateStat.push_back(s.substr(0,s.size()-1));
                        s = "";

                        i++;
                        string conCheck;
                        if(parsed_command[i] == "("){
                            i++;
                            while(parsed_command[i] != ")"){
                                conCheck = conCheck + parsed_command[i] + " ";
                                i++;
                            }
                        }
                        constraints.push_back(conCheck.substr(0,conCheck.size()-1));
                        i++;
                        end = 1;
                    }else if(parsed_command[i] == ","){
                        if(end == 0){
                            subCreateStat.push_back(s.substr(0,s.size()-1));
                            s = "";
                            constraints.push_back("-");
                        }
                        i++;
                    }else{
                        s = s + parsed_command[i] + "#";
                        //cout<<s<<endl;
                        i++;
                        end = 0;
                    }
                    
                    if( parsed_command[i] == ");" ){
                        if(end == 0){
                            subCreateStat.push_back(s.substr(0,s.size()-1));
                            s = "";
                            constraints.push_back("-");
                        }
                        break;
                    }
                }

                // cout<<subCreateStat.size()<<endl;
                // for(int i = 0 ; i < subCreateStat.size() ; i++){
                //     cout<<i<<": "<<subCreateStat[i]<<endl;
                // }
                
                if(!isPresent(tableName)){
                    createSchema();
                    
                    string t = "db/tables/" + tableName + ".txt";
                    table.open(t, ios::out);
                    if(table){
                        cout<<"Table created successfully!"<<endl;
                    }else{
                        cout<<"Error in creating table!"<<endl;
                    }
                    table.close();

                    string c = "db/tableContraints/" + tableName + ".txt";
                    tableConstraints.open(c, ios::out);
                    if(tableConstraints){
                        for(int i = 0 ; i < constraints.size() ; i++){
                            tableConstraints<<constraints[i]<<"\n";
                        }
                    }else{
                        cout<<"Error in creating tableConstraints!"<<endl;
                    }
                    tableConstraints.close();
                }else{
                    cout<<"Error: name is already used by an existing table."<<endl;
                }
            }
        }else{
            cout<<"Invalid tableName!"<<endl;
        }

    }else{
        cout<<"Command not found...Try Agian!"<<endl;
    }
}


int cmp(pair<string,int> a , pair<string,int> b){
    return a.second < b.second;
}

int checkN(string attribute, string optr, double consValue, double userValue){
    if(optr == "="){
        if(consValue == userValue)  return 1;
        else return 0;
    }else if(optr == "!="){
        if(userValue != consValue)  return 1;
        else return 0;
    }else if(optr == ">"){
        if(userValue > consValue)  return 1;
        else return 0;
    }else if(optr == "<"){
        if(userValue < consValue)  return 1;
        else return 0;
    }else if(optr == ">="){
        if(userValue >= consValue)  return 1;
        else return 0;
    }else if(optr == "<="){
        if(userValue <= consValue)  return 1;
        else return 0;
    }
}

int checkS(string attribute, string optr, string consValue, string userValue){
    consValue = consValue.substr(1,consValue.size()-2);
    if(optr == "="){
        if(consValue == userValue)  return 1;
        else return 0;
    }else if(optr == "!="){
        if(userValue != consValue)  return 1;
        else return 0;
    }else if(optr == ">"){
        if(userValue > consValue)  return 1;
        else return 0;
    }else if(optr == "<"){
        if(userValue < consValue)  return 1;
        else return 0;
    }else if(optr == ">="){
        if(userValue >= consValue)  return 1;
        else return 0;
    }else if(optr == "<="){
        if(userValue <= consValue)  return 1;
        else return 0;
    }
}

void insertIntoTable(){
    if(parsed_command[1] == "INTO" && parsed_command[3] == "VALUES"){
        tableName = parsed_command[2];
        getSchema(tableName);
        if(isPresent(tableName)){
            if(parsed_command[4] == "("){
                insertData.clear();
                for(int i = 5 ; i < parsed_command.size() ; i++){
                    if(parsed_command[i] == ");"){
                        break;
                    }else if(parsed_command[i] == ","){
                        continue;
                    }else{
                        insertData.push_back(parsed_command[i]);
                    }
                }

                for(int i = 0 ; i < insertData.size() ; i++){
                    vector<string> type = getAttrType(tableName, i);
                    if(type[0] == "INT"){
                        if(!is_number(insertData[i])){
                            cout<<"ERROR: Expected integer value for attribute '"<<tableAttributes[i].first<<"' ."<<endl;
                            return;
                        }
                        insertData[i] = to_string(stoi(insertData[i]));
                    }else if(type[0] == "STR"){
                        if(insertData[i][0] != '\'' && insertData[i][insertData.size()-1] != '\''){
                            cout<<"ERROR: Expected string value for attribute '"<<tableAttributes[i].first<<"' ."<<endl;
                            return;
                        }
                        insertData[i] = insertData[i].substr(1,insertData[i].size()-2);
                    }else if(type[0] == "DECIMAL"){
                        if(!is_number(insertData[i])){
                            cout<<"ERROR: Expected decimal value for attribute '"<<tableAttributes[i].first<<"' ."<<endl;
                            return;
                        }
                        insertData[i] = to_string(stod(insertData[i]));
                    }else if(type[0] == "CHAR"){
                        if(insertData[i][0] != '\'' && insertData[i][insertData[i].size()-1] != '\''){
                            cout<<"ERROR: Expected character value for attribute '"<<tableAttributes[i].first<<"' ."<<endl;
                            return;
                        }
                        int expectedLenght = stoi(type[1]);
                        int actualLength = insertData[i].size()-2;

                        if(actualLength > expectedLenght){
                            cout<<"ERROR: Maximum lenght for attribute "<<tableAttributes[i].first<<" is "<<expectedLenght<<" but got "<<actualLength<<" ."<<endl;
                            return;
                        }
                        insertData[i] = insertData[i].substr(1,insertData[i].size()-2);
                    }
                }
                // for(int i = 0 ; i < insertData.size() ; i++){
                //     cout<<insertData[i]<<" ";
                // }

                string c = "db/tableContraints/" + tableName + ".txt";
                tableConstraints.open(c, ios::in);
                if(tableConstraints){
                    string cons;
                    int count = 0;
                    while(getline(tableConstraints,cons)){
                        if(cons == "-"){
                            count++;
                            continue;
                        }else if(cons[0] == 'P' && cons[1] == 'K' && cons[2] == ':'){
                                vector<string> pk = splitString(cons, " "); 
                                if(pk[1].find("#") == string::npos){
                                    int colNo = getColNo(tableName,pk[1]);
                                    //cout<<"colno: "<<colno<<endl;

                                    vector<string> tempCol = getCol(tableName,pk[1]);
                                    // for(int i = 0 ; i < tempCol.size() ; i++){
                                    //     cout<<tempCol[i]<<endl;
                                    // }
                                    
                                    if(find(tempCol.begin() , tempCol.end() , insertData[colNo]) != tempCol.end()){
                                        cout<<"ERROR: Primary key constraint violated!"<<endl;
                                        tableConstraints.close();
                                        return;
                                    }
                                }else{
                                    //cout<<"In here!"<<endl;
                                    vector<pair<string,int>> pk_attributes_colno;
                                    vector<string> pk_attributes = splitString(pk[1], "#");
                                    for(int i = 0 ; i < pk_attributes.size() ; i++){
                                        pk_attributes_colno.push_back(make_pair(pk_attributes[i], getColNo(tableName,pk_attributes[i])));
                                    }
                                    sort(pk_attributes_colno.begin(),pk_attributes_colno.end() , cmp);
                                    // for(int i = 0 ; i < pk_attributes_colno.size() ; i++){
                                    //     cout<<pk_attributes_colno[i].first<<endl;
                                    // }
                                    
                                    string curr;
                                    for(int i = 0 ; i < pk_attributes_colno.size() ; i++){
                                        curr = curr + insertData[pk_attributes_colno[i].second] + "#";
                                    }
                                    //cout<<curr<<endl;

                                    vector<vector<string>> MultiCol;
                                    vector<string> temp;
                                    for(int i = 0 ; i < pk_attributes_colno.size() ; i++){
                                        temp = getCol(tableName,pk_attributes_colno[i].first);
                                        MultiCol.push_back(temp);
                                        temp.clear();
                                    }

                                    string tempValue;
                                    for(int i = 0 ; i < MultiCol[0].size() ; i++){
                                        for(int j = 0 ; j < MultiCol.size() ; j++){
                                            tempValue = tempValue + MultiCol[j][i] + "#";
                                        }
                                        if(curr == tempValue){
                                            cout<<"ERROR: Primary key constraint violated!"<<endl;
                                            tableConstraints.close();
                                            return;
                                        }else{
                                            tempValue = "";
                                        }
                                    }
                                }
                        }else if(cons[0] == 'F' && cons[1] == 'K' && cons[2] == ':'){
                            vector<string> fk = splitString(cons," ");
                            // for(int i = 0 ; i < fk.size() ; i++){
                            //     cout<<fk[i]<<" ";
                            // }
                            // cout<<endl;
                            int referencingTableColNo = getColNo(tableName,fk[1]);
                            vector<string> referencedTableCol = getCol(fk[2],fk[3]);
                            if(find(referencedTableCol.begin(),referencedTableCol.end(),insertData[referencingTableColNo]) == referencedTableCol.end()){
                                cout<<"ERROR: Referential Integrity constraint violated!"<<endl;
                                tableConstraints.close();
                                return;
                            }

                        }else if(cons[0] == 'R' && cons[1] == 'T' && cons[2] == ':'){
                            continue;
                        }else{
                            string userValue = insertData[count];
                            vector<string> splittedConstraints = splitString(cons," ");
                            vector<int> boolVector;
                            vector<string> logicalOptr;
                            int finalBoolValue;

                            int i = 0;
                            while(i < splittedConstraints.size()){
                                if(splittedConstraints[i] == "AND" || splittedConstraints[i] == "OR"){
                                    logicalOptr.push_back(splittedConstraints[i]);
                                    i++;
                                    continue;
                                }
                                string attribute1 = splittedConstraints[i++];
                                string optr = splittedConstraints[i++];
                                string value = splittedConstraints[i++];

                                if(is_number(value)){
                                    boolVector.push_back(checkN(attribute1, optr, stod(value), stod(userValue)));
                                }else{
                                    boolVector.push_back(checkS(attribute1, optr, value, userValue));
                                }
                            }

                            if(boolVector.size() == 1){
                                finalBoolValue = boolVector[0];
                            }else{
                                finalBoolValue = boolVector[0];
                                for(int i = 1 ; i < boolVector.size() ; i++){
                                    if(logicalOptr[i-1] == "AND"){
                                        finalBoolValue =  finalBoolValue & boolVector[i]; 
                                    }else if(logicalOptr[i-1] == "OR"){
                                        finalBoolValue =  finalBoolValue | boolVector[i]; 
                                    }
                                }
                            }

                            if(finalBoolValue == 0){
                                cout<<"ERROR: Attribute '"<<tableAttributes[count].first<<"' "<<"does not follow the check constraint!"<<endl;
                                tableConstraints.close();
                                return;
                            }
                            count++;
                        }
                    }
                }else{
                    cout<<"ERROR: Unable to access the constraints!"<<endl;
                }
                tableConstraints.close();

                string t = "db/tables/" + tableName + ".txt";
                table.open(t, ios::app);
                if(table){
                    string insertString;
                    for(int i = 0 ; i < insertData.size() ; i++){
                        if(i == insertData.size() - 1){
                            insertString = insertString + insertData[i];
                            break;
                        }
                        insertString = insertString + insertData[i] + "#";
                    }
                    //cout<<insertString<<" "<<endl;
                    table<<insertString<<"\n";
                    table.close();
                    cout<<"Record inserted sucessfully!"<<endl;
                }else{
                    cout<<"ERROR: Error in inserting!"<<endl;
                }
                table.close();
            }
        }else{
            cout<<"ERROR: table does not exist"<<endl;
        }
    }else{
        cout<<"ERROR: Command not found...Try Agian!"<<endl;
    }
}


vector<vector<string>> getFk(string tName){
    vector<vector<string>> ans;
    vector<string> tempAns;
    string c = "db/tableContraints/" + tName + ".txt";
    tableConstraints.open(c, ios::in);
    if(tableConstraints){
        string cons;
        while(getline(tableConstraints,cons)){
            tempAns.clear();
            if(cons[0] == 'F' && cons[1] == 'K' && cons[2] == ':'){
                vector<string> temp = splitString(cons, " ");
                for(int i = 1 ; i < temp.size() ; i++){
                    tempAns.push_back(temp[i]);
                }
                ans.push_back(tempAns);
            }else{
                continue;
            }
        }
    }
    tableConstraints.close();
    return ans;
}

vector<string> getPk(string tName){
    vector<string> ans;
    string c = "db/tableContraints/" + tName + ".txt";
    tableConstraints.open(c, ios::in);
    if(tableConstraints){
        string cons;
        while(getline(tableConstraints,cons)){
            if(cons[0] == 'P' && cons[1] == 'K' && cons[2] == ':'){
                vector<string> temp1 = splitString(cons, " ");
                ans = splitString(temp1[1], "#");
                tableConstraints.close();
                return ans;
            }else{
                continue;
            }
        }
    }
    tableConstraints.close();
    return ans;
}

vector<vector<string>> getRt(string tName){
    vector<vector<string>> ans;
    vector<string> tempAns;
    string c = "db/tableContraints/" + tName + ".txt";
    tableConstraints.open(c, ios::in);
    if(tableConstraints){
        string cons;
        while(getline(tableConstraints,cons)){
            tempAns.clear();
            if(cons[0] == 'R' && cons[1] == 'T' && cons[2] == ':'){
                vector<string> temp = splitString(cons, " ");
                for(int i = 1 ; i < temp.size() ; i++){
                    tempAns.push_back(temp[i]);
                }
                ans.push_back(tempAns);
            }else{
                continue;
            }
        }
    }
    tableConstraints.close();
    return ans;
}

void deleteFromTable(){
    if(parsed_command[1] == "FROM"){
        tableName = parsed_command[2];
        getSchema(tableName);
        if(isPresent(tableName)){
            //cout<<"In here"<<endl;
            if(parsed_command[3] == "WHERE"){
                    vector<string> logicalOperator;
                    vector<vector<int>> boolVectors;
                    vector<int> subBoolVector;
                    vector<int> finalBoolVector;

                    int i = 4;
                    while(true){
                        string conAttribute1 = parsed_command[i++];
                        string optr = parsed_command[i++];
                        string value = parsed_command[i++];
                        subBoolVector.clear();

                        if(is_number(value)){
                            double intValue = stod(value);
                            subBoolVector = relationalOperationN(conAttribute1,optr,intValue);
                        }else if(value[0] == '\''){
                            string strValue = value.substr(1,value.size()-2);
                            subBoolVector = relationalOperationSV(conAttribute1,optr,strValue);
                        }else{
                            subBoolVector = relationalOperationS(conAttribute1,optr,value);
                        }
                        
                        boolVectors.push_back(subBoolVector);

                        if(parsed_command[i] == ";"){
                            break;
                        }else{
                            logicalOperator.push_back(parsed_command[i++]);
                        }
                    }

                    //Printing individual bool vectors
                    // cout<<"Individual Bool Vectors : "<<endl;
                    // for(int i = 0 ; i < boolVectors.size() ; i++){
                    //     for(int j = 0 ; j < boolVectors[i].size() ; j++){
                    //         cout<<boolVectors[i][j]<<" ";
                    //     }
                    //     cout<<"\n";
                    // }
                    // cout<<"\n";

                    if(boolVectors.size() == 1){
                        finalBoolVector = boolVectors[0];
                    }else{
                        finalBoolVector = boolVectors[0];
                        for(int i = 1 ; i < boolVectors.size() ; i++){       
                            finalBoolVector = logicalOperation(finalBoolVector,boolVectors[i],logicalOperator[i-1]);
                        }
                    }

                    //Printing final bool vector
                    // cout<<"Final Bool Vector : "<<endl;
                    // for(int i = 0 ; i < finalBoolVector.size() ; i++){
                    //     cout<<finalBoolVector[i]<<" ";
                    // }
                    // cout<<"\n";

                    vector<string> allTuples;
                    string t = "db/tables/" + tableName + ".txt";
                    table.open(t, ios::in);
                    if(table){
                        string tuple;
                        int count = 0;
                        while(getline(table,tuple)){
                            allTuples.push_back(tuple);
                        }
                    }
                    table.close();

                    vector<vector<string>> rt = getRt(tableName);
                    // for(int i = 0; i < rt.size(); i++){
                    //     cout<<rt[i]<<" ";
                    // }
                    // cout<<endl;

                    if(rt.size() != 0){
                        for(int i = 0 ; i < rt.size() ; i++){
                            vector<string> a = getCol(tableName, rt[i][0]);
                            vector<string> b = getCol(rt[i][1], rt[i][2]);

                            string t = "db/tables/" + tableName + ".txt";
                            table.open(t, ios::in);
                            if(table){
                                string line;
                                int count = 0;
                                while(getline(table, line)){
                                    if(finalBoolVector[count] == 1){
                                        if(find(b.begin() , b.end() , a[count]) != b.end()){
                                            cout<<"ERROR: Can't delete the rows as foreign key constraint gets violated!"<<endl;
                                            table.close();
                                            return;
                                        }
                                    }
                                    count++;
                                }
                            }
                            table.close();
                        }
                    }

                    vector<string> tuplesAfterDeleting;
                    for(int i = 0 ; i < allTuples.size(); i++){
                        if(finalBoolVector[i] == 0){
                            tuplesAfterDeleting.push_back(allTuples[i]);
                        }
                    }

                    int deletedTuples = allTuples.size() - tuplesAfterDeleting.size();
                    
                    // for(int i = 0 ; i < tuplesAfterDeleting.size() ; i++){
                    //     cout<<tuplesAfterDeleting[i]<<endl;
                    // }
                    cout<<deletedTuples<<" record(s) from "<<tableName<<" deleted."<<endl;
                    cout<<""<<endl;
                    
                    t = "db/tables/" + tableName + ".txt";
                    table.open(t, ios::out);
                    if(table){
                        for(int i = 0 ; i < tuplesAfterDeleting.size() ; i++){
                            table<<tuplesAfterDeleting[i]<<"\n";
                        }
                    }else{
                        cout<<"ERROR: Error in deleting records from table!"<<endl;
                    }
                    table.close();
                }else{
                    cout<<"ERROR: Command not found...Try Agian!"<<endl;
                }
            }else{
                cout<<"ERROR: table does not exist"<<endl;
            }
        }else{
            cout<<"ERROR: Command not found...Try Again!"<<endl;
        }
}


string getConstraint(string tName, int attrNo){
    string ans;
    string c = "db/tableContraints/" + tName + ".txt";
    tableConstraints.open(c, ios::in);
    if(tableConstraints){
        string cons;
        int count = 0;
        while(getline(tableConstraints,cons)){
            if(count == attrNo){
                ans = cons;
                break;
            }
            count++;
        }
        tableConstraints.close();
        return ans;
    }else{
        tableConstraints.close();
        cout<<"ERROR: Unable to access the constraints!"<<endl;
    }
}

void updateTable(){
    tableName = parsed_command[1];
    if(isPresent(tableName)){
        if(parsed_command[2] == "SET"){
            vector<vector<string>> attributesToUpdate;
            vector<string> attributeToUpdate;
            int i = 3;
            while(parsed_command[i] != "WHERE"){
                if(parsed_command[i] == ","){
                    i++;
                }
                attributeToUpdate.push_back(parsed_command[i++]);
                attributeToUpdate.push_back(parsed_command[i++]);
                attributeToUpdate.push_back(parsed_command[i++]);

                attributesToUpdate.push_back(attributeToUpdate);
                attributeToUpdate.clear();
            }

            vector<string> logicalOperator;
            vector<vector<int>> boolVectors;
            vector<int> subBoolVector;
            vector<int> finalBoolVector;

            i++;
            while(true){
                string conAttribute1 = parsed_command[i++];
                string optr = parsed_command[i++];
                string value = parsed_command[i++];
                subBoolVector.clear();

                if(is_number(value)){
                    double intValue = stod(value);
                    subBoolVector = relationalOperationN(conAttribute1,optr,intValue);
                }else if(value[0] == '\''){
                    string strValue = value.substr(1,value.size()-2);
                    subBoolVector = relationalOperationSV(conAttribute1,optr,strValue);
                }else{
                    subBoolVector = relationalOperationS(conAttribute1,optr,value);
                }
                    
                boolVectors.push_back(subBoolVector);

                if(parsed_command[i] == ";"){
                    break;
                }else{
                    logicalOperator.push_back(parsed_command[i++]);
                }
            }

            //Printing individual bool vectors
            // cout<<"Individual Bool Vectors : "<<endl;
            // for(int i = 0 ; i < boolVectors.size() ; i++){
            //     for(int j = 0 ; j < boolVectors[i].size() ; j++){
            //         cout<<boolVectors[i][j]<<" ";
            //     }
            //     cout<<"\n";
            // }
            // cout<<"\n";

            if(boolVectors.size() == 1){
                finalBoolVector = boolVectors[0];
            }else{
                finalBoolVector = boolVectors[0];
                for(int i = 1 ; i < boolVectors.size() ; i++){       
                    finalBoolVector = logicalOperation(finalBoolVector,boolVectors[i],logicalOperator[i-1]);
                }
            }

            //Printing final bool vector
            // cout<<"Final Bool Vector : "<<endl;
            // for(int i = 0 ; i < finalBoolVector.size() ; i++){
            //     cout<<finalBoolVector[i]<<" ";
            // }
            // cout<<"\n";

            int noOfUpdates = 0;
            vector<string> updatedTuples;
            string t = "db/tables/" + tableName + ".txt";
            table.open(t, ios::in);
            if(table){
                string tuple;
                int count = 0;
                while(getline(table,tuple)){
                    if(finalBoolVector[count] == 1){
                        noOfUpdates++;
                        vector<int> colNo;
                        for(int i = 0 ; i < attributesToUpdate.size() ; i++){
                            colNo.push_back(getColNo(tableName,attributesToUpdate[i][0]));
                        }

                        //DataType Constraint
                        vector<string> tupleToUpdate = splitString(tuple,"#");
                        vector<string> tupleToUpdateTemp = splitString(tuple,"#");
                        for(int i = 0 ; i < colNo.size(); i++){
                            vector<string> tempAttrType = getAttrType(tableName,colNo[i]);
                            if(tempAttrType[0] == "STR"){
                                if(attributesToUpdate[i][2][0] != '\'' && attributesToUpdate[i][2][attributesToUpdate[i][2].size()-1] != '\''){
                                    cout<<"ERROR: Expected string value for attribute '"<<tableAttributes[i].first<<"' ."<<endl;
                                    table.close();
                                    return;
                                }
                                tupleToUpdate[colNo[i]] = attributesToUpdate[i][2].substr(1,attributesToUpdate[i][2].size() - 2);
                            }else if(tempAttrType[0] == "CHAR"){
                                if(attributesToUpdate[i][2][0] != '\'' &&  attributesToUpdate[i][2][attributesToUpdate[i][2].size()-1] != '\''){
                                    cout<<"ERROR: Expected character value for attribute '"<<tableAttributes[i].first<<"' ."<<endl;
                                    table.close();
                                    return;
                                }
                                int expectedLenght = stoi(tempAttrType[1]);
                                int actualLength = attributesToUpdate[i][2].size()-2;
                                if(actualLength > expectedLenght){
                                    cout<<"ERROR: Maximum lenght for attribute "<<tableAttributes[i].first<<" is "<<expectedLenght<<" but got "<<actualLength<<" ."<<endl;
                                    table.close();
                                    return;
                                }
                                tupleToUpdate[colNo[i]] = attributesToUpdate[i][2].substr(1,attributesToUpdate[i][2].size() - 2);
                            }else if(tempAttrType[0] == "INT"){
                                if(!is_number(attributesToUpdate[i][2])){
                                    cout<<"ERROR: Expected integer value for attribute '"<<tableAttributes[i].first<<"' ."<<endl;
                                    table.close();
                                    return;
                                }
                                tupleToUpdate[colNo[i]] = to_string(stoi(attributesToUpdate[i][2]));
                            }else if(tempAttrType[0] == "DECIMAL"){
                                if(!is_number(insertData[i])){
                                    cout<<"ERROR: Expected decimal value for attribute '"<<tableAttributes[i].first<<"' ."<<endl;
                                    table.close();
                                    return;
                                }
                                tupleToUpdate[colNo[i]] = to_string(stod(attributesToUpdate[i][2]));
                            }                         
                        }

                        //Check Constraint
                        for(int j = 0 ; j < colNo.size() ; j++){
                            string cons = getConstraint(tableName,colNo[j]);
                            //cout<<cons<<endl;
                            if(cons == "-"){
                                continue;
                            }else{
                                string userValue = tupleToUpdate[colNo[j]];
                                vector<string> splittedConstraints = splitString(cons," ");
                                vector<int> boolVector;
                                vector<string> logicalOptr;
                                int finalBoolValue;

                                int i = 0;
                                while(i < splittedConstraints.size()){
                                    if(splittedConstraints[i] == "AND" || splittedConstraints[i] == "OR"){
                                        logicalOptr.push_back(splittedConstraints[i]);
                                        i++;
                                        continue;
                                    }
                                    string attribute1 = splittedConstraints[i++];
                                    string optr = splittedConstraints[i++];
                                    string value = splittedConstraints[i++];

                                    if(is_number(value)){
                                        boolVector.push_back(checkN(attribute1, optr, stod(value), stod(userValue)));
                                    }else{
                                        boolVector.push_back(checkS(attribute1, optr, value, userValue));
                                    }
                                }

                                if(boolVector.size() == 1){
                                    finalBoolValue = boolVector[0];
                                }else{
                                    finalBoolValue = boolVector[0];
                                    for(int i = 1 ; i < boolVector.size() ; i++){
                                        if(logicalOptr[i-1] == "AND"){
                                            finalBoolValue =  finalBoolValue & boolVector[i]; 
                                        }else if(logicalOptr[i-1] == "OR"){
                                            finalBoolValue =  finalBoolValue | boolVector[i]; 
                                        }
                                    }
                                }

                                if(finalBoolValue == 0){
                                    cout<<"ERROR: Attribute '"<<tableAttributes[colNo[j]].first<<"' "<<"does not follow the check constraint!"<<endl;
                                    table.close();
                                    return;
                                }
                                //count++;
                            }
                        }

                        //Primary key Constraint
                        vector<string> tempPk = getPk(tableName);
                        // for(int i = 0 ; i < tempPk.size() ; i++){
                        //     cout<<tempPk[i]<<endl;
                        // }
                        if(tempPk.size() != 0){
                            if(tempPk.size() == 1){
                                int pkColNo = getColNo(tableName,tempPk[0]);
                                //cout<<"colno: "<<colno<<endl;

                                vector<string> tempCol = getCol(tableName,tempPk[0]);
                                // for(int i = 0 ; i < tempCol.size() ; i++){
                                //     cout<<tempCol[i]<<endl;
                                // }
                                
                                if(find(tempCol.begin() , tempCol.end() , tupleToUpdate[pkColNo]) != tempCol.end()){
                                    cout<<"ERROR: Primary key constraint violated!"<<endl;
                                    table.close();
                                    return;
                                }
                            }else{
                                vector<pair<string,int>> pk_attributes_colno;
                                vector<string> pk_attributes = tempPk;
                                for(int i = 0 ; i < pk_attributes.size() ; i++){
                                    pk_attributes_colno.push_back(make_pair(pk_attributes[i], getColNo(tableName,pk_attributes[i])));
                                }
                                sort(pk_attributes_colno.begin(),pk_attributes_colno.end() , cmp);
                                // for(int i = 0 ; i < pk_attributes_colno.size() ; i++){
                                //     cout<<pk_attributes_colno[i].first<<" : "<<pk_attributes_colno[i].second<<endl;
                                // }
                                
                                string curr;
                                for(int i = 0 ; i < pk_attributes_colno.size() ; i++){
                                    curr = curr + tupleToUpdate[pk_attributes_colno[i].second] + "#";
                                }
                                //cout<<curr<<endl;

                                vector<vector<string>> MultiCol;
                                vector<string> temp;
                               // cout<<pk_attributes_colno.size()<<endl;
                                for(int i = 0 ; i < pk_attributes_colno.size() ; i++){
                                    temp.clear();
                                    //cout<<tableName<<" "<<pk_attributes_colno[i].first<<endl;
                                    temp = getCol(tableName,pk_attributes_colno[i].first);
                                    // cout<<temp.size()<<endl;
                                    // for(int ii = 0 ; ii < temp.size() ; ii++){
                                    //     cout<<temp[ii]<<endl;
                                    // }
                                    MultiCol.push_back(temp);
                                    temp.clear();
                                    //cout<<"In here"<<endl;
                                }

                                string tempValue;
                                for(int i = 0 ; i < MultiCol[0].size() ; i++){
                                    //cout<<"In here1"<<endl;
                                    if(i != count){
                                        for(int j = 0 ; j < MultiCol.size() ; j++){
                                            tempValue = tempValue + MultiCol[j][i] + "#";
                                        }
                                        //cout<<tempValue<<endl;

                                        if(curr == tempValue){
                                            cout<<"ERROR: Primary key constraint violated!"<<endl;
                                            table.close();
                                            return;
                                        }else{
                                            tempValue = "";
                                        }
                                    }
                                }
                            }
                        }
                        
                        //Foreign key Constraint
                        //1. referenced table
                        vector<vector<string>> tempFk = getFk(tableName);
                        for(int l = 0 ; l < tempFk.size() ; l++){
                            //cout<<tempFk[l][0]<<" "<<tempFk[l][1]<<" "<<tempFk[l][2]<<endl;
                            int fkColNo = getColNo(tableName, tempFk[l][0]);
                            //cout<<fkColNo<<endl;

                            if(find(colNo.begin(), colNo.end(), fkColNo) != colNo.end()){
                                string currValue = tupleToUpdate[fkColNo];
                                //cout<<currValue<<endl;
                                vector<string> referencingTableCol = getCol(tempFk[l][1],tempFk[l][2]);
                                if(find(referencingTableCol.begin() , referencingTableCol.end(), currValue) == referencingTableCol.end()){
                                    cout<<"ERROR: Referential Intergrity constraint violated!"<<endl;
                                    table.close();
                                    return;
                                }
                            }
                        }
                        
                        //2. referencing table
                        vector<vector<string>> tempRt = getRt(tableName);
                        for(int l = 0 ; l < tempRt.size() ; l++){
                            //cout<<tempRt[l][0]<<" "<<tempRt[l][1]<<" "<<tempRt[l][2]<<endl;
                            int rtColNo = getColNo(tableName, tempRt[l][0]);

                            if(find(colNo.begin(), colNo.end(), rtColNo) != colNo.end()){
                                vector<string> rtTempCol = getCol(tempRt[l][1],tempRt[l][2]);
                                if(find(rtTempCol.begin(), rtTempCol.end(), tupleToUpdateTemp[rtColNo]) != rtTempCol.end()){
                                    cout<<"ERROR : Referential integrity constraint violated!"<<endl;
                                    table.close();
                                    return;
                                }
                            }
                        }

                        string updatedTuple = mergeIntoString(tupleToUpdate,"#");
                        updatedTuples.push_back(updatedTuple);
                    }else{
                        updatedTuples.push_back(tuple);
                    }
                    count++;
                }
            }
            table.close();

            // for(int i = 0 ; i < updatedTuples.size() ; i++){
            //     cout<<updatedTuples[i]<<endl;
            // }

            t = "db/tables/" + tableName + ".txt";
            table.open(t, ios::out);
            if(table){
                for(int i = 0 ; i < updatedTuples.size() ; i++){
                    table<<updatedTuples[i]<<"\n";
                }
            }else{
                cout<<"ERROR: Error in updating records from table!"<<endl;
            }
            table.close();
            cout<<noOfUpdates<<" record(s) updated from '"<<tableName<<"'."<<endl;

        }else{
            cout<<"ERROR: Command not found...Try Agian!"<<endl;
        }
    }else{
        cout<<"ERROR: table does not exist"<<endl;
    }
}


int getColNo(string tName,string attribute){
    getSchema(tName);
    for(int i = 0 ; i < tableAttributes.size() ; i++){
        if(tableAttributes[i].first == attribute){
            return i;
        }
    }
    return -1;
}

vector<string> getCol(string tName, string attribute){
    vector<string> column;
    string t = "db/tables/" + tName + ".txt";
    table1.open(t, ios::in);
    if(table1){
        int colNumber = getColNo(tName,attribute);
        string tuple;
        while(getline(table1,tuple)){
            vector<string> splittedTuple =  splitString(tuple,"#"); 
            column.push_back(splittedTuple[colNumber]);
        }
    }
    table1.close();
    return column;
}

vector<int> relationalOperationN(string conAttribute, string optr, double value){
    vector<int> boolVector;
    vector<string> column = getCol(tableName,conAttribute);

    if(optr == "="){
        for(int i=0 ; i<column.size() ; i++){
            if(stod(column[i]) == value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "!="){
        for(int i=0 ; i<column.size() ; i++){
            if(stod(column[i]) != value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == ">"){
        for(int i=0 ; i<column.size() ; i++){
            if(stod(column[i]) > value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "<"){
        for(int i=0 ; i<column.size() ; i++){
            if(stod(column[i]) < value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == ">="){
        for(int i=0 ; i<column.size() ; i++){
            if(stod(column[i]) >= value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "<="){
        for(int i=0 ; i<column.size() ; i++){
            if(stod(column[i]) <= value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }

    return boolVector;
}

vector<int> relationalOperationSV(string conAttribute, string optr, string value){
    vector<int> boolVector;
    vector<string> column = getCol(tableName,conAttribute);

    if(optr == "="){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] == value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "!="){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] != value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }

    return boolVector;
}

vector<int> relationalOperationS(string conAttribute, string optr, string value){
    vector<int> boolVector;
    vector<string> column = getCol(tableName,conAttribute);
    vector<string> column1 = getCol(tableName,value);

    if(optr == "="){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] == column1[i]){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "!="){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] != column1[i]){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == ">"){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] > column1[i]){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "<"){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] < column1[i]){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == ">="){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] >= column1[i]){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "<="){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] <= column1[i]){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }

    return boolVector;
}

int getColNo_J(string tName,string attribute){
    for(int i = 0 ; i < tableAttributesJ.size() ; i++){
        if(tableAttributesJ[i].first == attribute){
            return i;
        }
    }
    return -1;
}

vector<string> getCol_J(string tName, string attribute){
    vector<string> column;
    string t = "db/tempFiles/tempTables/" + tName + ".txt";
    table1.open(t, ios::in);
    if(table1){
        int colNumber = getColNo_J(tName,attribute);
        string tuple;
        while(getline(table1,tuple)){
            vector<string> splittedTuple =  splitString(tuple,"#"); 
            column.push_back(splittedTuple[colNumber]);
        }
    }
    table1.close();
    return column;
}

vector<int> relationalOperationN_J(string conAttribute, string optr, double value){
    vector<int> boolVector;
    vector<string> column = getCol_J(tableNameJ,conAttribute);

    if(optr == "="){
        for(int i=0 ; i<column.size() ; i++){
            if(stod(column[i]) == value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "!="){
        for(int i=0 ; i<column.size() ; i++){
            if(stod(column[i]) != value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == ">"){
        for(int i=0 ; i<column.size() ; i++){
            if(stod(column[i]) > value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "<"){
        for(int i=0 ; i<column.size() ; i++){
            if(stod(column[i]) < value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == ">="){
        for(int i=0 ; i<column.size() ; i++){
            if(stod(column[i]) >= value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "<="){
        for(int i=0 ; i<column.size() ; i++){
            if(stod(column[i]) <= value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }

    return boolVector;
}

vector<int> relationalOperationSV_J(string conAttribute, string optr, string value){
    vector<int> boolVector;
    vector<string> column = getCol_J(tableNameJ,conAttribute);

    if(optr == "="){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] == value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "!="){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] != value){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }

    return boolVector;
}

vector<int> relationalOperationS_J(string conAttribute, string optr, string value){
    vector<int> boolVector;
    vector<string> column = getCol_J(tableNameJ,conAttribute);
    // cout<<tableNameJ<<" "<<conAttribute<<endl;
    // for(int i = 0 ; i < column.size() ; i++){
    //     cout<<column[i]<<endl;
    // }
    vector<string> column1 = getCol_J(tableNameJ,value);
    // cout<<tableNameJ<<" "<<value<<endl;
    // for(int i = 0 ; i < column1.size() ; i++){
    //     cout<<column1[i]<<endl;
    // }

    if(optr == "="){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] == column1[i]){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "!="){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] != column1[i]){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == ">"){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] > column1[i]){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "<"){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] < column1[i]){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == ">="){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] >= column1[i]){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }else if(optr == "<="){
        for(int i=0 ; i<column.size() ; i++){
            if(column[i] <= column1[i]){
                boolVector.push_back(1);
            }else{
                boolVector.push_back(0);
            }
        }
    }

    return boolVector;
}

vector<int> logicalOperation(vector<int> boolVector1 , vector<int> boolVector2 , string optr){
    vector<int> ans;
    if(optr == "AND"){
        for(int i = 0 ; i < boolVector1.size() ; i++){
            ans.push_back((boolVector1[i] & boolVector2[i]));
        }
    }else if(optr == "OR"){
        for(int i = 0 ; i < boolVector1.size() ; i++){
            ans.push_back((boolVector1[i] | boolVector2[i]));
        }
    }
    return ans;
}

void createSchemaJ(string tName1, string tName2){
    string t = "db/tempFiles/tempSchema.txt";
    schemasJ.open(t,ios::app);
    if(schemasJ){
        string tempSchema = tName1 + "#" + tName2 + "#"; 
        getSchema(tName1);
        for(auto k : tableAttributes){
            tempSchema = tempSchema + k.first + "#" + k.second + "#";
        }
        getSchema(tName2);
        for(auto k : tableAttributes){
            tempSchema = tempSchema + k.first + "#" + k.second + "#";
        }
        tempSchema = tempSchema.substr(0,tempSchema.size()-1);
        //cout<<tempSchema<<endl;
        
        schemasJ<<tempSchema<<endl;

        tableAttributesJ.clear();
        vector<string> splitTempSchema = splitString(tempSchema, "#");
        for(int i = 2 ; i < splitTempSchema.size() ; i = i + 2){
            tableAttributesJ.push_back(make_pair(splitTempSchema[i],splitTempSchema[i+1]));
        }
        schemasJ.close();
    }else{
        cout<<"ERROR: Unable to create tempSchema!"<<endl;
    }
}

void createTableJ(string tName1, string tName2){
    vector<string> tupleFromT1;
    vector<string> tupleFromT2;
    vector<string> crossProduct;

    string t = "db/tables/"+tName1+".txt";
    table.open(t,ios::in);
    if(table){
        string tuple;
        while(getline(table,tuple)){
            tupleFromT1.push_back(tuple);
        }
    }
    table.close();

    t = "db/tables/"+tName2+".txt";
    table.open(t,ios::in);
    if(table){
        string tuple;
        while(getline(table,tuple)){
            tupleFromT2.push_back(tuple);
        }
    }
    table.close();

    for(int i = 0 ; i < tupleFromT1.size() ; i++){
        for(int j = 0 ; j < tupleFromT2.size() ; j++){
            crossProduct.push_back(tupleFromT1[i] + "#" + tupleFromT2[j]); 
        }
    }

    t = "db/tempFiles/tempTables/" + tName1 + "_" + tName2 + ".txt";
    table.open(t,ios::out);
    if(table){
        for(int i = 0 ; i < crossProduct.size() ; i++){
            table<<crossProduct[i]<<endl;
        }
    }
    table.close();    
}

void selectFromTable(){
    vector<string> attributeList;
    vector<string> tableList;
    if(find(parsed_command.begin(), parsed_command.end(), "JOIN") == parsed_command.end()){
        if(parsed_command[1] == "*" && parsed_command.size() == 5){
            if(parsed_command[2] == "FROM"){
                tableName = parsed_command[3];
                if(isPresent(tableName)){
                    getSchema(tableName);
                    cout<<"\n";
                    for(auto k : tableAttributes){
                        cout<<left<<setw(nameWidth)<<setfill(separator)<<k.first;
                    }
                    cout<<"\n";

                    string t = "db/tables/" + tableName + ".txt";
                    table.open(t, ios::in);
                    if(table){
                        string tuple;
                        while(getline(table,tuple)){
                            //cout<<tuple<<endl;
                            string attributeValue;
                            int i = 0;
                            while(i < tuple.size()){
                                if(tuple[i] == '#'){
                                    cout<<left<<setw(nameWidth)<<setfill(separator)<<attributeValue;
                                    attributeValue = "";
                                    i++;
                                    continue;
                                }
                                attributeValue = attributeValue + tuple[i];
                                i++;
                            }
                            cout<<attributeValue;
                            attributeValue = "";
                            cout<<"\n";
                        }
                    }
                    table.close();
                }else{
                    cout<<"ERROR: table does not exist"<<endl;
                }
            }else{
                cout<<"ERROR: Command not found...Try Agian!"<<endl;
            }
        }else if(parsed_command[1] == "*" && parsed_command.size() > 5){
            if(parsed_command[2] == "FROM"){
                tableName = parsed_command[3];
                if(isPresent(tableName)){
                    if(parsed_command[4] == "WHERE"){
                        vector<string> logicalOperator;
                        vector<vector<int>> boolVectors;
                        vector<int> subBoolVector;
                        vector<int> finalBoolVector;

                        int i = 5;
                        while(true){
                            string conAttribute1 = parsed_command[i++];
                            string optr = parsed_command[i++];
                            string value = parsed_command[i++];
                            subBoolVector.clear();

                            if(is_number(value)){
                                double intValue = stod(value);
                                subBoolVector = relationalOperationN(conAttribute1,optr,intValue);
                            }else if(value[0] == '\''){
                                string strValue = value.substr(1,value.size()-2);
                                subBoolVector = relationalOperationSV(conAttribute1,optr,strValue);
                            }else{
                                subBoolVector = relationalOperationS(conAttribute1,optr,value);
                            }
                            
                            boolVectors.push_back(subBoolVector);

                            if(parsed_command[i] == ";"){
                                break;
                            }else{
                                logicalOperator.push_back(parsed_command[i++]);
                            }
                        }

                        //Printing individual bool vectors
                        // cout<<"Individual Bool Vectors : "<<endl;
                        // for(int i = 0 ; i < boolVectors.size() ; i++){
                        //     for(int j = 0 ; j < boolVectors[i].size() ; j++){
                        //         cout<<boolVectors[i][j]<<" ";
                        //     }
                        //     cout<<"\n";
                        // }
                        // cout<<"\n";

                        if(boolVectors.size() == 1){
                            finalBoolVector = boolVectors[0];
                        }else{
                            finalBoolVector = boolVectors[0];
                            for(int i = 1 ; i < boolVectors.size() ; i++){       
                                finalBoolVector = logicalOperation(finalBoolVector,boolVectors[i],logicalOperator[i-1]);
                            }
                        }

                        //Printing final bool vector
                        // cout<<"Final Bool Vector : "<<endl;
                        // for(int i = 0 ; i < finalBoolVector.size() ; i++){
                        //     cout<<finalBoolVector[i]<<" ";
                        // }
                        // cout<<"\n";

                        getSchema(tableName);
                        cout<<"\n";
                        for(auto k : tableAttributes){
                            cout<<left<<setw(nameWidth)<<setfill(separator)<<k.first;
                        }
                        cout<<"\n";

                        string t = "db/tables/" + tableName + ".txt";
                        table.open(t, ios::in);
                        if(table){
                            string tuple;
                            int count = 0;
                            while(getline(table,tuple)){
                                //cout<<tuple<<endl;
                                if(finalBoolVector[count]){
                                    string attributeValue;
                                    int i = 0;
                                    while(i < tuple.size()){
                                        if(tuple[i] == '#'){
                                            cout<<left<<setw(nameWidth)<<setfill(separator)<<attributeValue;
                                            attributeValue = "";
                                            i++;
                                            continue;
                                        }
                                        attributeValue = attributeValue + tuple[i];
                                        i++;
                                    }
                                    cout<<attributeValue;
                                    attributeValue = "";
                                    cout<<"\n";
                                }
                                count++;
                            }
                        }
                        table.close();
                    }else{
                        cout<<"ERROR: Command not found...Try Agian!"<<endl;
                    }
                }else{
                    cout<<"ERROR: table does not exist"<<endl;
                }
            }else{
                cout<<"ERROR: Command not found...Try Agian!"<<endl;
            }
        }else if(parsed_command.size() == 5){
            if(parsed_command[2] == "FROM"){
                tableName = parsed_command[3];
                if(isPresent(tableName)){
                    cout<<"\n";
                    attributeList = splitString(parsed_command[1], ",");
                    for(auto k : attributeList){
                        cout<<left<<setw(nameWidth)<<setfill(separator)<<k;
                    }
                    cout<<"\n";

                    getSchema(tableName);
                    vector<int> colNo;
                    for(int j = 0 ; j < attributeList.size() ; j++){
                        for(int i = 0 ; i < tableAttributes.size() ; i++){
                            if(tableAttributes[i].first == attributeList[j]){
                                colNo.push_back(i);
                            }
                        }
                    }
                    for(int i = 0 ; i < colNo.size() ; i++){
                        //cout<<colNo[i]<<endl;
                    }

                    string t = "db/tables/" + tableName + ".txt";
                    table.open(t, ios::in);
                    if(table){
                        string tuple;
                        while(getline(table,tuple)){
                            vector<string> splittedTuple =  splitString(tuple,"#"); 
                            for(int i = 0 ; i < colNo.size() ; i++){
                                cout<<left<<setw(nameWidth)<<setfill(separator)<<splittedTuple[colNo[i]];
                            }
                            cout<<"\n"; 
                        }
                    }
                    table.close();
                    
                }else{
                    cout<<"table does not exist"<<endl;
                }
            }
        }else if(parsed_command.size() > 5){
        if(parsed_command[2] == "FROM"){
            tableName = parsed_command[3];
            if(isPresent(tableName)){
                if(parsed_command[4] == "WHERE"){
                    cout<<"\n";
                    attributeList = splitString(parsed_command[1], ",");
                    for(auto k : attributeList){
                        cout<<left<<setw(nameWidth)<<setfill(separator)<<k;
                    }
                    cout<<"\n";

                    getSchema(tableName);
                    vector<int> colNo;
                    for(int j = 0 ; j < attributeList.size() ; j++){
                        for(int i = 0 ; i < tableAttributes.size() ; i++){
                            if(tableAttributes[i].first == attributeList[j]){
                                colNo.push_back(i);
                            }
                        }
                    }
                    //Printing the col no.
                    // for(int i = 0 ; i < colNo.size() ; i++){
                    //     cout<<colNo[i]<<endl;
                    // }

                    vector<string> logicalOperator;
                    vector<vector<int>> boolVectors;
                    vector<int> subBoolVector;
                    vector<int> finalBoolVector;

                    int i = 5;
                    while(true){
                        string conAttribute1 = parsed_command[i++];
                        string optr = parsed_command[i++];
                        string value = parsed_command[i++];
                        subBoolVector.clear();

                        if(is_number(value)){
                            double intValue = stod(value);
                            subBoolVector = relationalOperationN(conAttribute1,optr,intValue);
                        }else if(value[0] == '\''){
                            string strValue = value.substr(1,value.size()-2);
                            subBoolVector = relationalOperationSV(conAttribute1,optr,strValue);
                        }else{
                            subBoolVector = relationalOperationS(conAttribute1,optr,value);
                        }
                        
                        boolVectors.push_back(subBoolVector);

                        if(parsed_command[i] == ";"){
                            break;
                        }else{
                            logicalOperator.push_back(parsed_command[i++]);
                        }
                    }

                    //Printing individual bool vectors
                    // cout<<"Individual Bool Vectors : "<<endl;
                    // for(int i = 0 ; i < boolVectors.size() ; i++){
                    //     for(int j = 0 ; j < boolVectors[i].size() ; j++){
                    //         cout<<boolVectors[i][j]<<" ";
                    //     }
                    //     cout<<"\n";
                    // }
                    // cout<<"\n";

                    if(boolVectors.size() == 1){
                        finalBoolVector = boolVectors[0];
                    }else{
                        finalBoolVector = boolVectors[0];
                        for(int i = 1 ; i < boolVectors.size() ; i++){       
                            finalBoolVector = logicalOperation(finalBoolVector,boolVectors[i],logicalOperator[i-1]);
                        }
                    }

                    //Printing final bool vector
                    // cout<<"Final Bool Vector : "<<endl;
                    // for(int i = 0 ; i < finalBoolVector.size() ; i++){
                    //     cout<<finalBoolVector[i]<<" ";
                    // }
                    // cout<<"\n";

                    string t = "db/tables/" + tableName + ".txt";
                    table.open(t, ios::in);
                    if(table){
                        string tuple;
                        int count = 0;
                        while(getline(table,tuple)){
                            if(finalBoolVector[count]){
                                vector<string> splittedTuple =  splitString(tuple,"#"); 
                                for(int i = 0 ; i < colNo.size() ; i++){
                                    cout<<left<<setw(nameWidth)<<setfill(separator)<<splittedTuple[colNo[i]];
                                }
                                cout<<"\n";
                            }
                            count++;
                        }
                    }
                    table.close();

                }
            }else{
                cout<<"ERROR: table does not exist"<<endl;
            }
        }
    }
    }else{
        if(parsed_command[1] == "*" && parsed_command.size() == 7){
            if(parsed_command[2] == "FROM"){
                if(parsed_command[4] == "JOIN"){
                    vector<string> combinedAttributes;
                    string tableName1 = parsed_command[3];
                    string tableName2 = parsed_command[5];
                    
                    if(!isPresent(tableName1)){
                        cout<<"ERROR: table does not exist!"<<endl;
                        return;
                    }
                    if(!isPresent(tableName2)){
                        cout<<"ERROR: table does not exist!"<<endl;
                        return;
                    }

                    createSchemaJ(tableName1,tableName2);
                    cout<<endl;
                    for(auto k : tableAttributesJ){
                        cout<<left<<setw(nameWidth)<<setfill(separator)<<k.first;
                    }
                    cout<<endl;

                    createTableJ(tableName1,tableName2);

                    string t = "db/tempFiles/tempTables/" + tableName1 + "_" + tableName2 + ".txt";
                    table.open(t, ios::in);
                    if(table){
                        string tuple;
                        while(getline(table,tuple)){
                            //cout<<tuple<<endl;
                            string attributeValue;
                            int i = 0;
                            while(i < tuple.size()){
                                if(tuple[i] == '#'){
                                    cout<<left<<setw(nameWidth)<<setfill(separator)<<attributeValue;
                                    attributeValue = "";
                                    i++;
                                    continue;
                                }
                                attributeValue = attributeValue + tuple[i];
                                i++;
                            }
                            cout<<attributeValue;
                            attributeValue = "";
                            cout<<"\n";
                        }
                    }
                    table.close();
                    
                }else{
                    cout<<"ERROR: Command not found...Try Agian!"<<endl;
                }
            }else{
                cout<<"ERROR: Command not found...Try Agian!"<<endl;
            }
        }else if(parsed_command[1] == "*" && parsed_command.size() > 7){
            if(parsed_command[2] == "FROM"){
                if(parsed_command[4] == "JOIN" && parsed_command[6] == "WHERE"){
                    string tableName1 = parsed_command[3];
                    string tableName2 = parsed_command[5];
                    
                    if(!isPresent(tableName1)){
                        cout<<"ERROR: table does not exist!"<<endl;
                        return;
                    }
                    if(!isPresent(tableName2)){
                        cout<<"ERROR: table does not exist!"<<endl;
                        return;
                    }

                    createSchemaJ(tableName1,tableName2);
                    cout<<endl;
                    for(auto k : tableAttributesJ){
                        cout<<left<<setw(nameWidth)<<setfill(separator)<<k.first;
                    }
                    cout<<endl;

                    createTableJ(tableName1,tableName2);
                    tableNameJ = tableName1 + "_" + tableName2;

                    vector<string> logicalOperator;
                    vector<vector<int>> boolVectors;
                    vector<int> subBoolVector;
                    vector<int> finalBoolVector;

                    int i = 7;
                    while(true){
                        string conAttribute1 = parsed_command[i++];
                        string optr = parsed_command[i++];
                        string value = parsed_command[i++];
                        subBoolVector.clear();

                        if(is_number(value)){
                            double intValue = stod(value);
                            subBoolVector = relationalOperationN_J(conAttribute1,optr,intValue);
                        }else if(value[0] == '\''){
                            string strValue = value.substr(1,value.size()-2);
                            subBoolVector = relationalOperationSV_J(conAttribute1,optr,strValue);
                        }else{
                            //cout<<"In here : "<<conAttribute1<<" "<<optr<<" "<<value<<endl;
                            subBoolVector = relationalOperationS_J(conAttribute1,optr,value);
                        }
                            
                        boolVectors.push_back(subBoolVector);

                        if(parsed_command[i] == ";"){
                            break;
                        }else{
                            logicalOperator.push_back(parsed_command[i++]);
                        }
                    }

                    //Printing individual bool vectors
                    // cout<<"Individual Bool Vectors : "<<endl;
                    // for(int i = 0 ; i < boolVectors.size() ; i++){
                    //     for(int j = 0 ; j < boolVectors[i].size() ; j++){
                    //         cout<<boolVectors[i][j]<<" ";
                    //     }
                    //     cout<<"\n";
                    // }
                    // cout<<"\n";

                    if(boolVectors.size() == 1){
                        finalBoolVector = boolVectors[0];
                    }else{
                        finalBoolVector = boolVectors[0];
                        for(int i = 1 ; i < boolVectors.size() ; i++){       
                            finalBoolVector = logicalOperation(finalBoolVector,boolVectors[i],logicalOperator[i-1]);
                        }
                    }

                    //Printing final bool vector
                    // cout<<"Final Bool Vector : "<<endl;
                    // for(int i = 0 ; i < finalBoolVector.size() ; i++){
                    //     cout<<finalBoolVector[i]<<" ";
                    // }
                    // cout<<"\n";


                    string t = "db/tempFiles/tempTables/" + tableName1 + "_" + tableName2 + ".txt";
                    table.open(t, ios::in);
                    if(table){
                        string tuple;
                        int count = 0;
                        while(getline(table,tuple)){
                            if(finalBoolVector[count] == 1){
                                //cout<<tuple<<endl;
                                string attributeValue;
                                int i = 0;
                                while(i < tuple.size()){
                                    if(tuple[i] == '#'){
                                        cout<<left<<setw(nameWidth)<<setfill(separator)<<attributeValue;
                                        attributeValue = "";
                                        i++;
                                        continue;
                                    }
                                    attributeValue = attributeValue + tuple[i];
                                    i++;
                                }
                                cout<<attributeValue;
                                attributeValue = "";
                                cout<<"\n";
                            }
                            count++;
                        }
                    }
                    table.close();
                    
                }else{
                    cout<<"ERROR: Command not found...Try Agian!"<<endl;
                }
            }else{
                cout<<"ERROR: Command not found...Try Agian!"<<endl;
            }
        }else if(parsed_command.size() == 7){
            if(parsed_command[2] == "FROM"){
                if(parsed_command[4] == "JOIN"){
                    vector<string> combinedAttributes;
                    string tableName1 = parsed_command[3];
                    string tableName2 = parsed_command[5];
                    
                    if(!isPresent(tableName1)){
                        cout<<"ERROR: table does not exist!"<<endl;
                        return;
                    }
                    if(!isPresent(tableName2)){
                        cout<<"ERROR: table does not exist!"<<endl;
                        return;
                    }

                    createSchemaJ(tableName1,tableName2);
                    vector<string> attributeList;
                    cout<<endl;
                    attributeList = splitString(parsed_command[1], ",");
                    for(auto k : attributeList){
                        cout<<left<<setw(nameWidth)<<setfill(separator)<<k;
                    }
                    cout<<endl;

                    vector<int> colNo;
                    for(int j = 0 ; j < attributeList.size() ; j++){
                        for(int i = 0 ; i < tableAttributesJ.size() ; i++){
                            if(tableAttributesJ[i].first == attributeList[j]){
                                colNo.push_back(i);
                            }
                        }
                    }

                    createTableJ(tableName1,tableName2);

                    string t = "db/tempFiles/tempTables/" + tableName1 + "_" + tableName2 + ".txt";
                    table.open(t, ios::in);
                    if(table){
                        string tuple;
                        while(getline(table,tuple)){
                            vector<string> splittedTuple =  splitString(tuple,"#"); 
                            for(int i = 0 ; i < colNo.size() ; i++){
                                cout<<left<<setw(nameWidth)<<setfill(separator)<<splittedTuple[colNo[i]];
                            }
                            cout<<"\n"; 
                        }
                    }
                    table.close();
                    
                }else{
                    cout<<"ERROR: Command not found...Try Agian!"<<endl;
                }
            }else{
                cout<<"ERROR: Command not found...Try Agian!"<<endl;
            }
        }else if(parsed_command.size() > 7){
            if(parsed_command[2] == "FROM"){
                if(parsed_command[4] == "JOIN" && parsed_command[6] == "WHERE"){
                    string tableName1 = parsed_command[3];
                    string tableName2 = parsed_command[5];
                    
                    if(!isPresent(tableName1)){
                        cout<<"ERROR: table does not exist!"<<endl;
                        return;
                    }
                    if(!isPresent(tableName2)){
                        cout<<"ERROR: table does not exist!"<<endl;
                        return;
                    }

                    createSchemaJ(tableName1,tableName2);
                    cout<<endl;
                    vector<string> attributeList = splitString(parsed_command[1], ",");
                    for(auto k : attributeList){
                        cout<<left<<setw(nameWidth)<<setfill(separator)<<k;
                    }
                    cout<<endl;

                    vector<int> colNo;
                    for(int j = 0 ; j < attributeList.size() ; j++){
                        for(int i = 0 ; i < tableAttributesJ.size() ; i++){
                            if(tableAttributesJ[i].first == attributeList[j]){
                                colNo.push_back(i);
                            }
                        }
                    }

                    createTableJ(tableName1,tableName2);
                    tableNameJ = tableName1 + "_" + tableName2;

                    vector<string> logicalOperator;
                    vector<vector<int>> boolVectors;
                    vector<int> subBoolVector;
                    vector<int> finalBoolVector;

                    int i = 7;
                    while(true){
                        string conAttribute1 = parsed_command[i++];
                        string optr = parsed_command[i++];
                        string value = parsed_command[i++];
                        subBoolVector.clear();

                        if(is_number(value)){
                            double intValue = stod(value);
                            subBoolVector = relationalOperationN_J(conAttribute1,optr,intValue);
                        }else if(value[0] == '\''){
                            string strValue = value.substr(1,value.size()-2);
                            subBoolVector = relationalOperationSV_J(conAttribute1,optr,strValue);
                        }else{
                            //cout<<"In here : "<<conAttribute1<<" "<<optr<<" "<<value<<endl;
                            subBoolVector = relationalOperationS_J(conAttribute1,optr,value);
                        }
                            
                        boolVectors.push_back(subBoolVector);

                        if(parsed_command[i] == ";"){
                            break;
                        }else{
                            logicalOperator.push_back(parsed_command[i++]);
                        }
                    }

                    //Printing individual bool vectors
                    // cout<<"Individual Bool Vectors : "<<endl;
                    // for(int i = 0 ; i < boolVectors.size() ; i++){
                    //     for(int j = 0 ; j < boolVectors[i].size() ; j++){
                    //         cout<<boolVectors[i][j]<<" ";
                    //     }
                    //     cout<<"\n";
                    // }
                    // cout<<"\n";

                    if(boolVectors.size() == 1){
                        finalBoolVector = boolVectors[0];
                    }else{
                        finalBoolVector = boolVectors[0];
                        for(int i = 1 ; i < boolVectors.size() ; i++){       
                            finalBoolVector = logicalOperation(finalBoolVector,boolVectors[i],logicalOperator[i-1]);
                        }
                    }

                    //Printing final bool vector
                    // cout<<"Final Bool Vector : "<<endl;
                    // for(int i = 0 ; i < finalBoolVector.size() ; i++){
                    //     cout<<finalBoolVector[i]<<" ";
                    // }
                    // cout<<"\n";


                    string t = "db/tempFiles/tempTables/" + tableName1 + "_" + tableName2 + ".txt";
                    table.open(t, ios::in);
                    if(table){
                        string tuple;
                        int count = 0;
                        while(getline(table,tuple)){
                            if(finalBoolVector[count] == 1){
                                vector<string> splittedTuple =  splitString(tuple,"#"); 
                                for(int i = 0 ; i < colNo.size() ; i++){
                                    cout<<left<<setw(nameWidth)<<setfill(separator)<<splittedTuple[colNo[i]];
                                }
                                cout<<"\n";
                            }
                            count++;
                        }
                    }
                    table.close();
                    
                }else{
                    cout<<"ERROR: Command not found...Try Agian!"<<endl;
                }
            }else{
                cout<<"ERROR: Command not found...Try Agian!"<<endl;
            }
        }else{
            cout<<"ERROR: Command not found....Try again!"<<endl;
        }
    }
}


void getSchema(string tName){
    schemas.open("db/schemas.txt",ios::in);
    string schema;
    string tempSchema;
    while(getline(schemas,schema)){
        int i = 0;
        string tempTableName;
        while(schema[i] != '#'){
            tempTableName = tempTableName + schema[i];
            i++;
        }
        //cout<<tempTableName<<endl;
        if(tempTableName == tName){
            tempSchema = schema;
            break;
        }
    }
    schemas.close();

    tableAttributes.clear();
    vector<string> splitStr = splitString(tempSchema , "#");
    for(int i = 1 ; i < splitStr.size() ; i++){
        //cout<<splitStr[i]<<"-"<<splitStr[i+1]<<" ";
        tableAttributes.push_back(make_pair(splitStr[i],splitStr[i+1]));
        i++;
    }
}

void getNoOfRecords(){
    string t = "db/tables/" + tableName + ".txt";
    table.open(t, ios::in);
    if(table){
        string tuple;
        noOfRecords = 0;
        while(getline(table,tuple)){
            noOfRecords++;
        }
    }
    table.close();
}

void describeTable(){
    if(parsed_command[1] == "TABLE"){
        tableName = parsed_command[2];
        if(isPresent(tableName)){
            getSchema(tableName);
            cout<<"\n";
            int count = 0;
            vector<string> constraints = getAllConstraints(tableName);
            cout<<left<<setw(16)<<setfill(separator)<<"RELATION_NAME"<<left<<setw(16)<<setfill(separator)<<"ATTRIBUTE_NAME"<<left<<setw(16)<<setfill(separator)<<"ATTRIBUTE_TYPE"<<"CONSTRAINTS"<<endl;
            for(auto k : tableAttributes){
                cout<<left<<setw(16)<<setfill(separator)<<tableName<<left<<setw(16)<<setfill(separator)<<k.first<<left<<setw(16)<<setfill(separator)<<k.second<<constraints[count++]<<endl;
            }
        }else{
            cout<<"ERROR: table does not exist"<<endl;
        }
    }else{
        cout<<"ERROR: Command not found...Try Agian!"<<endl;
    }
}


void dropTable(){
    if(parsed_command[1] == "TABLE"){
        tableName = parsed_command[2];

        vector<vector<string>> rt = getRt(tableName);
        // for(int i = 0 ; i < rt.size() ; i++){
        //     cout<<rt[i]<<" ";
        // }
        // cout<<endl;

        if(rt.size() != 0){
            cout<<"ERROR: Can't drop the table as foreign key constraint is violated!"<<endl;
            return;
        }

        if(isPresent(tableName)){

            vector<vector<string>> fk = getFk(tableName);
            if(fk.size() != 0){
                for(int i = 0 ; i < fk.size() ; i++){
                    //cout<<fk[i][0]<<" "<<fk[i][1]<<" "<<fk[i][2]<<endl;

                    string t = "db/tableContraints/" + fk[i][1] + ".txt";
                    table.open(t, ios::in);
                    vector<string> others;
                    if(table){
                        string line;
                        while(getline(table,line)){
                            if(line[0] == 'R' && line[1] == 'T' && line[2] == ':'){
                                vector<string> temp = splitString(line," ");
                                if(temp[2] != tableName){
                                    others.push_back(line);
                                }
                            }else{
                                others.push_back(line);
                            }
                        }
                    }
                    table.close();

                    t = "db/tableContraints/" + fk[i][1] + ".txt";
                    table.open(t, ios::out);
                    if(table){
                        for(int i = 0 ; i < others.size() ; i++){
                            table<<others[i]<<endl;
                        }
                    }
                    table.close();
                }
            }

            //Dropping schema
            schemas.open("db/schemas.txt",ios::in);
            string schema;
            vector<string> tempSchema;
            while(getline(schemas,schema)){
                int i = 0;
                string tempTableName;
                while(schema[i] != '#'){
                    tempTableName = tempTableName + schema[i];
                    i++;
                }
                //cout<<tempTableName<<endl;
                if(tempTableName != tableName){
                    tempSchema.push_back(schema);
                }
            }
            schemas.close();

            schemas.open("db/schemas.txt",ios::out);
            for(int i = 0 ; i < tempSchema.size() ; i++){
                schemas<<tempSchema[i]<<"\n";
            }
            schemas.close();

            // //Dropping table
            // string t = "db/tables/" + tableName + ".txt";
            // int n = t.length();
            // char *t_char = new char(n+1);
            // strcpy(t_char, t.c_str());
            // //cout<<t_char<<endl;
            // int r1 = remove(t_char);
            // //cout<<r1<<endl;

            // t = "db/tableContraints/" + tableName + ".txt";
            // n = t.length();
            // char *t_char1 = new char(n+1);
            // strcpy(t_char1, t.c_str());
            // //cout<<t_char1<<endl;
            // int r2 = remove(t_char1);
            // //cout<<r2<<endl;

            string file1_cpp = "db/tables/" + tableName + ".txt";
            char *a = &(file1_cpp[0]); 
            int r1 = remove(a);
            //cout<<r1<<endl;

            string file2_cpp = "db/tableContraints/" + tableName + ".txt";
            char *b = &(file2_cpp[0]); 
            int r2 = remove(b);
            //cout<<r2<<endl;

            cout<<"Table '"<<tableName<<"' dropped successfully!"<<endl;
            //cout<<"In here"<<endl;
        }else{
            cout<<"ERROR: table does not exist"<<endl;
        }
    }else{
        cout<<"ERROR: Command not found...Try Agian!"<<endl;
    }
}


void helpTable(){
    if(parsed_command[1] == "TABLES"){
        schemas.open("db/schemas.txt",ios::in);
        string schema;
        cout<<"\nList of tables present in database : "<<endl;
        while(getline(schemas,schema)){
            int i = 0;
            string tempTableName;
            while(schema[i] != '#'){
                tempTableName = tempTableName + schema[i];
                i++;
            }
            cout<<tempTableName<<endl;
        }
        schemas.close();
    }else if(parsed_command[2] == "CMD"){
        cout<<"ERROR: Work in progress..."<<endl;
    }else{
        cout<<"ERROR: Command not found...Try Agian!"<<endl;
    }
}

void helpCmd(){
    if(parsed_command[1] == "CREATE" && parsed_command[2] == "TABLE"){
        string t = "db/helpCmd/CREATE TABLE.txt";
        help.open(t,ios::in);
        if(help){
            string line;
            while(getline(help,line)){
                cout<<line<<endl;
            }
        }else{
            cout<<"ERROR: Unable to access the file!"<<endl;
        }
        help.close();
    }else if(parsed_command[1] == "DROP" && parsed_command[2] == "TABLE"){
        string t = "db/helpCmd/DROP TABLE.txt";
        help.open(t,ios::in);
        if(help){
            string line;
            while(getline(help,line)){
                cout<<line<<endl;
            }
        }else{
            cout<<"ERROR: Unable to access the file!"<<endl;
        }
        help.close();
    }else if(parsed_command[1] == "SELECT"){
        string t = "db/helpCmd/SELECT.txt";
        help.open(t,ios::in);
        if(help){
            string line;
            while(getline(help,line)){
                cout<<line<<endl;
            }
        }else{
            cout<<"ERROR: Unable to access the file!"<<endl;
        }
        help.close();
    }else if(parsed_command[1] == "INSERT"){
        string t = "db/helpCmd/INSERT.txt";
        help.open(t,ios::in);
        if(help){
            string line;
            while(getline(help,line)){
                cout<<line<<endl;
            }
        }else{
            cout<<"ERROR: Unable to access the file!"<<endl;
        }
        help.close();
    }else if(parsed_command[1] == "DELETE"){
        string t = "db/helpCmd/DELETE.txt";
        help.open(t,ios::in);
        if(help){
            string line;
            while(getline(help,line)){
                cout<<line<<endl;
            }
        }else{
            cout<<"ERROR: Unable to access the file!"<<endl;
        }
        help.close();
    }else if(parsed_command[1] == "UPDATE"){
        string t = "db/helpCmd/UPDATE.txt";
        help.open(t,ios::in);
        if(help){
            string line;
            while(getline(help,line)){
                cout<<line<<endl;
            }
        }else{
            cout<<"ERROR: Unable to access the file!"<<endl;
        }
        help.close();
    }else if(parsed_command[1] == "DATATYPES"){
        string t = "db/helpCmd/DATATYPES.txt";
        help.open(t,ios::in);
        if(help){
            string line;
            while(getline(help,line)){
                cout<<line<<endl;
            }
        }else{
            cout<<"ERROR: Unable to access the file!"<<endl;
        }
        help.close();
    }else{
        cout<<"ERROR: Command not found!....Try again!"<<endl;
    }
}


void quit(){
    cout<<"\n";
    exit(0);
}

                
int main(){
    system("CLS");
    char c;
    while(true){
        string command;
        cout<<"\n>>";
        cin.sync();
        getline(cin, command);
        //cout<<command<<endl;

        stringstream ss(command);
        string word;
        parsed_command.clear();
        while (ss >> word) {
            parsed_command.push_back(word);
        }

        if(parsed_command[0] == "CREATE"){
            createTable();
        } else if(parsed_command[0] == "INSERT"){
            insertIntoTable();
        }else if(parsed_command[0] == "SELECT"){
            selectFromTable();
        }else if(parsed_command[0] == "DELETE"){
            deleteFromTable();
        }else if(parsed_command[0] == "UPDATE"){
            updateTable();
        }else if(parsed_command[0] == "DESCRIBE"){
            describeTable();
        }else if(parsed_command[0] == "DROP"){
            dropTable();
        }else if(parsed_command[0] == "HELP" && parsed_command[1] == "TABLES"){
            helpTable();
        }else if(parsed_command[0] == "HELP"){
            helpCmd();
        }else if(parsed_command[0] == "QUIT"){
            quit();
        }else{
            cout<<"ERROR: Command not found...Try Again!"<<endl;
        }
    }

    return 0;
}