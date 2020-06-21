#ifndef EOCHROMOSOME_H
#define EOCHROMOSOME_H


#include "Chromosome.h"
//#include <eo>

#include <EO.h>


// EO Chromosome implementation.
class eoChromosome : public EO<double>, public Chromosome {

public:


    /// TODO: OPTIMIZAR - USAR PONTEIRO PARA CROMOSOMA

    // Copy constructor
    eoChromosome (Chromosome const& _chrom)
        : Chromosome(_chrom)
    {
        computeProximityCosts();
    }

    eoChromosome () { }

    eoChromosome (const Data& data) { init(data); }

    string className() const { return "EO Chromosome"; }

//    void updateProximityCost(double proximityCost) {
//        computeProximityCosts();
//    }

    /*virtual*/ void computeProximityCosts() {
        baseComputeProximityCosts();
    }

    /*virtual*/ void computeNumClashes() {
        baseComputeNumClashes();
    };

    /*virtual*/ void computeNumClashesDEETCProblem() {
        baseComputeNumClashesDEETCProblem();
    };


    friend ostream& operator<<(ostream& os, const eoChromosome& timetable);

};


#endif // EOCHROMOSOME_H
