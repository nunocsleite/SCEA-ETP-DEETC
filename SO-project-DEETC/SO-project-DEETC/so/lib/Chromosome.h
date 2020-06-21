#ifndef CHROMOSOME
#define CHROMOSOME

#include "Period.h"
#include <boost/random.hpp>
#include <vector>
#include <boost/unordered_set.hpp>
#include <set> // multiset
#include <cmath>
#include "Data.h"
#include "Matrix.h"
#include <boost/unordered_map.hpp>
#include "Exam.h"
#include "Common.h"
// Armadillo Matrix manipulation library
#include <armadillo>


using namespace std;
using namespace boost;


#define SATURDAY_PERIOD 6


// Chromosome implementation.
// A chromosome encodes a complete and feasible timetable.
// A period contains a list of exams to be held in the period.
////
class Chromosome {

public:

    Chromosome() { }

    Chromosome(const Data& data) {
        init(data);
    }

    // Copy constructor
    Chromosome (Chromosome const& _chrom)
        : originalNumPeriods(_chrom.originalNumPeriods),
          range(_chrom.range),
          periods(_chrom.periods),
          proximityCost(_chrom.proximityCost),
          numberClashes(_chrom.numberClashes),
          numberClashesDEETCProblem(_chrom.numberClashesDEETCProblem),
          conflictMatrix(_chrom.conflictMatrix),
          graph(_chrom.graph),
          numStudents(_chrom.numStudents),
          numExams(_chrom.numExams),
          numEnrolments(_chrom.numEnrolments),
          courseStudentCounts(_chrom.courseStudentCounts),
          courseNames(_chrom.courseNames)
    {
        createExamIndexMap();

    }

    void baseComputeNumClashes();
    int getNumClashesPeriod(int pi) const;

    void baseComputeNumClashesDEETCProblem();
    int getNumClashesPeriodDEETCProblem(int pi) const;

    void init(Data const& data);

    vector<int> getFeasiblePeriods(int exam, int period) const;
    inline int getNumPeriods() const { return periods.size(); }
    inline int getOriginalNumPeriods() const { return originalNumPeriods; }
    inline vector<int> const& getRange() const { return range; }
    inline vector<boost::unordered_set<int> >& getPeriods() { return periods; }
    inline vector<boost::unordered_set<int> > const& getConstPeriods() const { return periods; }
    inline void setPeriods(vector<boost::unordered_set<int> > const& _periods) {
        for (int i = 0; i < _periods.size(); ++i)
            periods[i] = _periods[i];
    }
    // Get Conflict matrix
    Matrix const* getConflictMatrix() { return conflictMatrix; }
    // Get graph
    AdjacencyList const* getGraph() { return graph; }

    inline double getProximityCost() const { return proximityCost; }
    inline double getNumClashes() const { return numberClashes; }
    inline double getNumClashesDEETCProblem() const { return numberClashesDEETCProblem; }
    inline int getNumExams() const { return numExams; }

    //    virtual void computeProximityCosts() = 0;


    int getNumStudentsPeriod(int pi) const;

    void validate() const;
    bool isFeasiblePeriodExam(int period, int exam) const;

    Chromosome const& getChromosome() const { return *this; }

    friend ostream& operator<<(ostream& os, const Chromosome& timetable);

    double getProximityCostExamPeriod(int exami, int pi) const;

    typedef vector<boost::unordered_set<int> >::iterator PeriodIterator;
    typedef vector<boost::unordered_set<int> >::const_iterator ConstPeriodIterator;

    // ADDED 17-june-2014

    int getExamIndex(string const& courseName) const {
        if (examToIndexMap.find(courseName) == examToIndexMap.end()) {
            throw runtime_error("Could not find course name " + courseName + " in examsIndexes");
        }

        return examToIndexMap.at(courseName);
    }
    string getExamName(int courseIndex) const {
        if (courseIndex-1 < 0 || courseIndex-1 >= courseNames->size()) {
            cout << (courseIndex-1) << endl;
            throw runtime_error("Invalid course index in getExamName()");
        }
        return (*courseNames)[courseIndex-1]; // Exams's indexes start at 1
    }

    void createExamIndexMap() {
        // Create exam index map
        for (int ei = 0; ei < courseNames->size(); ++ei) {
            pair<string, int> pairValue((*courseNames)[ei], ei+1); // Exams's indexes start at 1
            examToIndexMap.insert(pairValue);

        }
//        // Print map
//        for (auto it = examToIndexMap.begin(); it != examToIndexMap.end(); ++it)
//            cout << it->first << " - " << it->second << endl;
    }

    int getExamTimeslot(int exam) const;

    void toCSV(string const& filename) const;
    void toCSV(string const& filename, const string programCourseNames[], int numCourses) const;
//    vector<shared_ptr<eoChromosome> > generate(string* programCourseNames[], int programSizes[], int numPrograms);
    vector<Chromosome> generate(string* programCourseNames[], int programSizes[], int numPrograms) const;


//private:
protected:
    // Original number of Periods
    int originalNumPeriods;
    // Range
    vector<int> range;
    // Periods containing exams identifiers
    vector<boost::unordered_set<int> > periods;
    // Number of conflicts
    double proximityCost;
    // Number of clashes
    int numberClashes;
    // Number of clashes DEETC problem
    int numberClashesDEETCProblem;
    // Conflict matrix
    Matrix const* conflictMatrix;
    // Graph
    AdjacencyList const* graph;

    // Number of students
    int numStudents;
    // Number of exams
    int numExams;
    // Number of enrolments
    int numEnrolments;
    // Course student counts
    boost::unordered_map<int, int> const* courseStudentCounts;
    // Course names
    vector<string> const* courseNames;
    // Exam indexs
    boost::unordered_map<string, int> examToIndexMap;

    virtual void baseComputeProximityCosts();

    int getMinPeriod();
    multiset<Period> getExamsAvailablePeriods(const boost::unordered_set<int> &sourceExams, int sourcePeriod);
    void computeAvailablePeriods(boost::unordered_set<int> const& sourceExams, int sourcePeriod, vector<Period>& availablePeriodsVec);
    int computeNumClashesPeriod(int exam, int p) const;
    int computeNumClashesExamPeriod(int exam, int p, int minPeriod) const;
    void removeEmptyPeriods();
};


#endif
