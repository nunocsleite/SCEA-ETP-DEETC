#ifndef DEETCTESTSET_H
#define DEETCTESTSET_H


#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <sstream>
#include <boost/tokenizer.hpp>
#include "GraphColouringHeuristics.h"
#include "Matrix.h"
#include <boost/unordered_map.hpp>
#include "TestSet.h"
#include "TorontoTestSet.h"

using namespace std;
using namespace boost;



/////////////////////////////
// DEETC Dataset
//
/////////////////////////////
class DEETCTestSet : public TorontoTestSet {

public:
    // Constructor
    DEETCTestSet(string _name, string _desc, Data& _data, string _rootDir)
        : TorontoTestSet(_name, _desc, _data, _rootDir)  {

        cout << "DEETCTestSet ctor" << endl;

        // Load data

        /// ALREADY DONE IN BASE CLASS

//        load(_name, _rootDir);
    }


//protected:
//public:
//    virtual void load(string _testSetName, string _rootDir);

protected:
//    virtual void loadConflictMatrix();
//    virtual void loadCourseCounts();
    virtual void computeStudentCount();
    virtual void loadCourseNames();
};


#endif // DEETCTESTSET_H
