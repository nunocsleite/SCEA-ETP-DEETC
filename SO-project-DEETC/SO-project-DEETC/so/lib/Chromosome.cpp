#include "Chromosome.h"



void Chromosome::init(Data const& data) {
    // Set range
    range = data.getRange();
    // Set original number of Periods
    originalNumPeriods = data.getOriginalNumPeriods();
    // Set conflict matrix
    conflictMatrix = data.getProblemData().getConflictMatrix();
    // Set adjacency list
    graph = data.getProblemData().getGraph();
    // Set number of students
    numStudents = data.getProblemData().getNumStudents();
    // Set number of exams
    numExams = data.getProblemData().getNumExams();
    // Set number of enrolments
    numEnrolments = data.getProblemData().getNumEnrolments();
    // Set student counts
    courseStudentCounts = data.getProblemData().getCourseStudentCounts();
    // Set course names
    courseNames = data.getProblemData().getCourseNames();
    // Create exam index map
    createExamIndexMap();

    proximityCost = 0;
    numberClashes = 0;
    numberClashesDEETCProblem = 0;


    // Generate a random number of periods in the range interval
    int numPeriods = rand() % (data.getRange()[1] - data.getRange()[0] + 1) + data.getRange()[0];
//    int numPeriods = 19;
/// SEE

    //boost::random::mt19937 rng;  // random generator
    //boost::random::uniform_int_distribution<> distribution(data.getRange()[0], data.getRange()[1]); // distribution that maps to getRange()[0]..getRange()[1]
    //numPeriods = distribution(rng);

    periods = vector<boost::unordered_set<int> >(numPeriods);

}



void Chromosome::baseComputeProximityCosts() {
	// The problem has one hard constraint where conflicting exams cannot be assigned to the same time slot. 
	// In addition, a soft constraint is present where conflicting exams should be spread throughout the timetable 
	// as far as possible from each other. The objective here is to minimise the sum of proximity costs given as:
	//    sum_{i=0}^{4} (wi x n) / S
	// where
	// - wi = 2^{4-i} is the cost of assigning two exams with i time slots apart. Only exams with
	//   common students and are four or less time slots apart are considered as violations
	// - n is the number of students involved in the conflict
	// - S is the total number of students in the problem

    proximityCost = 0;
    int pj;
    int wi, n;
    // For each period do
    for (int pi = 0; pi < periods.size(); ++pi) {
        // For each exam do
        for (boost::unordered_set<int>::const_iterator it_i = periods[pi].begin(); it_i != periods[pi].end(); ++it_i) {
            // Consider exams four periods apart
            for (int i = 0; i <= 4; ++i) {
                pj = pi+i+1;
                if (pj < periods.size()) {
                    for (boost::unordered_set<int>::const_iterator it_j = periods[pj].begin(); it_j != periods[pj].end(); ++it_j) {
                        int exami = *it_i;
                        int examj = *it_j;
                        n = conflictMatrix->getVal(exami, examj);
                        if (n > 0) {
                            wi = (int)pow(2.0, 4.0-i);
                            proximityCost += wi*n;
                        }
                    }
                }
            }
        }
    }

    proximityCost /= numStudents;
}



//// Get number of clashes of exam in the given period.
//// A clash is the conflict of one student having an exam in two consecutive periods.
//int Chromosome::getNumClashesExamPeriod(int exami, int pi) const {
//    // If it's the last period, there's no clashes
//    if (pi == getNumPeriods())
//        return 0;

//    int pj, numClashes = 0, n;
//    pj = pi+1;
//    if (pj < periods.size()) {
//        // Compute conflicts with exams in period pi+1
////        for (int ej = 0; ej < periods[pj].size(); ++ej) {
//        for (unordered_set<int>::const_iterator it_j = periods[pj].begin(); it_j != periods[pj].end(); ++it_j) {
////            int examj = periods[pj][ej];
//            int examj = *it_j;

