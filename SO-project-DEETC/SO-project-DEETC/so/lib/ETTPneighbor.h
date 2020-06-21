#ifndef ETTPNEIGHBOR_H
#define ETTPNEIGHBOR_H

#include <neighborhood/moNeighbor.h>
//#include "Chromosome.h"
#include "ETTPKempeChainHeuristic.h"

#include "ProximityCostEval.h"


///**
// * Neighbor computation for ETTP problem (used in local search)
// */
template<class EOT, class Fitness = typename EOT::Fitness>
class ETTPneighborProximityCost: virtual public moNeighbor<EOT, typename EOT::Fitness> {


public:

    using moNeighbor<EOT, typename EOT::Fitness>::fitness;

    /**
     * Return the class Name
     * @return the class name as a std::string
     */
    virtual std::string className() const {
        return "ETTPneighbor";
    }

    void setSolution(EOT& _timetable) {
        /// TODO
        // COPY ??

        timetable = _timetable; /// Operator= hum

        // Kempe Chain neighbourhood
        ETTPKempeChainHeuristic<EOT> kempeChain;
        kempeChain(timetable);

        ProximityCostEval<EOT> proximityCostEval;
        proximityCostEval(timetable);


//        PlacedExamsCostEval<eoChromosome> placedExamsEval;
//        placedExamsEval(timetable);
//        eval(timetable);

//        cout << "Neigbour fitness = " << timetable.fitness() << endl;

        // Set fitness
        fitness(timetable.fitness());
    }

    ///
    /// ADDED IN ORDER TO SUPPORT H2 CONSTRAINT IN TWO/EPOCH PROBLEM
    ///
    ///
    void setSolution(EOT& _timetable, vector<int> const* _firstEpochPeriodsUpperBound) {
        /// TODO
        // COPY ??

        timetable = _timetable; /// Operator= hum


        // Kempe Chain neighbourhood
        ETTPKempeChainHeuristic<EOT> kempeChain(_firstEpochPeriodsUpperBound); /// ADDED
        kempeChain(timetable);

        ProximityCostEval<EOT> proximityCostEval;
        proximityCostEval(timetable);


//        PlacedExamsCostEval<eoChromosome> placedExamsEval;
//        placedExamsEval(timetable);
//        eval(timetable);

//        cout << "Neigbour fitness = " << timetable.fitness() << endl;

        // Set fitness
        fitness(timetable.fitness());
    }



    /**
     * Apply move on a solution
     * @param _sol the solution to move
     */
    virtual void move(EOT& _timetable) {

        ///
        /// ALTERAR
        ///

        _timetable = timetable;

    }

private:
    EOT timetable;
};




#endif // ETTPNEIGHBOR_H


