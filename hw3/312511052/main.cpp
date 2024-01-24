#include "classtype.h"
#include <iostream>
#include <sstream>
using namespace std;

void readFile (ifstream& file, GRAPH* ngraph, GRAPH* pgraph, unordered_map<string, NET*>& netMap) {
    string line;
    getline(file, line);
    EDGE* newEdge;
    NODE* newDrain;
    NODE* newSource;
    NET* newNetSource;
    NET* newNetDrain;
    GRAPH* graph;
    float NWidth=0, PWidth=0;
    while (getline(file, line)) {
        if (line.compare(".ENDS") == 0) break;
        else {
            string name, source, gate, drain, body, type, w, l, nfin;
            istringstream iss(line);
            vector<EDGE*> tmpVector;
            iss >> name >> drain >> gate >> source >> body >> type >> w >> l >> nfin;
            name = name.substr(1);
            graph = (type.compare("nmos_rvt") == 0) ? ngraph : pgraph;
            if ((type.compare("nmos_rvt") == 0) && (NWidth == 0)) {
                size_t equalSignPos = w.find('=');
                size_t dotPos = w.find('.');
                NWidth = stof(w.substr(equalSignPos + 1, dotPos - equalSignPos - 1));
            }
            else if ((type.compare("pmos_rvt") == 0) && (PWidth == 0)) {
                size_t equalSignPos = w.find('=');
                size_t dotPos = w.find('.');
                PWidth = stof(w.substr(equalSignPos + 1, dotPos - equalSignPos - 1));
            }
            
            newNetSource = (netMap.find(source) == netMap.end()) ? new NET(source) : netMap[source];
            newNetDrain = (netMap.find(drain) == netMap.end()) ? new NET(drain) : netMap[drain];
            newSource = (graph->NodeMap.find(source) == graph->NodeMap.end()) ? new NODE(source, newNetSource) : graph->NodeMap[source];
            newDrain = (graph->NodeMap.find(drain) == graph->NodeMap.end()) ? new NODE(drain, newNetDrain) : graph->NodeMap[drain];
            newEdge = new EDGE(name, gate, newSource, newDrain);

            if (graph->EdgeGateMap.find(gate) == graph->EdgeGateMap.end()) graph->EdgeGateMap.insert(pair<string, vector<EDGE*>>(gate, tmpVector));
            graph->EdgeGateMap[gate].push_back(newEdge);
            
            newSource->EdgeList.push_back(newEdge);
            newDrain->EdgeList.push_back(newEdge);
            
            graph->NodeMap.insert(pair<string, NODE*>(source, newSource));
            graph->NodeMap.insert(pair<string, NODE*>(drain, newDrain));
            graph->EdgeMap.insert(pair<string, EDGE*>(name, newEdge));
            netMap.insert(pair<string, NET*>(source, newNetSource));
            netMap.insert(pair<string, NET*>(drain, newNetDrain));
        }
    }
    ngraph->Y = NWidth/2 + PWidth/2 + 27;
    pgraph->Y = NWidth/2 + PWidth/2 + 27;
}

int main (int argc, char *argv[]) {

    ifstream InputFile(argv[1]);
    ofstream OutputFile(argv[2]);

    GRAPH* NmosGraph = new GRAPH(false);
    GRAPH* PmosGraph = new GRAPH(true);
    unordered_map<string, NET*> NetMap;

    readFile(InputFile, NmosGraph, PmosGraph, NetMap);

    NODE* PmosStartNode;
    NODE* NmosStartNode;

    // PmosGraph->printGraph();
    // NmosGraph->printGraph();
    
    if (NmosGraph->countEulerChoices() > PmosGraph->countEulerChoices()) {
        PmosStartNode = PmosGraph->checkEuler();
        PmosGraph->performDFS(PmosStartNode, NmosGraph, NetMap);
        // cout << PmosStartNode->Name << " " << PmosGraph->EulerCount << endl;
        PmosGraph->BestPath.printRecord(OutputFile, true);
    }
    else {
        NmosStartNode = NmosGraph->checkEuler();
        NmosGraph->performDFS(NmosStartNode, PmosGraph, NetMap);
        // cout << NmosStartNode->Name << " " << NmosGraph->EulerCount << endl;
        NmosGraph->BestPath.printRecord(OutputFile, false);
    }

    return 0;
}