#ifndef PROXIMITYCOSTEVAL_H
#define PROXIMITYCOSTEVAL_H

#include <eoEvalFunc.h>


// Single-objective (minimize number of clashes) evaluation
// Used in Local Search exploration
//class ProximityCostEval : public eoEvalFunc<Chromosome> {
template <typename EOT>
class ProximityCostEval : public eoEvalFunc<EOT> {

public:
    void operator () (EOT& _chrom) {

//        chrom.computeProximityCosts(); // shall include this, hum


//        cout << endl << "ProximityCostEval" << endl;

        // Evaluate proximity cost
        _chrom.fitness(_chrom.getProximityCost());
    }
};





template <typename EOT>
class PlacedExamsCostEval : public eoEvalFunc<EOT> {

public:
    void operator () (EOT& _chrom) {

//        chrom.computeProximityCosts(); // shall include this, hum

        // Evaluate proximity cost
//        chrom.fitness(chrom.getProximityCost());

//        cout << endl << "PlacedExamsCostEval" << endl;

//        _chrom.fitness(_chrom.getNumPlacedExams());



    }
};


#endif // PROXIMITYCOSTEVAL_H
