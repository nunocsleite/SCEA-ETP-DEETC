#ifndef CROSSOVER_H
#define CROSSOVER_H

#include "eoChromosome.h"
#include <eoOp.h>
#include "ETTPKempeChainHeuristic.h"
#include <utility>
#include <boost/unordered_map.hpp>
#include "ETTPneighborhood.h"
#include "ProximityCostEval.h"
#include "moTA.h"
#include "ETTPneighborEval.h"
#include "moSimpleCoolingSchedule.h"
#include "moGDA.h"
#include "TestSetDescription.h"
#include <eoGenContinue.h>
#include <utils/eoCheckPoint.h>

// For debugging purposes
//#define _CROSSOVER_DEBUG



using namespace std;


//class Crossover : public eoQuadOp<eoChromosome>
template <typename EOT>
class Crossover : public eoQuadOp<EOT>
{

public:

    /**
    * the class name (used to display statistics)
    */
    string className() const;

    /**
    * eoQuad crossover - _chromosome1 and _chromosome2 are the (future) offspring, i.e. _copies_ of the parents
    * @param _chromosome1 the first parent
    * @param _chromosome2 the second parent
    */
    bool operator()(EOT& _chromosome1, EOT& _chromosome2);

private:

    /**
    * generation of an offspring
    * @param _parent1 the first parent
    * @param _parent2 the second parent
    */
    void generateOffspring(EOT &_parent1, EOT &_parent2);
    void generateOffspring2(EOT &_parent1, EOT &_parent2);

    void feasibleExamInsertion(EOT &_chrom, int exam);

    boost::shared_ptr<int> selectDays(const EOT & _chrom);

//    void insertDays2(eoChromosome & _parent1, eoChromosome const& _parent2, boost::shared_ptr<int> _range2);

    void insertDays(EOT &_parent1, const EOT &_parent2, boost::shared_ptr<int> _range2);

    /**
    * best day computation
    * @param chromosome
    */
//    int bestDay(eoChromosome const& chromosome);

    /**
    * insert day into chromosome
    * @param _parent1
    * @param _parent2
    * @param day2
    */
//    void insertDay(eoChromosome& _parent1, eoChromosome& _parent2, int day2);
};




class IsInExamsToRemove {
    vector<int> const& examsToRemove;
public:
    IsInExamsToRemove(vector<int> const& examsToRemove) : examsToRemove(examsToRemove) { }
    bool operator()(int exam) {
        return find_if(examsToRemove.begin(), examsToRemove.end(), bind2nd(equal_to<int>(), exam)) != examsToRemove.end();
    }
};


template <typename EOT>
string Crossover<EOT>::className() const
{
    return "ETTP Crossover";
}

/*
bool Crossover::operator()(eoChromosome& _chromosome1, eoChromosome& _chromosome2)
{
//    cout << "Crossover" << endl;

    bool oneAtLeastIsModified = true;
    //
    // Day-exchange crossover
    //
    // In day-exchange crossover, only the best days (excluding
    // Saturdays, since exams scheduled on Saturdays are always
    // clash-free) of chromosomes, selected based on the crossover
    // rate, are eligible for exchange. The best day consists of three
    // periods and is the day with the lowest number of clashes per student.

    // Computation of the offspring
    generateOffspring(_chromosome1, _chromosome2);
    generateOffspring(_chromosome2, _chromosome1);


//    // does at least one genotype has been modified ?
//    if ((_chromosome1 != offspring1) || (_chromosome2 != offspring2))
//    {
//        // update
//        _chromosome1.value(offspring1);
//        _chromosome2.value(offspring2);
//        // at least one genotype has been modified
//        oneAtLeastIsModified = true;
//    }
//    else
//    {
//        // no genotype has been modified
//        oneAtLeastIsModified = false;
//    }
    // return 'true' if at least one genotype has been modified
    return oneAtLeastIsModified;
}
*/

template <typename EOT>
bool Crossover<EOT>::operator()(EOT& _chromosome1, EOT& _chromosome2)
{
//    cout << "Crossover" << endl;

    bool oneAtLeastIsModified = true;
    // Computation of the offspring
    EOT chrom1 = _chromosome1; // Original chromosome 1
    EOT chrom2 = _chromosome2; // Original chromosome 2
    generateOffspring(_chromosome1, chrom2); // offspring 1 is the resulting chromosome 1
    generateOffspring(_chromosome2, chrom1); // offspring 2 is the resulting chromosome 2

//    generateOffspring2(_chromosome1, chrom2); // offspring 1 is the resulting chromosome 1
//    generateOffspring2(_chromosome2, chrom1); // offspring 2 is the resulting chromosome 2

    return oneAtLeastIsModified;
}


