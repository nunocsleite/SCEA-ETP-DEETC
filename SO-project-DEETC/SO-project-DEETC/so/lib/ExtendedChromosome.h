#ifndef EXTENDEDCHROMOSOME_H
#define EXTENDEDCHROMOSOME_H


#include "Chromosome.h"


class ExtendedChromosome : public Chromosome {


public:
    ExtendedChromosome() : Chromosome() { }

    ExtendedChromosome(const Data& _data) : Chromosome(_data) { ptrSecondEpochChrom = nullptr; }

//    ExtendedChromosome(const Data& _data, Chromosome const& _secondEpochChrom) : Chromosome(_data), secondEpochChrom(_secondEpochChrom) { }

    // Copy constructor
    ExtendedChromosome(ExtendedChromosome const& _chrom) : Chromosome(_chrom) { ptrSecondEpochChrom = _chrom.ptrSecondEpochChrom; }


    void setSecondEpoch(Chromosome const& _secondEpochChrom) {
        ptrSecondEpochChrom = &_secondEpochChrom;
    }

    void baseComputeProximityCosts() override;

private:
    Chromosome const* ptrSecondEpochChrom;

};

#endif // EXTENDEDCHROMOSOME_H
