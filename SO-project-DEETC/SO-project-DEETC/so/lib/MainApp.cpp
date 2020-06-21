#include <eo>

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "TestSetDescription.h"
#include "Data.h"
#include "ETTPInit.h"

#include "ProximityCostEval.h"
#include "ETTPneighborEval.h"
#include "eoETTPEval.h"
#include "eoETTPNumberClashesEval.h"
#include "ETTPneighbor.h"
#include "DEETCTestSet.h"
#include "eoSCEA.h"
#include "eoSCEAConstrained.h"
#include "moGDA.h"
#include <eoDetSelect.h>
#include "Mutation.h"
#include "Crossover.h"

#include "eoGenerationContinue.h"
#include "CSVParser.h"
#include "eoExtendedChromosome.h"


// Handling Ctrl+c signal
#include <signal.h>

bool volatile keepRunning = true;

void intHandler(int dummy=0) {
    keepRunning = false;
}

using namespace std;

#define MIN_NUM_DAYS_BETWEEN_EXAMS 9



// These functions are defined below

//void runHeuristicAlg1(ofstream& outFile, TestSet const& testSet);
//void runHeuristicAlg(ofstream& outFile, TestSet const& testSet);

//void runSCEA(string const& outputDir, TestSet const& testSet);
//void runGDA(ofstream& outFile, TestSet const& testSet);
//void runTA(ofstream& outFile, TestSet const& testSet);
//void runStandardEA(ofstream& outFile, TestSet const& testSet);

// DEETC benchmarks
void runDEETCDatasets(string const& outputDir);
// DEETC 2 epochs
void runSCEA_DEETC2Epochs(string const& rootDir, string const& outputDir, TestSet const& testSet1stEpoch, TestSet const& testSet2ndEpoch);


