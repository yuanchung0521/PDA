#include "classtype.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime> 
#include <set>
#include <map>
#include <algorithm>
using namespace std;

void FastSA (BST& bst, unordered_map<string, BLOCK*>& BlockMap, ofstream& OutputFile);
ANSWER SA (BST& bst, unordered_map<string, BLOCK*>& BlockMap, ofstream& OutputFile);

int main (int argc, char *argv[]) {
    ifstream InputFile(argv[1]);
    ofstream OutputFile(argv[2]);
    string s;
    float Rl, Ru; // lower and upper bound
    int width, height;
    BLOCK* b;
    unordered_map<string, BLOCK*> BlockMap;
    ANSWER best(0, 0, 0, 0);
    ANSWER now(0, 0, 0, 0);

    InputFile >> Rl >> Ru;
    while (InputFile >> s >> width >> height) {
        b = new BLOCK(s, width, height);
        BlockMap.insert(pair<string, BLOCK*>(s, b));
    }
    
    
    // cout << Rl << " " << Ru << endl;
    BST bst(Rl, Ru);
    bst.Construct(BlockMap);

    // FastSA(bst, BlockMap, OutputFile);
    int epoch = 3;
    
    for (int i=0; i<epoch; i++) {
        now = SA(bst, BlockMap, OutputFile);
        if (i == 0 || ((i > 0) && (now.BestArea < best.BestArea))) {
            best = now;
        }
    }
    while (best.BestRatio == 0) {
        best = SA(bst, BlockMap, OutputFile);
    }

    OutputFile << "A = " << best.BestArea << endl;
    OutputFile << "R = " << best.BestRatio << endl;

    for (BLOCK block : best.BestResult) {
        OutputFile << block.Name << " " << block.Pos.first << " " << block.Pos.second << ((block.Rotate) ? " R" : "")<< endl;
    }
    return 0;
}