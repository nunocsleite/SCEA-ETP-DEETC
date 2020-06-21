#ifndef EOETTPNUMBERCLASHESEVAL_H
#define EOETTPNUMBERCLASHESEVAL_H


#include "eoChromosome.h"



//// Evaluation of objective function
//class eoETTPNumberClashesEval : public eoEvalFunc<eoChromosome> {

//public:

//    void operator()(eoChromosome& _chrom) {
//        // Compute number of clashes
//        _chrom.computeNumClashesDEETCProblem();
//        // Set solution's fitness
//        _chrom.fitness(_chrom.getNumClashesDEETCProblem());
//    }
//};



// Evaluation of objective function
template <typename EOT>
class eoETTPNumberClashesEval : public eoEvalFunc<EOT> {

public:

    void operator()(EOT& _chrom) {
        // Compute number of clashes
        _chrom.computeNumClashesDEETCProblem();
        // Set solution's fitness
        _chrom.fitness(_chrom.getNumClashesDEETCProblem());
    }
};


#endif // EOETTPNUMBERCLASHESEVAL_H
