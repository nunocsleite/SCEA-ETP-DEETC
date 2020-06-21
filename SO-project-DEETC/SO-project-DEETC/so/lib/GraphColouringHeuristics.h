#ifndef GRAPH_COLOURING_HEURISTIC
#define GRAPH_COLOURING_HEURISTIC

#include "Common.h"
#include <boost/random.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "VertexPriorityQueue.h"
#include "boost/unordered_set.hpp"
#include "Chromosome.h"
#include <utility>
#include <ctime>
#include <boost/dynamic_bitset.hpp>

using namespace std;
using namespace boost;


//#define _DEBUG




class GCHeuristics {

public:
    static void SD(Matrix const& _conflictMatrix, AdjacencyList const& _graph, Chromosome& _timetable);

    static void SD(Matrix const& _conflictMatrix, AdjacencyList const& _graph, Chromosome& _timetable,
                   vector<int> const& _firstEpochPeriodsUpperBound);


    static void SD(Matrix const& conflictMatrix, AdjacencyList const& graph, Chromosome& timetable,
                   vector<int> const& fixedExams, boost::unordered_map<int, int> const& examsPeriods, vector<int> & remainderExams);

    static void insertFixedExams(vector<boost::unordered_set<int> >& periods, vector<bool>& removed_exams, int numPeriods,
                                 VertexPriorityQueue& pq, vector<boost::unordered_set<int> >& examsAvailablePeriodsList,
                                 vector<int> const& fixedExams, boost::unordered_map<int, int> const& examsPeriods,
                                 vector<int> &remainderExams, int N, AdjacencyList const& graph);
};





#endif
