
#include <cmath>
#include <iostream>
#include <stdlib.h>

using namespace std;


extern void testMOEA(int datasetIndex, string const& outputDir);



int main(int argc, char* argv[])
{
    if (argc != 3) {
        cout << "Usage: ./exes <test set index [0..N]> <output directory>" << endl;
        cout << "   Example: ./exes 0 ./Run1/" << endl;
    }
    int datasetIndex = atoi(argv[1]);
    string outputDir = argv[2];

    testMOEA(datasetIndex, outputDir);

    return 0;
}












