/*
  <moSAexplorer.h>
  Copyright (C) DOLPHIN Project-Team, INRIA Lille - Nord Europe, 2006-2010

  Sébastien Verel, Arnaud Liefooghe, Jérémie Humeau

  This software is governed by the CeCILL license under French law and
  abiding by the rules of distribution of free software.  You can  use,
  modify and/ or redistribute the software under the terms of the CeCILL
  license as circulated by CEA, CNRS and INRIA at the following URL
  "http://www.cecill.info".

  As a counterpart to the access to the source code and  rights to copy,
  modify and redistribute granted by the license, users are provided only
  with a limited warranty  and the software's author,  the holder of the
  economic rights,  and the successive licensors  have only  limited liability.

  In this respect, the user's attention is drawn to the risks associated
  with loading,  using,  modifying and/or developing or reproducing the
  software by the user in light of its specific status of free software,
  that may mean  that it is complicated to manipulate,  and  that  also
  therefore means  that it is reserved for developers  and  experienced
  professionals having in-depth computer knowledge. Users are therefore
  encouraged to load and test the software's suitability as regards their
  requirements in conditions enabling the security of their systems and/or
  data to be ensured and,  more generally, to use and operate it in the
  same conditions as regards security.
  The fact that you are presently reading this means that you have had
  knowledge of the CeCILL license and that you accept its terms.

  ParadisEO WebSite : http://paradiseo.gforge.inria.fr
  Contact: paradiseo-help@lists.gforge.inria.fr
*/

#ifndef _moSAexplorer_h
#define _moSAexplorer_h

#include <cstdlib>

#include <explorer/moNeighborhoodExplorer.h>
#include <comparator/moSolNeighborComparator.h>

#include <coolingSchedule/moCoolingSchedule.h>
#include <neighborhood/moNeighborhood.h>

#include <utils/eoRNG.h>

/**
 * Explorer for the Simulated Annealing
 * Only the symetric case is considered when Q(x,y) = Q(y,x)
 * Fitness must be > 0
 *
 */
template< class Neighbor >
class moSAexplorer : public moNeighborhoodExplorer<Neighbor>
{
public:
    typedef typename Neighbor::EOT EOT ;
    typedef moNeighborhood<Neighbor> Neighborhood ;

    using moNeighborhoodExplorer<Neighbor>::neighborhood;
    using moNeighborhoodExplorer<Neighbor>::eval;
    using moNeighborhoodExplorer<Neighbor>::selectedNeighbor;

    /**
     * Constructor
     * @param _neighborhood the neighborhood
     * @param _eval the evaluation function
     * @param _solNeighborComparator a solution vs neighbor comparator
     * @param _coolingSchedule the cooling schedule
     */
  moSAexplorer(Neighborhood& _neighborhood, moEval<Neighbor>& _eval,
               moSolNeighborComparator<Neighbor>& _solNeighborComparator, moCoolingSchedule<EOT>& _coolingSchedule)
      : moNeighborhoodExplorer<Neighbor>(_neighborhood, _eval), solNeighborComparator(_solNeighborComparator),
        coolingSchedule(_coolingSchedule) {

        isAccept = false;

        if (!neighborhood.isRandom()) {
            std::cout << "moSAexplorer::Warning -> the neighborhood used is not random" << std::endl;
        }
    }

    /**
     * Destructor
     */
    ~moSAexplorer() {
    }

    /**
     * initialization of the initial temperature
     * @param _solution the solution
     */
    virtual void initParam(EOT & _solution) {
      temperature = coolingSchedule.init(_solution);
      isAccept = false;
    };

    /**
     * decrease the temperature if necessary
     * @param _solution unused solution
     */
    virtual void updateParam(EOT & _solution) {
//        cout << "Before Temperature = " << temperature << endl;
        coolingSchedule.update(temperature, this->moveApplied());
//        cout << "After Temperature = " << temperature << endl;
    };

    /**
     * terminate: NOTHING TO DO
     * @param _solution unused solution
     */
    virtual void terminate(EOT & _solution) {};

