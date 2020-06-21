#ifndef EOEXTENDEDCHROMOSOME_H
#define EOEXTENDEDCHROMOSOME_H


#include "ExtendedChromosome.h"
#include <EO.h>


// EO Extended Chromosome implementation.
class eoExtendedChromosome : public EO<double>, public ExtendedChromosome {

public:

    /// TODO: OPTIMIZAR - USAR PONTEIRO PARA CROMOSOMA

    // Copy constructor
    eoExtendedChromosome (ExtendedChromosome const& _chrom)
        : ExtendedChromosome(_chrom)
    {
        computeProximityCosts();
    }

    eoExtendedChromosome () { }

    eoExtendedChromosome (const Data& data) { init(data); }

    string className() const { return "EO ExtendedChromosome"; }

    /*virtual*/ void computeProximityCosts() {
        baseComputeProximityCosts();
    }

    /*virtual*/ void computeNumClashes() {
        baseComputeNumClashes();
    };

    /*virtual*/ void computeNumClashesDEETCProblem() {
        baseComputeNumClashesDEETCProblem();
    };


    friend ostream& operator<<(ostream& os, const eoExtendedChromosome& timetable);

};


#endif // EOEXTENDEDCHROMOSOME_H