/*

int max_element_idx(vector<unordered_set<int> > const& solutionPeriods) {
    int indexLargest = 0;
    for (int i = 1; i < solutionPeriods.size(); ++i) {
        if (solutionPeriods[i].size() > solutionPeriods[indexLargest].size())
            indexLargest = i;
    }
    return indexLargest;
}


//
// GPX based crossover (for graph colouring)
// (see Memetic Algorithms in Discrete Optimization paper by J.-K. Hao)
//
void Crossover::generateOffspring(eoChromosome & _parent1, eoChromosome & _parent2)
{
    // Original Parent 1 (copy)
    eoChromosome s1 = _parent1;
    // Original Parent 2 (reference)
    eoChromosome& s2 = _parent2;
    // We change directly _parent1 to be the new offspring because it's already a copy
    // of the original parent.
    eoChromosome& offspring = _parent1;
    // Get parent 1 periods
    vector<unordered_set<int> >& s1Periods = s1.getPeriods();
    // Get parent 2 periods
    vector<unordered_set<int> >& s2Periods = s2.getPeriods();
    // Get offspring periods
    vector<unordered_set<int> >& offspringPeriods = offspring.getPeriods();
    // Empty periods in the offspring
    offspringPeriods.clear();


    cout << "Periods sizes:" << endl;
    for (auto it = s1Periods.begin(); it != s1Periods.end(); ++it)
        cout << (*it).size() << " ";
    cout << endl;

    bool firstParent = true;
    vector<unordered_set<int> > * s1PeriodsPtr = 0;
    vector<unordered_set<int> > * s2PeriodsPtr = 0;

    // At the first step, GPX create first offspring period by choosing the largest period
    // from s1 and removes its vertices from both the s1 and s2.
    // GPX repeats then these same operations for the next k-1 steps, but alternates each time
    // the parent considered. If some vertices remain unassigned at the end of these k steps,
    // they are randomly assigned to one of the k periods (classes).
    for (int i = 0; i < s1.getNumPeriods(); ++i) {
        if (firstParent) {
            s1PeriodsPtr = &s1Periods;
            s2PeriodsPtr = &s2Periods;
        }
        else {
            s1PeriodsPtr = &s2Periods;
            s2PeriodsPtr = &s1Periods;
        }

        // Choose the largest period from s1 and insert it in the offspring
        int indexLargest = max_element_idx(*s1PeriodsPtr);

//        cout << "largest period size = " << (*s1PeriodsPtr)[indexLargest].size() << endl;

        // Insert largest period exams in offspring
        offspringPeriods.push_back((*s1PeriodsPtr)[indexLargest]);
//        cout << "Offspring periods:" << endl;

//        for (int i = 0; i < offspringPeriods.size(); ++i)
//            cout << offspringPeriods[i].size() << " ";
//        cout << endl;

        // Removes largest period vertices from both the s1 and s2.
        // Remove from s1
        (*s1PeriodsPtr)[indexLargest].clear();
        // Remove from s2
        unordered_set<int> const& examsToRemove = offspringPeriods.back();

        IsInExamsToRemove pred(examsToRemove);
        // Remove each exam in examsToRemove vector
        for (unordered_set<int>::const_iterator itRem = examsToRemove.begin(); itRem != examsToRemove.end(); ++itRem) {
            int i;
            for (i = 0; i < (*s2PeriodsPtr).size(); ++i) {
                unordered_set<int>& exams = (*s2PeriodsPtr)[i];
                if (exams.find(*itRem) != exams.end()) {
                    exams.erase(*itRem);
                    break;
                }
            }
            if (i == (*s2PeriodsPtr).size()) {
                cout << endl << "numPeriods = " << (*s2PeriodsPtr).size() << endl;
                cout << endl << "Couldn't find exam " << *itRem << endl;
                cin.get();
            }
        }
//        s2.validate();

        // Alternate between parents
        firstParent = !firstParent;

    }

    cout << "s1 Periods sizes:" << endl;
    for (auto it = s1Periods.begin(); it != s1Periods.end(); ++it)
        cout << (*it).size() << " ";
    cout << endl;


    cout << "s2 Periods sizes:" << endl;
    for (auto it = s2Periods.begin(); it != s2Periods.end(); ++it)
        cout << (*it).size() << " ";
    cout << endl;


    cout << "offspring Periods sizes:" << endl;
    for (auto it = offspringPeriods.begin(); it != offspringPeriods.end(); ++it)
        cout << (*it).size() << " ";
    cout << endl;

    cin.get();

    // If some vertices remain unassigned at the end of these k steps,
    // they are randomly assigned to one of the k periods (classes).

/// TODO: Try to insert exams into feasible periods first before using Kempe chain operator

    // Insert remainder exams from s1
    for (auto it = s1Periods.begin(); it != s1Periods.end(); ++it) {
        if ((*it).size() > 0) {
            cout << (*it).size() << " exams to insert" << endl;

            // Insert all exams from this period at random periods in the offspring
            for (auto examIt = (*it).begin(); examIt != (*it).end(); ++examIt) {
                cout << endl << endl << "\tInserting exam " << *examIt << endl;

                cin.get();

                feasibleExamInsertion(offspring, *examIt);

            }
        }
    }

//    cin.get();

    offspring.validate();

    // Actualize proximity costs
    offspring.computeProximityCosts();
}



void Crossover::feasibleExamInsertion(eoChromosome & _chrom, int exam) {

    cout << "_chrom.getNumPeriods() = " << _chrom.getNumPeriods() << endl;
    cout << "Inserting exam " << exam << endl;


    ETTPKempeChainHeuristic kempeHeuristic;

    int exami;
    int ti, tj;

    unordered_set<int>* ti_period;
    unordered_set<int>* tj_period;

    // Select randomly two timeslots, ti and tj.
    do {
        ti = rng.random(_chrom.getNumPeriods());
        do {
            tj = rng.random(_chrom.getNumPeriods());
        }
        while (ti == tj);
    }
    while (_chrom.getPeriods()[ti].size() == 0);

    // Select randomly an exam ei from timeslot ti and move it
    // to timeslot tj. If the solution becomes infeasible,
    // because exam ei has students in common with exams ej, ek, ...,
    // that are assigned to time slot tj, one have to move exams
    // ej, ek, ..., to time slot ti. This process is repeated until all the
    // exams that have students in common are assigned to different time slots.


//    cout << "ti = " << ti << ", tj = " << tj << ", _chrom.getNumPeriods() = " << _chrom.getNumPeriods() << endl;

    vector<unordered_set<int> >& periods = _chrom.getPeriods();

    ti_period = &periods[ti];
    tj_period = &periods[tj];

//    // Generate random index
//    int idx = rng.random(ti_period->size());

////    cout << "idx = " << idx << ", ti_period.size() = " << ti_period.size() << endl;

//    /// TODO - NOT EFFICIENT
//    // Get exam id
//    unordered_set<int>::const_iterator it_i;
//    it_i = ti_period->begin();

//    for (int i = 0; i < idx; ++i, ++it_i)
//        ;

//    exami = *it_i;

    // Assume that exam to insert feasibly into a random time slot is initially at time slot ti
    // This is temporary because exam is going to be moved to time slot tj
    // Note: exam could be infeasible in time slot ti
    ti_period->insert(exam);
    exami = exam;

    // Kempe chain operator
    kempeHeuristic.kempeChainOperator(_chrom, *ti_period, *tj_period, exami);

}

*/


