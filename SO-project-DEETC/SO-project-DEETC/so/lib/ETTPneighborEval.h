#ifndef ETTPNEIGHBOREVAL_H
#define ETTPNEIGHBOREVAL_H

#include <eval/moEval.h>
#include "ETTPneighbor.h"


template <typename TNeighbor>
class ETTPneighborEval : public moEval<TNeighbor> {

public:

    virtual void operator()(typename TNeighbor::EOT & timetable, TNeighbor & neighbor) {

//        cout << endl << "ETTPneighborEval" << endl;

//            // Nothing to do because neighbor has already a fitness value


//        PlacedExamsCostEval<eoChromosome> placedExamsCostEval;
//        placedExamsCostEval(neighbor.)


     }

};


#endif // ETTPNEIGHBOREVAL_H