void testMOEA(int datasetIndex, string const& outputDir) {
    // Handling Ctrl+c signal
    signal(SIGINT, intHandler);

    runDEETCDatasets(outputDir);
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// runDEETCDatasets
///

void generateSubTimetables(string const& _outputDir, eoChromosome const& _solution, string* _deetcProgramsPtr[],
                           int _programSizes[], int _numDEETCPrograms, string const _deetcProgramNames[]) {
    // Generate chromosomes corresponding to sub timetables
    vector<Chromosome> programTimetables = _solution.generate(_deetcProgramsPtr, _programSizes, _numDEETCPrograms);
    int i = 0;
    for (Chromosome chrom : programTimetables) {
        // Creating the output file
        stringstream filenameStr;
        filenameStr << _deetcProgramNames[i] << "_numPeriods_" << chrom.getNumPeriods()
                    << "_cost_" << chrom.getNumClashesDEETCProblem();
        string filename;
        filenameStr >> filename;
        replace(filename.begin(), filename.end(), '.', '_');
        // Convert to csv file
        chrom.toCSV(_outputDir + filename + ".csv", _deetcProgramsPtr[i], _programSizes[i]);

        // Compute number of clashes
        chrom.baseComputeNumClashesDEETCProblem();

        /// TODO - Use eoChromosome + eoETTPNumClashesEval
        ///
        cout << "Number of clashes of generated solution: " << chrom.getNumClashesDEETCProblem() << endl;
        ++i;
    }
}



eoChromosome generate2ndEpoch(string const& _outputDir, ofstream & _outFile, string const& _filename, TestSet const& _testSet2ndEpoch,
                      int _m, int _N, int _q, int _F, long _L, double _up,
                      double _cp, double _mp, double _ip) {
    //
    //   1. Generate initial 2nd epoch timetable (12 time slots) and optimize it
    //
    cout << "Run SCEA on 2nd Epoch..." << endl;
    _outFile << "Run SCEA on 2nd Epoch..." << endl;

    cout << _testSet2ndEpoch << endl;
    _outFile << _testSet2ndEpoch << endl;

    // Solution initializer
    ETTPInit<eoChromosome> init(_testSet2ndEpoch.getData(),
                                *(_testSet2ndEpoch.getConflictMatrix()), *(_testSet2ndEpoch.getGraph()));
    // Generate initial population
    eoPop<eoChromosome> pop(_F, init);
    // Objective function evaluation
    eoETTPEval<eoChromosome> proximityCostEval;
    eoETTPNumberClashesEval<eoChromosome> numClashesEval;

    for (int i = 0; i < pop.size(); ++i) {
//        numClashesEval(pop[i]);
        proximityCostEval(pop[i]);
    }

    // Terminate after concluding L time loops or 'Ctrl+C' signal is received
    eoGenerationContinue<eoChromosome> terminator(_L);
    eoCheckPoint<eoChromosome> checkpoint(terminator);

    // Crossover and mutation
    Mutation<eoChromosome> mutation;
    Crossover<eoChromosome> crossover;

    // Chromosome evolution operator
    eoSFLAEvolOperator_3<eoChromosome> evolutionOperator;

    // Build SCEA
    eoSCEA<eoChromosome> scea(_outputDir, _outFile, _filename, _m, _N, _q, _F, _L, _up,
                              _cp, _mp, _ip,
                              init, checkpoint, proximityCostEval,
                              crossover, mutation, evolutionOperator);

    // Run the algorithm
    scea(pop);

    // Best frog
    eoChromosome bestFrog = scea.getBestFrog();

    // Number of clashes eval
    numClashesEval(bestFrog);

    cout << "End of evolution cycle" << endl
         << "Writing best solution to file..." << endl;

    // Write best solution to file
    ofstream solutionFile;
    solutionFile.open(_outputDir + _filename + ".sol", ios::out | ios::trunc);
    solutionFile << "==============================================================" << endl;
    solutionFile << "Date/Time = " << currentDateTime() << endl;
    solutionFile << bestFrog.getChromosome() << endl;
    solutionFile << "==============================================================" << endl;
    solutionFile.close();

    cout << "End Date/Time = " << currentDateTime() << endl;

    // Write to file
    _outFile << "End Date/Time = " << currentDateTime() << endl;

    // Return best solution
    return bestFrog;

}


void getPeriodsUpperBound(eoChromosome const& _bestSolution2ndEpoch, vector<int> & _firstEpochPeriodsUpperBound) {
    int period = 0;
    for (vector<boost::unordered_set<int> >::const_iterator it = _bestSolution2ndEpoch.getConstPeriods().begin();
                    it != _bestSolution2ndEpoch.getConstPeriods().end(); ++it) {
        for (boost::unordered_set<int>::const_iterator examit = (*it).begin(); examit != (*it).end(); ++examit) {
            // 1st epoch period upper bound = 2nd epoch period (in interval [19, 30]) - Min exam distance
            int upperBound = (period + NUM_PERIODS_FIRST_EPOCH) - (MIN_NUM_DAYS_BETWEEN_EXAMS+1);
            _firstEpochPeriodsUpperBound[*examit] =
                    (upperBound >= NUM_PERIODS_FIRST_EPOCH) ? NUM_PERIODS_FIRST_EPOCH-1 : upperBound;
        }
        ++period;
    }
}



eoChromosome generate1stEpoch(string const& _outputDir, ofstream & _outFile, string const& _filename, TestSet const& _testSet1stEpoch,
                      int _m, int _N, int _q, int _F, long _L, double _up,
                      double _cp, double _mp, double _ip,
                      eoChromosome const& _bestSolution2ndEpoch) {

    cout << "Run SCEA on 1st Epoch..." << endl;
    _outFile << "Run SCEA on 1st Epoch..." << endl;

    cout << _testSet1stEpoch << endl;
    _outFile << _testSet1stEpoch << endl;

    // Get first epoch periods's upper bound as specified by the H2 constraint
    // Vector has length numExams+1 and the value at each index (>=1) represents the period upper bound of that exam
    vector<int> firstEpochPeriodsUpperBound(_bestSolution2ndEpoch.getNumExams()+1); // Exams are indexed at 1
    getPeriodsUpperBound(_bestSolution2ndEpoch, firstEpochPeriodsUpperBound);

    cout << "First-epoch periods upper bound:" << endl;
    for (int i = 1; i < firstEpochPeriodsUpperBound.size(); ++i) {
        cout << i << " - " << firstEpochPeriodsUpperBound[i] << endl;
    }

    //   2. Generate 1st epoch (18 time slots) taking into
    //        account the hard constraint H2 - minimum exam distance
    //      - Use saturation degree (SD) heuristic
    //
    // Solution initializer
    ETTPInit<eoExtendedChromosome> init(_testSet1stEpoch.getData(), *(_testSet1stEpoch.getConflictMatrix()),
                                *(_testSet1stEpoch.getGraph()), firstEpochPeriodsUpperBound);
    // Generate initial population
    eoPop<eoExtendedChromosome> pop(_F, init);
    // Objective function evaluation
    eoETTPEval<eoExtendedChromosome> proximityCostEval;
    eoETTPNumberClashesEval<eoExtendedChromosome> numClashesEval;
    // Append second epoch to each extended chromosome object
    for (int i = 0; i < pop.size(); ++i) {
        pop[i].setSecondEpoch(_bestSolution2ndEpoch);
    }
    // Evaluate population
    for (int i = 0; i < pop.size(); ++i) {
        proximityCostEval(pop[i]);
        cout << pop[i].fitness() << " ";
    }
    cout << endl;

    //   3. Optimize 1st epoch timetables using modified SCEA in order to operate
    //      feasible solutions which respect both H1 and H2 hard constraints
    //
    //
    // Terminate after concluding L time loops or 'Ctrl+C' signal is received
    eoGenerationContinue<eoExtendedChromosome> terminator(_L);
    eoCheckPoint<eoExtendedChromosome> checkpoint(terminator);

    // Crossover and mutation
    Mutation<eoExtendedChromosome> mutation;
    Crossover<eoExtendedChromosome> crossover;

    // Chromosome evolution operator
    eoSFLAEvolOperator_3<eoExtendedChromosome> evolutionOperator;

    // Build SCEAConstrained object
    eoSCEAConstrained<eoExtendedChromosome> scea(_outputDir, _outFile, _filename, _m, _N, _q, _F, _L, _up,
                              _cp, _mp, _ip,
                              init, checkpoint, proximityCostEval,
                              crossover, mutation, evolutionOperator,
                              firstEpochPeriodsUpperBound);

    // Run the algorithm
    scea(pop);

    // Best frog
    eoExtendedChromosome bestFrog = scea.getBestFrog();

    // Number of clashes eval
    numClashesEval(bestFrog);

    cout << "End of evolution cycle" << endl
         << "Writing best solution to file..." << endl;

    // Write best solution to file
    ofstream solutionFile;
    solutionFile.open(_outputDir + _filename + ".sol", ios::out | ios::trunc);
    solutionFile << "==============================================================" << endl;
    solutionFile << "Date/Time = " << currentDateTime() << endl;
    solutionFile << bestFrog.getChromosome() << endl;
    solutionFile << "==============================================================" << endl;
    solutionFile.close();

    cout << "End Date/Time = " << currentDateTime() << endl;

    // Write to file
    _outFile << "End Date/Time = " << currentDateTime() << endl;

    // Return best solution
    return bestFrog;
}



void runDEETCDatasets(string const& outputDir) {
    //
    // ISEL/DEETC department test data - Winter 09/10
    //
    // 1st Epoch based on XLS file 'Relacoes UC 0910i.xls'
    // 2nd Epoch based on XLS file 'Relacoes UC 0910i_2epoca.xls'
    //

    // Root directory
    string rootDir = "./../../../ETTP-Benchmarks/DEETC-2009-2010";

    //
    // 1st epoch definition
    //
    TestSetDescription deetcTestSetDescription1stEpoch = TestSetDescription("deetc0910_1stEpoch", "DEETC ISEL 1stEpoch", 18);
    // Periods range definition.
    int periods = deetcTestSetDescription1stEpoch.getPeriods();
//    cout << "Number of time slots 1st epoch = " << periods << endl;
    // Specify MinPeriods and MaxPeriods
    Data data1stEpoch(periods, periods, periods); // Fixed length timetables
    // Create TestSet instance
    DEETCTestSet testSet1stEpoch(deetcTestSetDescription1stEpoch.getName(), deetcTestSetDescription1stEpoch.getDescription(),
                         data1stEpoch, rootDir);

    TorontoTestSet* ptr = &testSet1stEpoch;
    ptr->load(deetcTestSetDescription1stEpoch.getName(), rootDir);

    //
    // 2nd epoch definition
    //
    TestSetDescription deetcTestSetDescription2ndEpoch = TestSetDescription("deetc0910_2ndEpoch", "DEETC ISEL 2ndEpoch", 12);
    // Periods range definition.
    periods = deetcTestSetDescription2ndEpoch.getPeriods();
    cout << "Number of time slots 2nd epoch = " << periods << endl;
    // Specify MinPeriods and MaxPeriods
    Data data2ndEpoch(periods, periods, periods); // Fixed length timetables
    // Create TestSet instance
    DEETCTestSet testSet2ndEpoch(deetcTestSetDescription2ndEpoch.getName(), deetcTestSetDescription2ndEpoch.getDescription(),
                         data2ndEpoch, rootDir);

    ptr = &testSet2ndEpoch;
    ptr->load(deetcTestSetDescription2ndEpoch.getName(), rootDir);

    // Print dataset info
    cout << testSet1stEpoch << endl;
    cout << testSet2ndEpoch << endl;


//    ofstream outFile(deetcTestSetDescription1stEpoch.getName() + ".txt");

    // Run test set
//    runSFLA(outFile, testSet);
//    runCellularEA(testSet1stEpoch);

//    runSCEA(testSet1stEpoch);

    runSCEA_DEETC2Epochs(rootDir, outputDir, testSet1stEpoch, testSet2ndEpoch);
}




void runSCEA_DEETC2Epochs(string const& rootDir, string const& outputDir,
                          TestSet const& testSet1stEpoch, TestSet const& testSet2ndEpoch) {
    //
    // ISEL/DEETC department test data - Winter 09/10
    //
    // Read Manual solution from CSV file
    //

// *ORIGINAL DATA
// *
//    // Constants
//    const int NUM_DEETC_PROGRAMS = 5;
//    const int NUM_LEETC_COURSES  = 33;
//    const int NUM_LEIC_COURSES   = 31;
//    const int NUM_LERCM_COURSES  = 31;
//    const int NUM_MEIC_COURSES   = 21;
//    const int NUM_MEET_COURSES   = 32;

//    //    LEETC   32
//    //    LEIC    30
//    //    LERCM   29
//    //    MEIC    19
//    //    MEET    25

//    // DEETC Program and Course names
//    string filenames[NUM_DEETC_PROGRAMS] = { "LEETC", "LEIC", "LERCM", "MEIC", "MEET" };

//    string leetcCourses[NUM_LEETC_COURSES] = { "ALGA", "Pg", "AM1", "FAE", "ACir",
//                        "POO", "AM2", "LSD", "E1", "MAT",
//                        "PE", "ACp", "EA", "E2", "SS",
//                        "RCp", "PICC/CPg", "PR", "FT", "SEAD1",
//                        "ST", "RCom", "SEAD2", "RI", "SE1", "AVE", "SCDig", "SOt",
//                        "PI", "SCDist", "EGP", "OGE", "SG" };

//    string leicCourses[NUM_LEIC_COURSES] = { "ALGA", "Pg", "LSD", "M1", "Elctr",
//                        "POO", "PE", "ACp", "M2" "PSC",
//                        "AC2", "AED", "Com", "CG", "SI1", "LC", "PF", "EGP", "OGE", "SG",
//                        "RCp", "AVE", "SOi",
//                        "PI", "SI2", "PC", "SI", "RI", "SE1", "Cpl",
//                        "SD" };

//    string lercmCourses[NUM_LERCM_COURSES] = { "ALGA", "Pg", "AM1", "F1", "ITI",
//                        "POO", "PE", "AM2", "F2", "PDSr",
//                        "SCDig", "MNO", "AIEC", "PICC/CPg", "CSDin",
//                        "RCp", "CSM", "FIA", "MSr", "SOt",
//                        "BD", "CGAV", "AA", "RI", "SCDist",
//                        "ES", "EGP", "OGE", "RSCM", "PCM", "PIV" };

//    string meicCourses[NUM_MEIC_COURSES] = { "SI", "SD", "ES", "RI", "SE1", "Cpl", "CCD", "CSO",
//                        "CSI", "RSCM", "CAC", "CIA", "AA",
//                        "ASI", "GSI", "PSTR", "IRS", "IS", "EGP", "OGE", "SG" };

//    string meetCourses[NUM_MEET_COURSES] = { "AVE", "SE1", "RI", "SEAD2", "ST", "RCom", "PIV",
//                    "SCDig", "SEAS", "CEE", "OE", "PRC", "RSCM", "SET",
//                    "Ant", "CMov", "STBL", "CSDist",
//                    "BD", "CSM", "PI", "SCDist", "EGP", "OGE", "SG", "SCO",
//                    "PCI", "IRS", "RDC", "SEADI", "RM", "STDS/PSTR" };


    // ExamsToRemove = {'LIC', 'SEAD2', 'AC2', 'FIA', 'PCM', 'CIA', 'PSTR', ...
    //  'SEAS', 'CEE', 'PRC', 'SEADI', 'RM', 'STDS'}; % PRC does not have exam. Remove 12 exams from a total of 92


    // CSV file parser
    CSVParser csvParser;
    // Constants
    const int NUM_DEETC_PROGRAMS = 5;
    const int NUM_LEETC_COURSES  = 32;//33
    const int NUM_LEIC_COURSES   = 30;//31
    const int NUM_LERCM_COURSES  = 29;//31
    const int NUM_MEIC_COURSES   = 19;//21
    const int NUM_MEET_COURSES   = 25;//32
    // DEETC Program and Course names
    string deetcProgramNames[NUM_DEETC_PROGRAMS] = { "LEETC", "LEIC", "LERCM", "MEIC", "MEET" };

    string leetcCourses[NUM_LEETC_COURSES] = { "ALGA", "Pg", "AM1", "FAE", "ACir",
                        "POO", "AM2", "LSD", "E1", "MAT",
                        "PE", "ACp", "EA", "E2", "SS",
                        "RCp", "PICC/CPg", "PR", "FT", "SEAD1",
                        "ST", "RCom", /*"SEAD2",*/ "RI", "SE1", "AVE", "SCDig", "SOt",
                        "PI", "SCDist", "EGP", "OGE", "SG" };

    string leicCourses[NUM_LEIC_COURSES] = { "ALGA", "Pg", "LSD", "M1", "Elctr",
                        "POO", "PE", "ACp", "M2", /*"LIC",*/
                        "PSC", /*"AC2",*/ "AED", "Com", "CG", "SI1", "LC", "PF", "EGP", "OGE", "SG",
                        "RCp", "AVE", "SOi",
                        "PI", "SI2", "PC", "SI", "RI", "SE1", "Cpl",
                        "SD" };

    string lercmCourses[NUM_LERCM_COURSES] = { "ALGA", "Pg", "AM1", "F1", "ITI",
                        "POO", "PE", "AM2", "F2", "PDSr",
                        "SCDig", "MNO", "AIEC", "PICC/CPg", "CSDin",
                        "RCp", "CSM", /*"FIA",*/ "MSr", "SOt",
                        "BD", "CGAV", "AA", "RI", "SCDist",
                        "ES", "EGP", "OGE", "RSCM", /*"PCM",*/ "PIV" };

    string meicCourses[NUM_MEIC_COURSES] = { "SI", "SD", "ES", "RI", "SE1", "Cpl", "CCD", "CSO",
                        "CSI", "RSCM", "CAC", /*"CIA",*/ "AA",
                        "ASI", "GSI", /*"PSTR",*/ "IRS", "IS", "EGP", "OGE", "SG" };

    string meetCourses[NUM_MEET_COURSES] = { "AVE", "SE1", "RI", /*"SEAD2",*/ "ST", "RCom", "PIV",
                    "SCDig", /*"SEAS", "CEE",*/ "OE", /*"PRC",*/ "RSCM", "SET",
                    "Ant", "CMov", "STBL", "CSDist",
                    "BD", "CSM", "PI", "SCDist", "EGP", "OGE", "SG", "SCO",
                    "PCI", "IRS", "RDC", /*"SEADI", "RM", "STDS/PSTR"*/ };


    // Print program info
    string* deetcProgramsPtr[NUM_DEETC_PROGRAMS] = { leetcCourses, leicCourses, lercmCourses, meicCourses, meetCourses };
    int programSizes[NUM_DEETC_PROGRAMS] = { NUM_LEETC_COURSES, NUM_LEIC_COURSES, NUM_LERCM_COURSES, NUM_MEIC_COURSES, NUM_MEET_COURSES };
    for (int i = 0; i < NUM_DEETC_PROGRAMS; ++i) {
        cout << "Program: " << deetcProgramNames[i] << endl;
        copy(deetcProgramsPtr[i], deetcProgramsPtr[i] + programSizes[i], ostream_iterator<string>(cout, " "));
        cout << endl;
    }



/*
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Read Manual solution from csv files
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // Create empty first and second epoch solutions
    eoChromosome solFirstEpoch, solSecondEpoch;
    // Initialize first and second epoch solutions
    // with different number of periods and conflict matrices
    solFirstEpoch.init(testSet1stEpoch.getData());
    solSecondEpoch.init(testSet2ndEpoch.getData());
    /// Print solution info ////////////////////
//    cout << "1st epoch info:" << endl;
//    cout << "NumPeriods: " << solFirstEpoch.getNumPeriods() << endl;
//    cout << "2nd epoch info:" << endl;
//    cout << "NumPeriods: " << solSecondEpoch.getNumPeriods() << endl;
    ////////////////////////////////////////////

    cout << "Import timetable from CSV file..." << endl;
    vector<string> timetables[NUM_DEETC_PROGRAMS];
    for (int i = 0; i < NUM_DEETC_PROGRAMS; ++i) {
//        string programFilename = deetcProgramNames[i] + "-ManualSol.csv";
        string programFilename = deetcProgramNames[i] + "-AutomaticSol.csv";
        cout << "Importing " << programFilename << "..." << endl;
        csvParser.parse(rootDir + "/Generated/deetc0910-" + programFilename, timetables[i]);
//        copy(timetables[i].begin(), timetables[i].end(), ostream_iterator<string>(cout, " "));
//        cout << endl;
        // Create 2 eoChromosome objects from each CSV file
        cout << "Create eoChromosome objects (1st and 2nd epochs) from CSV file..." << endl;

        csvParser.parse(rootDir + "/Generated/deetc0910-" + programFilename, solFirstEpoch, solSecondEpoch);

    }

    // Validate solutions
    cout << "Validating 1st epoch solution..." << endl;
    solFirstEpoch.validate();
    cout << "Validating 2nd epoch solution..." << endl;
    solSecondEpoch.validate();

    // Proximity cost objective function evaluation
//    eoETTPEval eval;
//    eval(solFirstEpoch);
//    cout << "Cost of 1st epoch solution: " << solFirstEpoch.fitness() << endl;
//    eval(solSecondEpoch);
//    cout << "Cost of 2nd epoch solution: " << solSecondEpoch.fitness() << endl;
//    Cost of 1st epoch solution: 25.8279
//    Cost of 2nd epoch solution: 46.7803

    // Number of clashes objective function evaluation
    eoETTPNumberClashesEval numClashesEval;
    numClashesEval(solFirstEpoch);
    cout << "Number of clashes of 1st epoch solution: " << solFirstEpoch.fitness() << endl;

    numClashesEval(solSecondEpoch);
    cout << "Number of clashes of 2nd epoch solution: " << solSecondEpoch.fitness() << endl;

//    solFirstEpoch.toCSV();
//    solSecondEpoch.toCSV();

    vector<Chromosome> programTimetables = solFirstEpoch.generate(deetcProgramsPtr, programSizes, NUM_DEETC_PROGRAMS);

    int i = 0;
    for (Chromosome chrom : programTimetables) {
//        // Creating the output file
//        stringstream filenameStr;
//        filenameStr << deetcPrograms[i] << "_numPeriods_" << chrom.getNumPeriods()
//                    << "_cost_" << chrom.getProximityCost();
//        string filename;
//        filenameStr >> filename;
//        replace(filename.begin(), filename.end(), '.', '_');
//        // Convert to csv file
////        chrom.toCSV(filename + ".csv");
//        chrom.toCSV(filename + ".csv", deetcProgramsPtr[i], programSizes[i]);

//        // Compute number of clashes
        chrom.baseComputeNumClashesDEETCProblem();

//        /// TODO - Use eoChromosome + eoETTPNumClashesEval
//        ///
        cout << "Number of clashes of generated solution: " << chrom.getNumClashesDEETCProblem() << endl;
        ++i;
    }

    programTimetables = solSecondEpoch.generate(deetcProgramsPtr, programSizes, NUM_DEETC_PROGRAMS);

    i = 0;
    for (Chromosome chrom : programTimetables) {
//        // Compute number of clashes
        chrom.baseComputeNumClashesDEETCProblem();

//        /// TODO - Use eoChromosome + eoETTPNumClashesEval
//        ///
        cout << "Number of clashes of generated solution: " << chrom.getNumClashesDEETCProblem() << endl;
        ++i;
    }


    cout << "Done generating DEETC manual solution." << endl;



    return;

*/



    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Automatic DEETC timetable generation
    //
    //   1. Generate initial 2nd epoch timetable (12 time slots) and optimize it
    //   2. Generate 1st epoch (18 time slots) taking into
    //        account the hard constraint H2 - minimum exam distance
    //      - Use saturation degree (SD) heuristic
    //   3. Optimize 1st epoch timetables using modified SCEA in order to operate
    //      feasible solutions which respect both H1 and H2 hard constraints
    //
    // Steps:
    //   1. First generate 2nd-epoch.
    //   2. Then, put timetables with standard names (LEETC.csv, ...) in folder "secondEpochSolutions/"
    //   3. Finally, comment 2nd-epoch generation code and run 1st-epoch code
    //
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //
    // SCEA parameters
    //
    const int m = 3;  // m is the number of memeplexes
    const int N = 8;  // N is the number of frogs in each memeplex
    const int q = 8;  // q is the number of frogs in each submemeplex
//    const double up = 0.99;
//    const double up = 0.01;
    const double up = 0.001; // First experiences
//    const double up = 0.0001;
//    const double up = 0.00001;
//    const double up = 0.000001;

//    const double up = 0.0000001; // ECTA 2014

    const int F = m*N; // The total sample size, F, in the swamp is given by F = mN.

    // Number of consecutive time loops
//    int L = 1;
//    int L = 15;
//    int L = 50;
    int L = 100000000;

    // Crossover, mutation and improve probabilities
//    const double cp = 0.6;
//    const double mp = 0.1;

//        const double cp = 0;
    const double cp = 0.2;
//    const double mp = 1;

//    const double cp = 0;
    const double mp = 0;

    const double ip = 1;


    //////////////////////////////////////////////////////////////////////////////////////////
    /// STEP 1 CODE (SEE ABOVE) - COMMENT REMAINDER CODE
    ///
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
/*
    // Create output file
    stringstream filenameStr2;
    filenameStr2 << testSet2ndEpoch.getName()
                << "_m_" << m << "_N_" << N << "_q_" << q
                << "_L_" << L << "_cp_" << cp << "_mp_" << mp << "_ip_" << ip
                << "_up_" << up;

    string filename2;
    filenameStr2 >> filename2;
    ofstream outFile2(outputDir + filename2 + ".txt");

    cout << "Start Date/Time = " << currentDateTime() << endl;
    // Write Start time and algorithm parameters to file
    outFile2 << "Start Date/Time = " << currentDateTime() << endl;
    outFile2 << "SCEA parameters:" << endl;
    outFile2 << "m = " << m << ", N = " << N << ", q = " << q << ", L = " << L << endl;
    outFile2 << "cp = " << cp << ", mp = " << mp << ", ip = " << ip << endl;
    outFile2 << "GDA parameters:" << endl;
    outFile2 << "up: " << up << endl;

    //
    //   1. Generate initial 2nd epoch timetable (12 time slots) and optimize it
    //
    eoChromosome bestSolution2ndEpoch = generate2ndEpoch(outputDir, outFile2, filename2, testSet2ndEpoch, m, N, q, F, L, up, cp, mp, ip);
    // Generate sub timetables for best solution
    generateSubTimetables(outputDir, bestSolution2ndEpoch, deetcProgramsPtr, programSizes, NUM_DEETC_PROGRAMS, deetcProgramNames);
*/


    //////////////////////////////////////////////////////////////////////////////////////////
    /// STEP 2-3 CODE (SEE ABOVE) - COMMENT STEP 1 CODE
    ///
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    //
    // Read 2nd epoch solution from CSV file
    //
//    eoChromosome solFirstEpoch;
    eoChromosome bestSolution2ndEpoch;
    // Initialize first and second epoch solutions
    // with different number of periods and conflict matrices
//    solFirstEpoch.init(testSet1stEpoch.getData());
    bestSolution2ndEpoch.init(testSet2ndEpoch.getData());
    /// Print solution info ////////////////////
//    cout << "1st epoch info:" << endl;
//    cout << "NumPeriods: " << solFirstEpoch.getNumPeriods() << endl;
//    cout << "2nd epoch info:" << endl;
//    cout << "NumPeriods: " << solSecondEpoch.getNumPeriods() << endl;
    ////////////////////////////////////////////
    cout << "Import timetable from CSV file..." << endl;
    vector<string> timetables[NUM_DEETC_PROGRAMS];
    for (int i = 0; i < NUM_DEETC_PROGRAMS; ++i) {
        string programFilename = deetcProgramNames[i] + ".csv";
        cout << "Importing " << programFilename << "..." << endl;
        // Create 2 eoChromosome objects from each CSV file
//        cout << "Create eoChromosome objects (1st and 2nd epochs) from CSV file..." << endl;
//        csvParser.parse(rootDir + "/secondEpochSolutions/" + programFilename, solFirstEpoch, bestSolution2ndEpoch);
        cout << "Create eoChromosome object (2nd epoch) from CSV file..." << endl;
        cout << "Second epoch dir: " << outputDir + "secondEpochSolutions/" + programFilename << endl;
        csvParser.parse(outputDir + "secondEpochSolutions/" + programFilename, bestSolution2ndEpoch);

    }

    // Validate solutions
//    cout << "Validating 1st epoch solution..." << endl;
//    solFirstEpoch.validate();
    cout << "Validating 2nd epoch solution..." << endl;
    bestSolution2ndEpoch.validate();

    // Compute 2nd epoch solution # clashes
    bestSolution2ndEpoch.baseComputeNumClashesDEETCProblem();
    cout << "2nd epoch solution # clashes = " << bestSolution2ndEpoch.getNumClashesDEETCProblem() << endl;

//    cin.get();
    //////////////////////////////////////////////////////////////////////////////////////////

    //   2. Generate 1st epoch (18 time slots) taking into
    //        account the hard constraint H2 - minimum exam distance
    //      - Use saturation degree (SD) heuristic
    //
    //   3. Optimize 1st epoch timetables using modified SCEA in order to operate
    //      feasible solutions which satisfy both H1 and H2 hard constraints
    //

    // Create output file
    stringstream filenameStr1;
    filenameStr1 << testSet1stEpoch.getName()
                << "_m_" << m << "_N_" << N << "_q_" << q
                << "_L_" << L << "_cp_" << cp << "_mp_" << mp << "_ip_" << ip
                << "_up_" << up;

    string filename1;
    filenameStr1 >> filename1;
    ofstream outFile1(outputDir + filename1 + ".txt");

    cout << "Start Date/Time = " << currentDateTime() << endl;
    // Write Start time and algorithm parameters to file
    outFile1 << "Start Date/Time = " << currentDateTime() << endl;
    outFile1 << "SCEA parameters:" << endl;
    outFile1 << "m = " << m << ", N = " << N << ", q = " << q << ", L = " << L << endl;
    outFile1 << "cp = " << cp << ", mp = " << mp << ", ip = " << ip << endl;
    outFile1 << "GDA parameters:" << endl;
    outFile1 << "up: " << up << endl;

    eoChromosome bestSolution1stEpoch = generate1stEpoch(outputDir, outFile1, filename1, testSet1stEpoch, m, N, q, F, L, up, cp, mp, ip,
                                                         bestSolution2ndEpoch);
    // Generate sub timetables for best solution
    generateSubTimetables(outputDir, bestSolution1stEpoch, deetcProgramsPtr, programSizes, NUM_DEETC_PROGRAMS, deetcProgramNames);

    cout << "End of 2-epoch timetable generation. Exiting..." << endl;
}








