#ifndef TESTSETDESCRIPTION_H
#define TESTSETDESCRIPTION_H

#include <string>

using namespace std;


class TestSetDescription {
    string name;
    string description;
    int numPeriods; /// DELETE
public:
    TestSetDescription(string name, string description, int numPeriods) :
        name(name), description(description), numPeriods(numPeriods) { }


    TestSetDescription(string _name, string _description) :
        name(_name), description(_description) { }

    string getName() const { return name; }
    string getDescription() const { return description; }
//    int getPeriods() const { return 0; }//numPeriods; } /// ????

    int getPeriods() const { return numPeriods; }
    friend ostream& operator<<(ostream& os, const TestSetDescription& t);
};


#endif // TESTSETDESCRIPTION_H