//            n = conflictMatrix->getVal(exami, examj);
//            if (n > 0)
//                numClashes += n;
//        }
//    }
//    return numClashes;
//}



// Get number of clashes of exam in the given period.
// A clash is the conflict of one student having an exam in two consecutive periods.
double Chromosome::getProximityCostExamPeriod(int exami, int pi) const {
    // If it's the last period, there's no clashes
    if (pi == getNumPeriods())
        return 0;

    int pj, wi, n, numStudentsPeriod = 0;

    /// TODO: SHOULD INCLUDE PROXIMITY COST OF PERIOD PI?
    double proxCost = 0.0;

    // Consider exams four periods apart
    for (int i = 0; i <= 4; ++i) {
        pj = pi+i+1;
        if (pj < periods.size()) {
            // Compute conflicts with exams in period pi+1
    //        for (int ej = 0; ej < periods[pj].size(); ++ej) {
            for (boost::unordered_set<int>::const_iterator it_j = periods[pj].begin(); it_j != periods[pj].end(); ++it_j) {
    //            int examj = periods[pj][ej];
                int examj = *it_j;

                n = conflictMatrix->getVal(exami, examj);
                if (n > 0) {
                    wi = (int)pow(2.0, 4.0-i);
                    proxCost += wi*n;
//                    numStudentsPeriod += getNumStudentsPeriod(pj); // ACHO QUE NAO ESTA BEM
                }
            }
            // ADDED
            numStudentsPeriod += getNumStudentsPeriod(pj); // AQUI SIM
        }
    }
    if (numStudentsPeriod > 0)
        proxCost /= numStudentsPeriod;
    return proxCost;
}



// Number of clashes computation
void Chromosome::baseComputeNumClashes() {
    numberClashes = 0;
    // For each period do
    for (int pi = 0; pi < periods.size(); ++pi) {
        numberClashes += getNumClashesPeriod(pi);
    }
}

// Get number of clashes of the current period.
// A clash is the conflict of one student having an exam in two consecutive periods.
int Chromosome::getNumClashesPeriod(int pi) const {
    // If it's the last period, there are no clashes
    if (pi == getNumPeriods())
        return 0;

    int pj, numClashes = 0, n;
    // For each exam in period pi
    for (boost::unordered_set<int>::const_iterator it_i = periods[pi].begin(); it_i != periods[pi].end(); ++it_i) {
        // Consecutive period
        pj = pi+1;
        if (pj < periods.size()) {
            // Compute conflicts with exams in period pi+1
            for (boost::unordered_set<int>::const_iterator it_j = periods[pj].begin(); it_j != periods[pj].end(); ++it_j) {
                int exami = *it_i;
                int examj = *it_j;
                n = conflictMatrix->getVal(exami, examj);
                if (n > 0)
                    numClashes += n;
            }
        }
    }
    return numClashes;
}


// Number of clashes computation for the DEETC problem (Exams scheduled at saturdays have no clashes)
void Chromosome::baseComputeNumClashesDEETCProblem() {
    numberClashesDEETCProblem = 0;
    // For each period do
    for (int pi = 0; pi < periods.size(); ++pi) {
        numberClashesDEETCProblem += getNumClashesPeriodDEETCProblem(pi);
    }
}


// Get number of clashes of the current period in the DEETC problem (Exams scheduled at saturdays have no clashes)
// A clash is the conflict of one student having an exam in two consecutive periods.
int Chromosome::getNumClashesPeriodDEETCProblem(int pi) const {
    // If it's saturday, there are no clashes
    if (pi != 0 && (pi+1) % SATURDAY_PERIOD == 0)
        return 0;

    int pj, numClashes = 0, n;
    // For each exam in period pi
    for (boost::unordered_set<int>::const_iterator it_i = periods[pi].begin(); it_i != periods[pi].end(); ++it_i) {
        // Consecutive period
        pj = pi+1;
        if (pj < periods.size()) {
            // Compute conflicts with exams in period pi+1
            for (boost::unordered_set<int>::const_iterator it_j = periods[pj].begin(); it_j != periods[pj].end(); ++it_j) {
                int exami = *it_i;
                int examj = *it_j;
                n = conflictMatrix->getVal(exami, examj);
                if (n > 0)
                    numClashes += n;
            }
        }
    }
    return numClashes;
}