    /**
     * Explore one random solution in the neighborhood
     * @param _solution the solution
     */
    virtual void operator()(EOT & _solution) {
//      cout << "SA" << endl;

        //Test if _solution has a Neighbor
        if (neighborhood.hasNeighbor(_solution)) {
//            cout << "hasNeighbor" << endl;

            //init on the first neighbor: supposed to be random solution in the neighborhood
            neighborhood.init(_solution, selectedNeighbor);

            //eval the _solution moved with the neighbor and stock the result in the neighbor
            eval(_solution, selectedNeighbor);
        }
        else {
            //if _solution hasn't neighbor,
            isAccept=false;
        }
    };

    /**
     * continue if the temperature is not too low
     * @param _solution the solution
     * @return true if the criteria from the cooling schedule is true
     */
    virtual bool isContinue(EOT & _solution) {
        return coolingSchedule(temperature);
    };

    /**
     * acceptance criterion according to the boltzmann criterion
     * @param _solution the solution
     * @return true if better neighbor or rnd < exp(delta f / T)
     */
    //  % Simulated Annealing (algorithm 2 for minimization of f)
    //  %
    //  % Step 1  Make T = Tmax  and Choose a solution u (at random)
    //  %
    //  % Step 2  Select a neighbor of u, say v
    //  %         If f(v) < f(u) make u = v;
    //  %         Else make u = v with probability
    //  %              p = exp((fu-fv)/(fu T)))
    //  %
    //  %         Repeat Step 2 k times
    //  %
    //  % Step 3  Make t = t+1; Set T = T(t) see Eq.(4)
    //  %
    //  %         If  T < Tmin  go to Step 2;
    //  %         Else go to Step 1.
    //  %
    virtual bool accept(EOT & _solution) {
//        cout << "accept method" << endl;

        if (neighborhood.hasNeighbor(_solution)) {
//            if (solNeighborComparator(_solution, selectedNeighbor)) { // accept if the current neighbor is better than the solution


            double fit1, fit2;
            fit1 = (double)selectedNeighbor.fitness();
            fit2 = (double)_solution.fitness();

            cout << "fit1 (neighbour) = " << fit1 << " fit2 (current solution) = " << fit2
                 << ", T = " << this->temperature << endl;


            if (selectedNeighbor.fitness() < _solution.fitness()) { // Minimization problem
                isAccept = true;
//                cout << "accept because the current neighbor is better than the solution" << endl;
            }
            else {
                double alpha = 0.0;
//                double fit1, fit2;
//                fit1 = (double)selectedNeighbor.fitness();
//                fit2 = (double)_solution.fitness();

//                cout << "temperature = " << temperature << endl;
//                cout << "fit1 (neighbour) = " << fit1 << endl;
//                cout << "fit2 (solution)  = " << fit2 << endl;

//                cin.get();

//                if (fit1 < fit2) // this is a maximization
//                    alpha = exp((fit1 - fit2) / temperature );
//                else // this is a minimization
//                    alpha = exp((fit2 - fit1) / temperature );
//                isAccept = (rng.uniform() < alpha);

//                alpha = exp((fit2 - fit1) / (fit2*0.01) ); // 40.56
//                alpha = exp((fit2 - fit1) / (fit2*2) ); //
                alpha = exp((fit2 - fit1) / (fit2*temperature) );
//                alpha = exp((fit2 - fit1) / temperature );

                double r = rng.uniform();

//                cout << "temperature = " << temperature << endl;
//                cout << "alpha = " << alpha << endl;
//                cout << "rand = " << r << endl;


                isAccept = (r < alpha);
//                isAccept = (rng.uniform() < alpha);

//                cout << "isAccept = " << isAccept << endl;

            }
        }
        return isAccept;
    };

    /**
     * Getter
     * @return the temperature
     */
    double getTemperature() {
        return temperature;
    }

private:
    // comparator betwenn solution and neighbor
    moSolNeighborComparator<Neighbor>& solNeighborComparator;

    moCoolingSchedule<EOT>& coolingSchedule;

  // temperature of the process
    double temperature;

    // true if the move is accepted
    bool isAccept;
};


#endif