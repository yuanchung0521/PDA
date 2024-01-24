#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stack>
using namespace std;

struct GRAPH;
struct RECORD;
struct NODE; // net
struct EDGE; // mos
struct NET;

struct NET {
    string Name;
    float Right, Left, Y;
    bool Other;
    
    NET (string name) : Name(name), Right(0), Left(0), Y(-1), Other(false) {}

    void setLoc(bool other, float x, float y) {
        // fout << "setloc" << endl;
        // fout << Name << " " << Right << " " << Left << " " << other << " " << Other << " " << Y << " " << y << endl;
        if(Left == 0) {
            Left = x;
            Right = x;
        }
        else {
            Right = x;
        }

        if (Y == -1) {
            Other = other;
            Y = 0;
        }
        else if (Y == 0) {
            if (Other ^ other) Y = y;
        }
        // fout << Other << " " << Right << " " << Left << " " << Y << endl;
        // fout << (Other ^ other) << endl;
    }

};

struct NODE {
    string Name;
    vector<EDGE*> EdgeList;
    NET* Net;
    
    NODE (string name, NET* net) : Name(name), Net(net) {}
    
};

struct EDGE {
    string Name;
    string Gate;
    NODE* Source;
    NODE* Drain;
    bool Visited; // New flag to mark visited edges
    bool Fliped; // defalut false: drain -> gate -> source

    EDGE (string name, string gate, NODE* source, NODE* drain) : Name(name), Gate(gate), Source(source), Drain(drain), Visited(false), Fliped(false) {}

    void Flip();
};

struct RECORD {
    vector<EDGE> ThisMosList;
    vector<EDGE> OtherMosList;
    float HPWL;

    RECORD(): HPWL(__INT_MAX__) {}
    void printRecord(ofstream& fout, bool P) {
        fout << HPWL << endl;
        if (P) {
            for (int i=0; i<ThisMosList.size(); i++) {
                if (ThisMosList[i].Name.find("Dummy") != string::npos) fout << "Dummy ";
                else fout << ThisMosList[i].Name << " ";
            }
            fout << endl;

            fout << ThisMosList[0].Drain->Name << " ";
            for (int i=0; i<ThisMosList.size(); i++) {
                fout << ThisMosList[i].Gate << " " << ThisMosList[i].Source->Name << " ";
            }
            fout << endl;

            for (int i=0; i<OtherMosList.size(); i++) {
                if (OtherMosList[i].Name.find("Dummy") != string::npos) fout << "Dummy ";
                else fout << OtherMosList[i].Name << " ";
            }
            fout << endl;

            fout << OtherMosList[0].Drain->Name << " ";
            for (int i=0; i<OtherMosList.size(); i++) {
                fout << OtherMosList[i].Gate << " " << OtherMosList[i].Source->Name << " ";
            }
            fout << endl;
        }
        else {

            for (int i=0; i<OtherMosList.size(); i++) {
                if (OtherMosList[i].Name.find("Dummy") != string::npos) fout << "Dummy ";
                else fout << OtherMosList[i].Name << " ";
            }
            fout << endl;

            fout << OtherMosList[0].Drain->Name << " ";
            for (int i=0; i<OtherMosList.size(); i++) {
                fout << OtherMosList[i].Gate << " " << OtherMosList[i].Source->Name << " ";
            }
            fout << endl;

            for (int i=0; i<ThisMosList.size(); i++) {
                if (ThisMosList[i].Name.find("Dummy") != string::npos) fout << "Dummy ";
                else fout << ThisMosList[i].Name << " ";
            }
            fout << endl;

            fout << ThisMosList[0].Drain->Name << " ";
            for (int i=0; i<ThisMosList.size(); i++) {
                fout << ThisMosList[i].Gate << " " << ThisMosList[i].Source->Name << " ";
            }
            fout << endl;
        }
    }
};

struct GRAPH {
    bool Type; // 0 for nmos, 1 for pmos
    float Y;
    int EulerCount, NewDummyCount, MinNewDummyCount;
    unordered_map<string, NODE*> NodeMap;
    unordered_map<string, EDGE*> EdgeMap;
    unordered_map<string, vector<EDGE*>> EdgeGateMap;
    vector<EDGE*> EulerPath;
    RECORD BestPath;

    GRAPH(bool type) : Y(0), Type(type), EulerCount(0), NewDummyCount(0), MinNewDummyCount(__INT_MAX__), BestPath() {}
    void printGraph();
    NODE* checkEuler();
    int countEulerChoices();
    void performDFS(NODE* currentNode, GRAPH* otherGraph, unordered_map<string, NET*>& netMap);
    void otherGraphPathGenerator(GRAPH* otherGraph, int index, unordered_map<string, NET*>& netMap);
    RECORD calHPWL(GRAPH* otherGraph, unordered_map<string, NET*>& netMap);

};