
#include "DEETCTestSet.h"



//////////////////////////////////////////////////////////////
//// DEETC Dataset Methods
////
//////////////////////////////////////////////////////////////


// [Override]
void DEETCTestSet::computeStudentCount() {
    string filename = rootDir + "/" + name + ".stu";

    cout << filename << endl;

    // The number of students is equal to the number of records in '.stu' file
    numStudents = 1238;

    cout << "numStudents = " << numStudents << endl;

}

// [Override]
// Load course names
void DEETCTestSet::loadCourseNames() {
    string filename = rootDir + "/" + name + ".names";

    cout << filename << endl;

    ifstream in(filename.c_str());
    if (!in.is_open())
        throw runtime_error("Could not open file " + filename + " in DEETCTestSet::loadCourseNames() method");

    string line;
    while (getline(in, line)) // Each line is the course name
    {
//        cout << line << endl;
        // Insert entry into the vector
        courseNames.push_back(line);
    }
    cout << "Readed " << courseNames.size() << " names" << endl;
}





