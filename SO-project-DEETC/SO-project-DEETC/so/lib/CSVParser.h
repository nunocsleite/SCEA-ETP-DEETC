#ifndef CSVPARSER_H
#define CSVPARSER_H


#include <string>
#include <vector>
#include <iterator>
#include <boost/tokenizer.hpp>
#include <boost/unordered_set.hpp>
#include "eoChromosome.h"

using namespace std;
using namespace boost;

// Maximum number of periods of the two epoch problem
#define MAX_NUM_PERIODS         30
#define NUM_PERIODS_FIRST_EPOCH 18


class CSVParser {

public:
    void parse(string const& csvFileName, vector<string> & outputVec) {
        string wholeFile = read_from_file(csvFileName.c_str());
        tokenizer<escaped_list_separator<char> > tk(wholeFile, escaped_list_separator<char>('\\', ',', '\"'));

        for (tokenizer<escaped_list_separator<char> >::iterator i(tk.begin()); i != tk.end(); ++i)
        {
           outputVec.push_back(*i);
//           cout << *i << " ";
        }
    }

    // Parses a CSV file containing a 2-epoch timetable and builds the corresponding eoChromosome objects
    void parse(string const& _csvFileName, eoChromosome & _solFirstEpoch, eoChromosome & _solSecondEpoch) {
        ifstream in(_csvFileName.c_str());
        if (!in.is_open()) {
            cerr << "Couldn't open " << _csvFileName.c_str() << " file" << endl;
            exit(-1);
        }

        // eoChromosome periods
        vector<boost::unordered_set<int> >& periodsFirstEpoch = _solFirstEpoch.getPeriods();
        vector<boost::unordered_set<int> >& periodsSecondEpoch = _solSecondEpoch.getPeriods();

        // Parse CSV file and build eoChromosomes
        typedef tokenizer<escaped_list_separator<char> > Tokenizer;
        string line;
        int period, firstPeriod, secondPeriod;
        string courseName;

        while (getline(in, line))
        {
            Tokenizer tok(line);
            period = firstPeriod = secondPeriod = -1;
            // Parse a line
            for (tokenizer<escaped_list_separator<char> >::iterator i(tok.begin()); i != tok.end(); ++i)
            {
                if (period == -1) {
                    courseName = *i;
                    period = 1;
                }
                else {
                    if (*i == "x") {
                        if (firstPeriod == -1)
                            firstPeriod = period;
                        else
                            secondPeriod = period;
                    }
                    ++period;
                }
            }
            // Row parsing complete. Update solution info and reset variables

//            cout << "courseName = " << courseName << ", firstPeriod = " << firstPeriod
//                 << ", secondPeriod = " << secondPeriod << endl;

            //
            // Schedule exams
            //
            if (firstPeriod != -1 && secondPeriod != -1) {
                int exam = _solFirstEpoch.getExamIndex(courseName);
                // Schedule exam in the first period
                periodsFirstEpoch[firstPeriod-1].insert(exam);
                // Schedule exam in the second period
                periodsSecondEpoch[secondPeriod-NUM_PERIODS_FIRST_EPOCH-1].insert(exam);
//                cout << "Scheduled exam " << courseName << endl;
            }
        }
    }


    // Parses a CSV file containing a the 2-epoch solution timetable and builds the corresponding eoChromosome object
    void parse(string const& _csvFileName, eoChromosome & _solSecondEpoch) {
        ifstream in(_csvFileName.c_str());
        if (!in.is_open()) {
            cerr << "Couldn't open " << _csvFileName.c_str() << " file" << endl;
            exit(-1);
        }

        // eoChromosome periods
        vector<boost::unordered_set<int> >& periodsSecondEpoch = _solSecondEpoch.getPeriods();

        // Parse CSV file and build eoChromosomes
        typedef tokenizer<escaped_list_separator<char> > Tokenizer;
        string line;
        int period, secondPeriod;
        string courseName;

        while (getline(in, line))
        {
            Tokenizer tok(line);
            period = secondPeriod = -1;
            // Parse a line
            for (tokenizer<escaped_list_separator<char> >::iterator i(tok.begin()); i != tok.end(); ++i)
            {
                if (period == -1) {
                    courseName = *i;
                    period = 1;
                }
                else {
                    if (*i == "x") {
                        secondPeriod = period;
                    }
                    ++period;
                }
            }
            // Row parsing complete. Update solution info and reset variables

//            cout << "courseName = " << courseName << ", firstPeriod = " << firstPeriod
//                 << ", secondPeriod = " << secondPeriod << endl;

            //
            // Schedule exams
            //
            if (secondPeriod != -1) {
                int exam = _solSecondEpoch.getExamIndex(courseName);
                // Schedule exam in the second period
                periodsSecondEpoch[secondPeriod-1].insert(exam);
//                cout << "Scheduled exam " << courseName << endl;
            }
        }
    }

};


#endif // CSVPARSER_H
