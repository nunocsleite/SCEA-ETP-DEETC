

#include "eoChromosome.h"
#include <iostream>

using namespace std;


ostream& operator<<(ostream& os, const eoChromosome& timetable) {
    os << timetable.getChromosome();
    return os;
}


