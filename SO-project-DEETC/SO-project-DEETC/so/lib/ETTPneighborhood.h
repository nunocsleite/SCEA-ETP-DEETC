#ifndef ETTPNEIGHBORHOOD_H
#define ETTPNEIGHBORHOOD_H

#include <neighborhood/moNeighborhood.h>
#include <neighborhood/moNeighbor.h>
#include "ETTPneighbor.h"
#include <utils/eoRNG.h>

using namespace boost;


template <typename TNeighbor>
class ETTPneighborhood : public moNeighborhood<TNeighbor> {

public:

    ///
    /// ADDED IN ORDER TO SUPPORT H2 CONSTRAINT IN TWO/EPOCH PROBLEM
    ///
    ///
    ETTPneighborhood(vector<int> const* _firstEpochPeriodsUpperBound) :
        feasibleNeigh(true),
        firstEpochPeriodsUpperBound(_firstEpochPeriodsUpperBound) { }


    ETTPneighborhood() :
        feasibleNeigh(true),
        firstEpochPeriodsUpperBound(nullptr) { }


    //
    // Public methods
    //
    void randomNeighbor(typename moNeighborhood<TNeighbor>::EOT& timetable, TNeighbor& neighbor) {
        if (firstEpochPeriodsUpperBound == nullptr)
            neighbor.setSolution(timetable);
        else
            neighbor.setSolution(timetable, firstEpochPeriodsUpperBound);
        feasibleNeigh = true;
    }


    /**
     * @return true if the neighborhood is random (default false)
     */
    virtual bool isRandom() {
        return true;
    }


    /**
     * Test if a solution has (again) a Neighbor
     * @param _solution the related solution
     * @return true if _solution has a Neighbor
     */
    virtual bool hasNeighbor(typename moNeighborhood<TNeighbor>::EOT & _solution) {
        // In the beginning, this variable is true in order to call randomNeighbor method
        return feasibleNeigh;
    }

    /**
     * Initialization of the neighborhood
     * @param _solution the solution to explore
     * @param _current the first neighbor
     */
    virtual void init(typename moNeighborhood<TNeighbor>::EOT & _solution, TNeighbor & _current) {
        randomNeighbor(_solution, _current);
    }

    /**
     * Give the next neighbor
     * @param _solution the solution to explore
     * @param _current the next neighbor
     */
    virtual void next(typename moNeighborhood<TNeighbor>::EOT & _solution, TNeighbor & _current) {
        randomNeighbor(_solution, _current);
    }

    /**
     * Test if there is again a neighbor
     * @param _solution the solution to explore
     * @return true if there is again a neighbor not explored
     */
    virtual bool cont(typename moNeighborhood<TNeighbor>::EOT & _solution) {
        return true;                                /// TODO: eager or lazy?
    }

    /**
     * Return the class Name
     * @return the class name as a std::string
     */
    virtual std::string className() const {
        return "ETTPNeighborhood";
    }

private:
    bool feasibleNeigh;
    vector<int> const* firstEpochPeriodsUpperBound;
};


#endif // ETTPNEIGHBORHOOD_H