// Get number of students of the current "day" (or period)
int Chromosome::getNumStudentsPeriod(int pi) const {
    int numberStudents = 0;
    int exami;
    // For each exam in period pi
//    for (int ei = 0; ei < periods[pi].size(); ++ei) {
    for (boost::unordered_set<int>::const_iterator it_i = periods[pi].begin(); it_i != periods[pi].end(); ++it_i) {
//        exami = periods[pi][ei];
        int exami = *it_i;
        numberStudents += (*courseStudentCounts).at(exami);
    }
    return numberStudents;
}



/////////////////////////////////////////////////////////////////////
// Timetable packing auxiliary methods
/////////////////////////////////////////////////////////////////////

vector<int> Chromosome::getFeasiblePeriods(int exam, int period) const {
    vector<int> feasiblePeriods;
    // Determine possible feasible periods where exam can be placed
    for (int p = 0; p < getNumPeriods(); ++p) {
        if (p != period && isFeasiblePeriodExam(p, exam))
            feasiblePeriods.push_back(p);
    }
    return feasiblePeriods;
}


bool Chromosome::isFeasiblePeriodExam(int period, int exam) const {
    // Verify exam period feasibility.
    // Constraint that no student is to be scheduled
    // to take two exams at any one time:
    //   Sum_{i=1}^{|E|-1} Sum_{j=i+1}^{|E|} Sum_{p=1}^{|P|} aip ajp cij = 0
//    vector<int> const& examList = periods[period];
    boost::unordered_set<int> const& examList = periods[period];

    bool feasible = true;
//    for (int i = 0; i < examList.size(); ++i) {
    for (boost::unordered_set<int>::const_iterator it_i = examList.begin(); it_i != examList.end(); ++it_i) {
//        int numStudents = (*conflictMatrix).getVal(examList[i], exam);
        int numStudents = (*conflictMatrix).getVal(*it_i, exam);
        if (numStudents > 0) {
            feasible = false;
            break;
        }
    }
    return feasible;
}


// Get the period with the smallest number of students
int Chromosome::getMinPeriod() {
    int minPeriod = 0, count;
    int minNumSudents = getNumStudentsPeriod(minPeriod);
    int numPeriods = getNumPeriods();
    for (int p = 1; p < numPeriods; ++p) {
        count = getNumStudentsPeriod(p);
        if (count < minNumSudents) {
            minNumSudents = count;
            minPeriod = p;
        }
    }
    return minPeriod;
}

//multiset<Period> Chromosome::getExamsAvailablePeriods(vector<int> const& sourceExams, int sourcePeriod) {
multiset<Period> Chromosome::getExamsAvailablePeriods(boost::unordered_set<int> const& sourceExams, int sourcePeriod) {

    // Initialize vector in order to determine period capacity
    vector<Period> availablePeriodsVec(getNumPeriods());
    int i = 0;
    // Initialize period's Id
    for (vector<Period>::iterator it = availablePeriodsVec.begin(); it != availablePeriodsVec.end(); ++it, ++i) {
        (*it).setId(i);
    }

//    cout << "Initial periods" << endl;
//    cout << "size = " << availablePeriodsVec.size() << endl;
//    for (vector<Period>::iterator it = availablePeriodsVec.begin(); it != availablePeriodsVec.end(); ++it)
//        cout << *it << endl;

    // Determine available period capacity by filling 'availablePeriodsVec' vector
    computeAvailablePeriods(sourceExams, sourcePeriod, availablePeriodsVec);
    // Create sorted set
    multiset<Period> availablePeriodsSet;
    // Insert periods into the sorted set
    for (vector<Period>::iterator it = availablePeriodsVec.begin(); it != availablePeriodsVec.end(); ++it) {
        if ((*it).getCapacity() > 0)
            availablePeriodsSet.insert((*it));
    }
    return availablePeriodsSet;
}


