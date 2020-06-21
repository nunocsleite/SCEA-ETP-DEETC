#ifndef MOTAEXPLORER_H
#define MOTAEXPLORER_H


#include <explorer/moNeighborhoodExplorer.h>
#include <comparator/moSolNeighborComparator.h>
#include <coolingSchedule/moCoolingSchedule.h>
#include <neighborhood/moNeighborhood.h>


/**
 * Explorer for the Threshold Accepting algorithm
 * Fitness must be > 0
 *
 */
template <class Neighbor>
class moTAexplorer : public moNeighborhoodExplorer<Neighbor>
{
public:
    typedef typename Neighbor::EOT EOT;
    typedef moNeighborhood<Neighbor> Neighborhood;

    using moNeighborhoodExplorer<Neighbor>::neighborhood;
    using moNeighborhoodExplorer<Neighbor>::eval;
    using moNeighborhoodExplorer<Neighbor>::selectedNeighbor;

    /**
     * Constructor
     * @param _neighborhood the neighborhood
     * @param _eval the evaluation function
     * @param _solNeighborComparator a solution vs neighbor comparator
     * @param _coolingSchedule the cooling schedule
     * @param _qmax the starting threshold
     */
  moTAexplorer(Neighborhood& _neighborhood, moEval<Neighbor>& _eval,
               moSolNeighborComparator<Neighbor>& _solNeighborComparator, moCoolingSchedule<EOT>& _coolingSchedule/*,
               double _qmax*/)
      : moNeighborhoodExplorer<Neighbor>(_neighborhood, _eval),
        solNeighborComparator(_solNeighborComparator), coolingSchedule(_coolingSchedule)/*, qmax(_qmax)*/ {

        isAccept = false;

        if (!neighborhood.isRandom()) {
            std::cout << "moTAexplorer::Warning -> the neighborhood used is not random" << std::endl;
        }
    }

    /**
     * Destructor
     */
    ~moTAexplorer() { }


    /**
     * initialization of the initial temperature
     * @param _solution the solution
     */
    virtual void initParam(EOT & _solution) {
//        temperature = coolingSchedule.init(_solution);
        q = coolingSchedule.init(_solution);
//        q = qmax; /* Starting threshold */

        isAccept = false;

//        cout << "Initial solution: " << _solution.fitness() << endl;


    };

    /**
     * decrease the temperature if necessary
     * @param _solution unused solution
     */
    virtual void updateParam(EOT & _solution) {
//        cout << "Before Temperature = " << temperature << endl;
//        coolingSchedule.update(temperature, this->moveApplied());
//        cout << "After Temperature = " << temperature << endl;

        // q = g(q); /* Threshold update */
        coolingSchedule.update(q, this->moveApplied());
    }

    /**
     * terminate: NOTHING TO DO
     * @param _solution unused solution
     */
    virtual void terminate(EOT & _solution) { }

    /**
     * Explore one random solution in the neighborhood
     * @param _solution the solution
     */
    virtual void operator()(EOT & _solution) {
//      cout << "TA" << endl;

        // Test if _solution has a Neighbor
        if (neighborhood.hasNeighbor(_solution)) {
//            cout << "hasNeighbor" << endl;

            // Init on the first neighbor: supposed to be random solution in the neighborhood
            neighborhood.init(_solution, selectedNeighbor);
            // Eval the _solution moved with the neighbor and stock the result in the neighbor
            eval(_solution, selectedNeighbor);
        }
        else {
            // If _solution hasn't neighbor,
            isAccept = false;
        }
    };

    /**
     * continue if the temperature is not too low
     * @param _solution the solution
     * @return true if the criteria from the cooling schedule is true
     */
    virtual bool isContinue(EOT & _solution) {
        /* e.g. a given number of iterations executed at each threshold Q */
        return coolingSchedule(q);
    }

    ///////////////////////////////////////////////////////////////////////////
    //  Algorithm 2.4 Template of threshold accepting algorithm.
    //  Input: Threshold annealing.
    //  s = s0 ; /∗ Generation of the initial solution ∗/
    //  Q = Qmax ; /∗ Starting threshold ∗/
    //  Repeat
    //      Repeat /∗ At a fixed threshold ∗/
    //          Generate a random neighbor s′ ∈ N(s) ;
    //          E = f (s′ ) − f (s) ;
    //          If E ≤ Q Then s = s′ /∗ Accept the neighbor solution ∗/
    //      Until Equilibrium condition
    //      /∗ e.g. a given number of iterations executed at each threshold Q ∗/
    //      Q = g(Q) ; /∗ Threshold update ∗/
    //  Until Stopping criteria satisfied /∗ e.g. Q ≤ Qmin ∗/
    //  Output: Best solution found.
    ///////////////////////////////////////////////////////////////////////////

    /**
     * acceptance criterion of Threshold Accepting algorithm
     * @param _solution the solution
     * @return true if f (s′ ) − f (s) ≤ Q
     */
    virtual bool accept(EOT & _solution) {
//        cout << "accept method" << endl;

        if (neighborhood.hasNeighbor(_solution)) {
//            if (solNeighborComparator(_solution, selectedNeighbor)) { // accept if the current neighbor is better than the solution

            double e = selectedNeighbor.fitness() - _solution.fitness();

            if (e <= q) { // Minimization problem
                isAccept = true;
            }
            else {
                isAccept = false;
            }

//            cout << "solution: " << _solution.fitness() << " neighbour: " << selectedNeighbor.fitness() << " level = " << level << endl;

//            cout << "solution: " << _solution.fitness() << ", q = " << q << ", solution.cost: " << _solution.cost << endl;
//            cout << "solution: " << _solution.fitness() << ", q = " << q << endl;

//            cout << _solution.fitness() << " ";
//            cout << "solution: " << _solution.fitness() << " proximity cost: " << _solution.proximityCostStandard << " neighbour: " << selectedNeighbor.fitness() << ", q = " << q << endl;

//            cout << "solution: " << _solution.fitness() << " neighbour: " << selectedNeighbor.fitness()
//                 << ", q = " << q << endl;


        }
        return isAccept;
    };


private:
    // comparator betwenn solution and neighbor
    moSolNeighborComparator<Neighbor>& solNeighborComparator;
    // true if the move is accepted
    bool isAccept;
    // TA parameters
//    double qmax; // Threshold annealing
    double q; // Current threshold

    moCoolingSchedule<EOT>& coolingSchedule;
};


#endif // MOTAEXPLORER_H