//===
// Abdullah feasible crossover
//===
template <typename EOT>
void Crossover<EOT>::generateOffspring(EOT &_parent1, EOT &_parent2)
{
    // We change directly _parent1 to be the new offspring because it's already a copy
    // of the original parent.
    EOT& offspring = _parent1;


    vector<boost::unordered_set<int> >& p2Periods = _parent2.getPeriods();
    // Get offspring periods
    vector<boost::unordered_set<int> >& offspringPeriods = offspring.getPeriods();

    // Generate random number of periods to insert from Pw
//    int randNumPeriods = rng.uniform(10);

//    int randNumPeriods = 100;
    int randNumPeriods = 3; // BETTER in cGA
//    int randNumPeriods = 1; //LAST used with GDA
    for (int i = 1; i <= randNumPeriods; ++i) {
        // Get a random period from Pw
        int randParent2Period = rng.random(_parent2.getNumPeriods());
        // Insert Pw random period exams (those that are feasible)
        // into the new frog and remove duplicated exams

        // Generate a random index in the new frog
        int randOffspringPeriod = rng.random(offspring.getNumPeriods());
        // Pw exams from best day
        boost::unordered_set<int>& p2Exams = p2Periods[randParent2Period];

        // newPw exams from rand period
        boost::unordered_set<int>& offspringExams = offspringPeriods[randOffspringPeriod];

        // Insert Pw period into newPw into that random position
        for (auto it = p2Exams.begin(); it != p2Exams.end(); ++it) {
            // If the exam does not exist and is feasible then insert it
            if (offspringExams.find(*it) == offspringExams.end()
                    && offspring.isFeasiblePeriodExam(randOffspringPeriod, *it)) {
//                        if (newPwExams.find(*it) == newPwExams.end() && newPw.isFeasiblePeriodExam(bestDayIdx, *it)) {


            offspringExams.insert(*it);
//                ++numInsertedExams;
//                        cout << "Insert: " << *it << endl;
//                        cin.get();


                /// TODO / SEE POSSIBLE OPTIMIZATIONS

                // Remove duplicated exams
                for (int i = 0; i < offspringPeriods.size(); ++i) {
                    if (i != randOffspringPeriod) {
//                        if (i != bestDayIdx) {
                        boost::unordered_set<int>& exams = offspringPeriods[i];
                        if (exams.find(*it) != exams.end()) {
                            exams.erase(*it);
//                            ++numRemovedExams;
//                            cout << "Remove: " << *it << endl;
                            break;
                        }
                    }
                }

            }
//            else {
//                notInserted.insert(*it);
//                ++numNotInsertedExams;
//            }

        }
    }


//    ETTPKempeChainHeuristic kempe;
//    kempe(offspring);

    // Actualize proximity costs
    offspring.computeProximityCosts();
}

#endif // CROSSOVER_H