// Compute available periods.
// A given p period's capacity is defined as the number of exams from
// the set 'sourceExams', the 'sourcePeriod' exams, which can scheduled into p
// without causing any clashes while maintaining feasibility.
//void Chromosome::computeAvailablePeriods(vector<int> const& sourceExams, int sourcePeriod,
//                                         vector<Period>& availablePeriodsVec) {

void Chromosome::computeAvailablePeriods(boost::unordered_set<int> const& sourceExams, int sourcePeriod,
                                         vector<Period>& availablePeriodsVec) {

    int numClashes, exam;
    int numPeriods = getNumPeriods();
    // Compute each period availability
    for (int p = 0; p < numPeriods; ++p) {
        if (p != sourcePeriod) {
//            for (int i = 0; i < sourceExams.size(); ++i) {
            for (boost::unordered_set<int>::const_iterator it_i = sourceExams.begin(); it_i != sourceExams.end(); ++it_i) {
//                exam = sourceExams[i];
                exam = *it_i;
                // Verify if period 'p' is a feasible period for the current exam
                if (isFeasiblePeriodExam(p, exam)) {
                    // Compute number of clashes between this exam and other exams
                    // in periods p-1 and p+1, excluding sourcePeriod
                    numClashes = computeNumClashesExamPeriod(exam, p, sourcePeriod);
                    if (numClashes == 0) {
                        // Add current exam to period vector which increments by one the period capacity
                        boost::shared_ptr<Exam> pExam(new Exam(exam));
                        availablePeriodsVec[p].addExam(pExam);
                    }
                }
            }
        }
    }
}


// Compute number of clashes between 'exam'
// and other exams in period p
int Chromosome::computeNumClashesPeriod(int exam, int p) const {
    int numClashes = 0;
    // Get exam's list
//    vector<int> const& examList = periods[p];
    boost::unordered_set<int> const& examList = periods[p];
//    for (int i = 0; i < examList.size(); ++i) {
    for (boost::unordered_set<int>::const_iterator it_i = examList.begin(); it_i != examList.end(); ++it_i) {
//        int numStudents = conflictMatrix->getVal(examList[i], exam);
        int numStudents = conflictMatrix->getVal(*it_i, exam);
        if (numStudents > 0)
            numClashes += numStudents;
    }
    return numClashes;
}


// Compute number of clashes between 'exam' and other exams
// in periods p-1 and p+1, excluding sourcePeriod
int Chromosome::computeNumClashesExamPeriod(int exam, int p, int sourcePeriod) const {

    /// TODO -> In Saturdays there's no clashes with the following period

    int numClashes0 = 0, numClashes1 = 0;
    if (p-1 >= 0 && p-1 != sourcePeriod) {
        numClashes0 = computeNumClashesPeriod(exam, p-1);
    }
    if (p+1 < getNumPeriods() && p+1 != sourcePeriod) {
        numClashes1 = computeNumClashesPeriod(exam, p+1);
    }
    return numClashes0 + numClashes1;
}


struct size_pred {
    bool operator()(boost::unordered_set<int> const& periodExams) { return periodExams.size() == 0; }
};

void Chromosome::removeEmptyPeriods() {
    periods.erase(std::remove_if(periods.begin(), periods.end(), size_pred()), periods.end());
}



