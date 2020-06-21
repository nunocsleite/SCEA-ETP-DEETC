

#include "eoExtendedChromosome.h"
#include <iostream>

using namespace std;


ostream& operator<<(ostream& os, const eoExtendedChromosome& timetable) {
    os << timetable.getChromosome();
    return os;
}



