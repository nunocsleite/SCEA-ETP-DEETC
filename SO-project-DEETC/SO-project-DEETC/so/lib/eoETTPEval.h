#ifndef EOETTPEVAL_H
#define EOETTPEVAL_H

#include "eoChromosome.h"



/// Original code


//// Evaluation of objective function
//class eoETTPEval : public eoEvalFunc<eoChromosome> {
//public:
//    void operator()(eoChromosome& _chrom) {

////        cout << "eoETTPEval::operator()()" << endl;

//        _chrom.fitness(proximityCost(_chrom));

////        proximityCost(_chrom); // ???? don't use the return value
//    }

//    inline double proximityCost(eoChromosome& _chrom) {
//        _chrom.computeProximityCosts();

////        cout << "_chrom.getRange()[1] = " << _chrom.getRange()[1] << endl;
////        cout << "_chrom.getNumPeriods() = " << _chrom.getNumPeriods() << endl;

////        cin.get();
////LAST
////        if (_chrom.getNumPeriods() > _chrom.getRange()[1] //|| _chrom.getNumPeriods() < _chrom.getRange()[0]/)
////            return _chrom.getProximityCost() + abs(_chrom.getNumPeriods() - _chrom.getRange()[1])*1000; // Penalize solution


//////        if (_chrom.getNumPeriods() > _chrom.getRange()[1] //|| _chrom.getNumPeriods() < _chrom.getRange()[1]/)
////////            return _chrom.getProximityCost() + abs(_chrom.getNumPeriods() - _chrom.getRange()[1])*1000; // Penalize solution
//////            return _chrom.getProximityCost() + 0.1*abs(_chrom.getNumPeriods() - _chrom.getRange()[1]); // Penalize solution
////        else
////            return _chrom.getProximityCost();
////            return _chrom.getNumPeriods();
////            return _chrom.getProximityCost()*10 / _chrom.getNumPeriods(); /// ADDED 17-April-2014
////            return _chrom.getProximityCost() + _chrom.getNumPeriods(); // NOT GOOD
////            return _chrom.getProximityCost() / _chrom.getNumPeriods(); // SAME AS USING JUST THE COST



//        return _chrom.getProximityCost();


////        if (chrom.getNumPeriods() > chrom.getRange()[1])
////            return chrom.getNumClashes() + (chrom.getNumPeriods() - chrom.getRange()[1])*1000; // Penalize solution
////        else
////            return chrom.getNumClashes();

//        /// TODO: HOW TO PENALIZE SOLUTION?

////        if (_chrom.getNumPeriods() > _chrom.getRange()[1])
////            return _chrom.getProximityCost() + (_chrom.getNumPeriods() - _chrom.getRange()[1])*0.2; // Penalize solution
////        else
////            return _chrom.getProximityCost();


//        // LAST
////        return _chrom.getProximityCost();

//    }
//};



// Evaluation of objective function
template <typename EOT>
class eoETTPEval : public eoEvalFunc<EOT> {
public:
    void operator()(EOT& _chrom) {

//        cout << "eoETTPEval::operator()()" << endl;

        _chrom.fitness(proximityCost(_chrom));
    }

    inline double proximityCost(EOT& _chrom) {
        _chrom.computeProximityCosts();
        return _chrom.getProximityCost();
    }
};



#endif // EOETTPEVAL_H