void Chromosome::validate() const {

    cout << endl << "[validate() method]" << endl;

    int numExams = 0, numEnrolments = 0;
    boost::unordered_map<int, int> exams;
    bool unique = true;
    for (ConstPeriodIterator it = periods.begin(); it != periods.end(); ++it) {
        for (boost::unordered_set<int>::const_iterator examIt = (*it).begin(); examIt != (*it).end(); ++examIt) {
            ++numExams;
            numEnrolments += (*courseStudentCounts).at(*examIt);
            // Verify unicity of exams
            pair<boost::unordered_map<int, int>::iterator, bool> p =
                    exams.insert(pair<int, int>(*examIt, (*courseStudentCounts).at(*examIt)));
            if (p.second == false) {
                unique = false;
                break;
            }
        }
    }

    cout << "numExams = " << numExams << endl;
    cout << "numEnrolments = " << numEnrolments << endl;

    cout << "Original values" << endl;
    cout << "numExams = " << this->numExams << endl;
    cout << "numEnrolments = " << this->numEnrolments << endl;

    if (!unique) {
        cout << "There are duplicated exams" << endl;
        cin.get();
    }
    else if (numExams != this->numExams) {
        cout << "numExams is different" << endl;
        cout << "counted: " << numExams << "; original: " <<  this->numExams << endl;

        cin.get();
    }
    else if (numEnrolments != this->numEnrolments) {
        cout << "numEnrolments is different" << endl;
        int count = 0, enrol = 0;
        // Exams indexed from [1..numExams]
        vector<int> sorted_exams(this->numExams+1);
        for (boost::unordered_map<int, int>::iterator it = exams.begin(); it != exams.end(); ++it) {
//            cout << (*it).first << " " << (*it).second << endl;
            ++count;
            enrol += (*it).second;
            sorted_exams[(*it).first] = (*it).second;
        }
        cout << endl << "count = " << count << ", enrol = " << enrol << endl;
        copy(sorted_exams.begin(), sorted_exams.end(), ostream_iterator<int>(cout, "\n"));

        cin.get();
    }

    // Verify feasibility
    int i = 0;
    for (ConstPeriodIterator it = periods.begin(); it != periods.end(); ++it, ++i) {
        for (boost::unordered_set<int>::const_iterator examIt = (*it).begin(); examIt != (*it).end(); ++examIt) {
            if (!isFeasiblePeriodExam(i, *examIt)) {
                cout << endl << "Not FeasiblePeriodExam" << endl;
                cin.get();
            }
        }
    }
}



ostream& operator<<(ostream& os, const Chromosome& timetable) {
    os << endl << "Timetable" << endl;
//    int i = 1;
//    for (vector<unordered_set<int> >::const_iterator it = timetable.getConstPeriods().begin(); it != timetable.periods.end(); ++it) {
//        os << endl << "Period " << i << endl;
//        for (unordered_set<int>::const_iterator examit = (*it).begin(); examit != (*it).end(); ++examit) {
//            //os << timetable.deetc_ucs[*examit-1] << " ";
//            os << *examit << " ";
//        }
//        os << endl;
//        i++;
//    }


//    os << "NumClashes = " << timetable.getNumClashes() << " - Periods = " << timetable.getNumPeriods() << endl;
//    os << "Proximity cost = " << timetable.getProximityCost() << " - Periods = " << timetable.getNumPeriods();
//    if (timetable.getNumPeriods() == timetable.getOriginalNumPeriods()) {
//        os << " <-- " << endl;
//    }
//    else
//        os << endl;

//    timetable.validate();

    os << endl;

//    if (timetable.getNumPeriods() == timetable.getOriginalNumPeriods())
//        os << " & " << timetable.getProximityCost() << " & " << timetable.getNumPeriods() << " \\\\ " << endl;

//    timetable.baseComputeNumClashesDEETCProblem();

    os << " cost = " << timetable.getProximityCost() << " - periods = " << timetable.getNumPeriods() << endl;
    os << " Number of clashes = " << timetable.getNumClashesDEETCProblem() << endl;

    os << endl << ".Sol Data" << endl;
    int period = 0;
    for (vector<boost::unordered_set<int> >::const_iterator it = timetable.getConstPeriods().begin(); it != timetable.periods.end(); ++it) {
        for (boost::unordered_set<int>::const_iterator examit = (*it).begin(); examit != (*it).end(); ++examit) {
            os << *examit << " " << period << endl;
        }
        ++period;
    }

    return os;
}



