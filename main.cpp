#include "line_percolation_projective_plane.h"
#include <typeinfo>
#include <stdlib.h>
using namespace std;

int main(int argc, char *argv[]){
    if (argc != 4){
        cout << "Incorrect number of arguments (" << argc - 1 << ") given, 3 expected" << endl;
        return 1;
    }
    int planeOrder = atoi(argv[1]);
    int infectionRate = atoi(argv[2]);
    int rounds = atoi(argv[3]);
    
    ProjectivePlane plane(planeOrder);
    randomSetSimulation(plane, infectionRate, rounds, 
                        true,
                        "test.log");
    return 0;
}
