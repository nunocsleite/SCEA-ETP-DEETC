#ifndef EOSIMPLEGA_H
#define EOSIMPLEGA_H


// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// eoSGA.h
// (c) GeNeura Team, 2000 - EEAAX 1999 - Maarten Keijzer 2000
/*
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: todos@geneura.ugr.es, http://geneura.ugr.es
             Marc.Schoenauer@polytechnique.fr
             mak@dhi.dk
 */
//-----------------------------------------------------------------------------


#include <eoInvalidateOps.h>
#include <eoContinue.h>
#include <eoPop.h>
#include <eoSelectOne.h>
#include <eoSelectPerc.h>
#include <eoEvalFunc.h>
#include <eoAlgo.h>
#include <apply.h>

//// ADDED 05/May/2014 //////////////////

#include "ETTPneighborhood.h"
#include "ETTPneighborEval.h"
#include "ProximityCostEval.h"
#include "moSimpleCoolingSchedule.h"
#include "moTA.h"

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
extern const std::string currentDateTime();


////////////////////////////////////////////


/** The Simple Genetic Algorithm, following Holland and Goldberg
 *
 * Needs a selector (class eoSelectOne) a crossover (eoQuad, i.e. a
 * 2->2 operator) and a mutation with their respective rates, of
 * course an evaluation function (eoEvalFunc) and a continuator
 * (eoContinue) which gives the stopping criterion. Performs full
 * generational replacement.
 *
 * @ingroup Algorithms
 */
template <class EOT>
class eoSimpleGA : public eoAlgo<EOT>
{
public :

  // added this second ctor as I didn't like the ordering of the parameters
  // in the one above. Any objection :-) MS
  eoSimpleGA(
        eoSelectOne<EOT>& _select,
        eoQuadOp<EOT>& _cross, float _crate,
        eoMonOp<EOT>& _mutate, float _mrate,
        eoEvalFunc<EOT>& _eval,
        eoContinue<EOT>& _cont)
    : cont(_cont),
          mutate(_mutate),
          mutationRate(_mrate),
          cross(_cross),
          crossoverRate(_crate),
          select(_select),
          eval(_eval) {}

  void operator()(eoPop<EOT>& _pop)
  {
    eoPop<EOT> offspring;

    do {
        select(_pop, offspring);

        unsigned i;

        for (i = 0; i < _pop.size()/2; i++)
        {
            if ( rng.flip(crossoverRate) )
            {
                // this crossover generates 2 offspring from two parents
                if (cross(offspring[2*i], offspring[2*i+1]))
                {
                    offspring[2*i].invalidate();
                    offspring[2*i+1].invalidate();
                }
            }
        }

        for (i=0; i < offspring.size(); i++) {
            if (rng.flip(mutationRate) ) {
                if (mutate(offspring[i]))
                    offspring[i].invalidate();
            }
        }

//        _pop.swap(offspring);
//        apply<EOT>(eval, _pop);


        //////////////////////// ADDED 05/May/2014 FOR TEST PURPOSES ///////////////////////////////////////

        apply<EOT>(eval, offspring);

        ETTPneighborhood<ETTPneighborProximityCost> neighborhood;
        ETTPneighborEval<ETTPneighborProximityCost> neighEval;
        ProximityCostEval<eoChromosome> fullEval;
//        moSimpleCoolingSchedule<eoChromosome> cool(0.1, 0.01, 15, 2e-5); // Yor 35.96...
        moSimpleCoolingSchedule<eoChromosome> cool(0.1, 0.99, 15, 2e-5); // Yor 3...

        moTA<ETTPneighborProximityCost> ta(neighborhood, fullEval, neighEval, cool);


        for (i = 0; i < offspring.size(); i++) {
            if (rng.uniform() < 0.1) {
                ta(offspring[i]);
            }
        }

        // Copying the best individual doesn't converge

//        EOT* bestIndPtr = 0;

//        for (i = 0; i < offspring.size(); i++) {
//            if (rng.uniform() < 0.1) {
//                ta(offspring[i]);
//                // Update best individual
//                if (bestIndPtr == 0 || offspring[i].fitness() < bestIndPtr->fitness())
//                    bestIndPtr = &offspring[i];
//            }
//        }
//        // Update best individual from parent population
//        for (i = 0; i < _pop.size(); i++) {
//            // Update best individual
//            if (bestIndPtr == 0 || _pop[i].fitness() < bestIndPtr->fitness())
//                bestIndPtr = &_pop[i];
//        }

//        // Get best individual
//        EOT bestInd = *bestIndPtr;

//        // Replace pop with offspring
//        _pop.swap(offspring);
//        apply<EOT>(eval, _pop);

//        // Maintain elitism by inserting best indivual in a random position
//        _pop[rng.uniform(_pop.size())] = bestInd;

        // Replace pop with offspring
//        _pop.swap(offspring);
//        apply<EOT>(eval, _pop);

        // Maintain elitism
        for (i = 0; i < _pop.size(); i++) {
            // Copy the best individual
            if (offspring[i].fitness() < _pop[i].fitness())
                _pop[i] = offspring[i];
        }
//        apply<EOT>(eval, _pop);

        // Print population
        cout << endl;
        for (unsigned i = 0; i < _pop.size(); ++i)
            cout << /*_pop[i].fitness()*/ _pop[i].getProximityCost() << " ";

//            for (unsigned i = 0; i < _pop.size (); ++i)
//                cout << _pop[i].cost << " ";

        cout << " Date/Time = " << currentDateTime() << endl;

        // Print population # periods
        cout << endl;
        for (unsigned i = 0; i < _pop.size(); ++i)
            cout << _pop[i].getNumPeriods() << " ";
        cout << endl;
        ///////////////////////////////////////////////////////////////

      } while (cont(_pop));
  }

private :

    eoContinue<EOT>& cont;
    /// eoInvalidateMonOp invalidates the embedded operator
    eoInvalidateMonOp<EOT> mutate;
    float mutationRate;
    // eoInvalidateQuadOp invalidates the embedded operator
    eoInvalidateQuadOp<EOT> cross;
    float crossoverRate;
    eoSelectPerc<EOT> select;
    eoEvalFunc<EOT>& eval;
};


#endif // EOSIMPLEGA_H