int Chromosome::getExamTimeslot(int exam) const {
    int period = 0;
    for (vector<boost::unordered_set<int> >::const_iterator it = this->getConstPeriods().begin(); it != this->periods.end(); ++it) {
        boost::unordered_set<int> const& periodExams = *it;
        if (periodExams.find(exam) != periodExams.end()) {
            return period;
        }
        ++period;
    }
    // If code reached here, the exam doesn't exist, so throw exception
    throw runtime_error("[Chromosome::getExamTimeslot method] Could not find exam in periodExams");
}



void Chromosome::toCSV(string const& filename) const {
    // Creating the output file
    ofstream csvFile(filename);
    int period = 1;
    for (vector<boost::unordered_set<int> >::const_iterator it = getConstPeriods().begin(); it != periods.end(); ++it) {
        for (boost::unordered_set<int>::const_iterator examit = (*it).begin(); examit != (*it).end(); ++examit) {
            csvFile << getExamName(*examit);
            for (int i = 1; i <= period-1; ++i) // beginning commas
                csvFile << ",";
            csvFile << ",x";
            for (int i = period+1; i <= periods.size(); ++i) // trailing commas
                csvFile << ",";
            csvFile << "\n";
        }
        ++period;
    }
    csvFile.close();
}


void Chromosome::toCSV(string const& filename, const string programCourseNames[], int numCourses) const {
    // Creating the output file
    ofstream csvFile(filename);
    // Generate the csv file printing program courses by the order as they appear in the parameter array
    for (int i = 0; i < numCourses; ++i) {
        string const& courseName = programCourseNames[i];
        // Find course period
        int examIndex = getExamIndex(courseName);
        int timeslot = getExamTimeslot(examIndex); // timeslot starts at zero

//        cout << "course name: " << courseName << ", course index: " << examIndex << ", timeslot: " << timeslot << endl;

        // Generate csv row
        csvFile << courseName;
        for (int i = 0; i < timeslot; ++i) // beginning commas
            csvFile << ",";
        csvFile << ",x";
        for (int i = timeslot+1; i < periods.size(); ++i) // trailing commas
            csvFile << ",";
        csvFile << "\n";
    }
    csvFile.close();
}


//
// Generate subtimetables given the program course names.
// We use this method to generate the timetables for the
// LEETC, LEIC, LERCM, MEIC and MEET programs given the sets
// of course names
//
vector<Chromosome> Chromosome::generate(string* programCourseNames[], int programSizes[], int numPrograms) const {
    // Vector where the timetables are saved
    vector<Chromosome> timetables;
    // Create Data and ProblemData structures to initialize eoChromosome solutions
    ProblemData problemData(conflictMatrix, graph, numStudents, numExams,
                numEnrolments, courseStudentCounts, courseNames);

    Data data(originalNumPeriods, originalNumPeriods, originalNumPeriods);
    data.setProblemData(problemData);
    // Get reference to original chromosome periods
    vector<boost::unordered_set<int> > const& chromPeriods = this->getConstPeriods();

    for (int i = 0; i < numPrograms; ++i) {
        // Instantiate the solution
        Chromosome chrom;
        chrom.init(data);

        cout << "Number of periods: " << chrom.getNumPeriods() << endl;

        // Get reference to new chromosome periods
        vector<boost::unordered_set<int> >& newChromPeriods = chrom.getPeriods();

        // Fill timetable exams
        for (int k = 0; k < programSizes[i]; ++k) {
            // Find exam in the original chromosome and copy it to the generate subtimetable
            string const& courseName = programCourseNames[i][k];
            int exam = this->getExamIndex(courseName);
            // Find time slot in the original chromosome
            int timeslot = this->getExamTimeslot(exam);
            // Schedule exam
            newChromPeriods[timeslot].insert(exam);
        }
        // Compute chromosome proximity cost
        chrom.baseComputeProximityCosts();
        chrom.baseComputeNumClashesDEETCProblem();
        // Add to timetable vector
        timetables.push_back(chrom);
    }
    return timetables;
}











