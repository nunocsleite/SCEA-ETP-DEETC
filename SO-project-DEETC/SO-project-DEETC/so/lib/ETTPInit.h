#ifndef ETTPINIT_H
#define ETTPINIT_H

#include "GraphColouringHeuristics.h"
#include "Matrix.h"
#include "Data.h"
#include "TestSet.h"
#include "eoETTPEval.h"

using namespace std;
using namespace boost;


template <typename EOT>
class ETTPInit : public eoInit<EOT> {

public:
    ETTPInit(Data const& _data, Matrix const& _conflictMatrix, AdjacencyList const& _graph)
        : numPeriods(0), data(_data), conflictMatrix(_conflictMatrix), graph(_graph),
          firstEpochPeriodsUpperBoundPtr(nullptr) { }

    ETTPInit(Data const& _data, Matrix const& _conflictMatrix, AdjacencyList const& _graph,
             vector<int> const& _firstEpochPeriodsUpperBound)
        : numPeriods(0), data(_data), conflictMatrix(_conflictMatrix), graph(_graph),
          firstEpochPeriodsUpperBoundPtr(&_firstEpochPeriodsUpperBound) { }


    virtual void operator()(EOT& _chrom) {
        do {
            // For each chromosome, a timetable with a random number of empty
            // periods within the desired range is created.
            _chrom.init(data);

            // Exams are then inserted into randomly selected periods in the order
            // determined by the graph colouring heuristic, depending on the version
            // of the MOEA. Like the mutation operator, when it is not possible to
            // schedule an exam without violating any of the hard constraints,
            // a new period will be created at the end of the timetable to
            // accommodate the exam.
            if (firstEpochPeriodsUpperBoundPtr == nullptr)
                GCHeuristics::SD(conflictMatrix, graph, _chrom);
            else
                GCHeuristics::SD(conflictMatrix, graph, _chrom, *firstEpochPeriodsUpperBoundPtr);

        } while (!isFeasible(_chrom));
    }

private:

    bool isFeasible(EOT const& _chrom) const {
        if (_chrom.getNumPeriods() > _chrom.getRange()[1]) {
             cout << "Infeasible chromosome: Number of periods = " << _chrom.getNumPeriods()
                  << " above upper limit (" << _chrom.getRange()[1] << ")" << endl;
            return false;
        }
        //  if (_chrom.getNumPeriods() < _chrom.getRange()[0]) {
        //        cout << "Infeasible chromosome: Number of periods = " << _chrom.getNumPeriods()
        //             << " below lower limit (" << _chrom.getRange()[0] << ")" << endl;
        //        return false;
        //  }
        return true;
    }

    // Instance fields
    const Data& data;
    const Matrix& conflictMatrix;
    const AdjacencyList& graph;
    int numPeriods;
    vector<int> const* firstEpochPeriodsUpperBoundPtr;
};


#endif // ETTPINIT_H
