#include "classtype.h"
#include <functional>

void EDGE::Flip() {
    Fliped = !Fliped;
    NODE* tmpNode = Source;
    Source = Drain;
    Drain = tmpNode;
}

void GRAPH::printGraph() {
    cout << "===================== " << Type << " =====================" << endl;
    for (auto& node : NodeMap) {
        cout << node.second->Name << " ( ";
        for (EDGE* edge : node.second->EdgeList) cout << edge->Name << " ";
        cout << ") " << node.second->Net->Name << endl;
    }
    cout << "=============================================" << endl;
}

void GRAPH::performDFS(NODE* currentNode, GRAPH* otherGraph, unordered_map<string, NET*>& netMap) { // use the smallest degree node to be the start one // and use > 2 lesser to be (n or p)
    for (EDGE* edge : currentNode->EdgeList) {
        if (!edge->Visited) {

            edge->Visited = true;
            // cout << currentNode->Name << " " << edge->Drain->Name << " " << (currentNode == edge->Drain) << endl;
            if (currentNode != edge->Drain) edge->Flip();
            EulerPath.push_back(edge);

            if (EulerPath.size() == EdgeMap.size()) {
                EulerCount ++;
                // if print all euler path
                // cout << "Euler Path: ";
                // for (EDGE* edge : EulerPath) {
                //     cout << edge->Name << "(" << edge->Fliped << ")" << " ";
                // } 
                // cout << endl;
                otherGraphPathGenerator(otherGraph, 0, netMap);

            }
            NODE* nextNode = (currentNode == edge->Source) ? edge->Drain : edge->Source;
            performDFS(nextNode, otherGraph, netMap);
            edge->Visited = false;
            EulerPath.pop_back();
        }
    }
}

int GRAPH::countEulerChoices() {
    int EulerChoices = 0;
    for (auto& node : NodeMap) {
        if (node.second->EdgeList.size() > 2) EulerChoices++;
    }
    return EulerChoices;
}

NODE* GRAPH::checkEuler() { // find smallest number of dummy to be the one (n or p) and return the start node
    EDGE* newEdge = nullptr;
    NODE* biggerNode = nullptr;
    int dummyCount = 0;
    vector<NODE*> nodeStack;
    for (auto& node : NodeMap) {
        if (node.second->EdgeList.size() & 1) {
            if (nodeStack.size() < 2) {
                nodeStack.push_back(node.second);
            } else {
                biggerNode = node.second;
                for (int i=0; i<2; i++) {
                    if (node.second->EdgeList.size() < nodeStack[i]->EdgeList.size()) {
                        biggerNode = nodeStack[i];
                        nodeStack[i] = node.second;
                        break;
                    }
                }
                if (newEdge == nullptr) {
                    newEdge = new EDGE("Dummy" + to_string(dummyCount), "Dummy", biggerNode, nullptr);
                } else {
                    newEdge->Drain = biggerNode;
                    biggerNode->EdgeList.push_back(newEdge);
                    newEdge->Source->EdgeList.push_back(newEdge);
                    // cout << newEdge->Name << endl;
                    // cout << newEdge->Source->Name << endl;
                    // cout << newEdge->Drain->Name << endl;
                    EdgeMap.insert(pair<string, EDGE*>("Dummy"+ to_string(dummyCount), newEdge));
                    dummyCount ++;
                    newEdge = nullptr;
                }
            }
        }
    }
    if (nodeStack.empty()) {
        NODE* smallestNode = NodeMap.begin()->second;
        for (auto& node : NodeMap) {
            if (node.second->EdgeList.size() < smallestNode->EdgeList.size()) smallestNode = node.second; 
        }
        return smallestNode;
    }
    if (nodeStack[0]->EdgeList.size() > nodeStack[1]->EdgeList.size()) return nodeStack[1];
    else return nodeStack[0];
}

void GRAPH::otherGraphPathGenerator(GRAPH* otherGraph, int index, unordered_map<string, NET*>& netMap) {
    // for (EDGE* tmpEdge : otherGraph->EulerPath) cout << tmpEdge->Name << endl;
    if (index == EulerPath.size()) {
        // Base case: reached the end of the vector, print the current combination
        // cout << "other euler path: ";
        // for (EDGE* edgeDone : otherGraph->EulerPath) {
        //     cout << edgeDone->Name << "(" << edgeDone->Fliped << ")" << " ";
        // }
        // cout << endl;
        
        if (NewDummyCount < MinNewDummyCount) {
            RECORD path = calHPWL(otherGraph, netMap);
            // cout << path.HPWL << " " << BestPath.HPWL << endl;
            if (path.HPWL < BestPath.HPWL) {
                BestPath = path;
            }
        }
        return;
    }
    // cout << "index = " << index << endl;
    if (EulerPath[index]->Gate.compare("Dummy") == 0) {
        EDGE* edge = new EDGE(EulerPath[index]->Name, "Dummy", nullptr, otherGraph->EulerPath.back()->Source);
        otherGraph->EulerPath.push_back(edge);
        otherGraphPathGenerator(otherGraph, index + 1, netMap);
        otherGraph->EulerPath.pop_back();
    }
    else {
        
        // if (otherGraph->EdgeGateMap[currentEulerPath[index]->Gate].size() > 1) cout << currentEulerPath[index]->Gate << endl;
        for (EDGE* edge : otherGraph->EdgeGateMap[EulerPath[index]->Gate]) {
            // if (currentEulerPath[index]->Gate.compare("SH")==0) cout << index << " " << edge->Visited << endl;
            if (!edge->Visited) {

                if (index == 0) {
                    if ((EulerPath[index]->Source == edge->Drain) || (EulerPath[index]->Drain == edge->Source)) edge->Flip();
                }
                else {
                    if (edge->Drain == otherGraph->EulerPath.back()->Source) ;
                    else if (edge->Source == otherGraph->EulerPath.back()->Source) edge->Flip();
                    else NewDummyCount ++;
                }
                if (!otherGraph->EulerPath.empty()) {
                    if (otherGraph->EulerPath.back()->Gate.compare("Dummy") == 0) otherGraph->EulerPath.back()->Source = edge->Drain;
                }
                otherGraph->EulerPath.push_back(edge);
                edge->Visited = true;
                
                otherGraphPathGenerator(otherGraph, index + 1, netMap);
                // cout << "indexx = " << index << endl;
                if (otherGraph->EulerPath.size()>1) {
                    if (otherGraph->EulerPath.back()->Drain != otherGraph->EulerPath[otherGraph->EulerPath.size()-2]->Source) NewDummyCount --;
                }
                otherGraph->EulerPath.back()->Visited = false;
                otherGraph->EulerPath.pop_back();
            }
            // cout << index << " " << edge->Visited << " " << otherGraph->EulerPath.size() << endl;
        }
    }
}

RECORD GRAPH::calHPWL(GRAPH* othergraph, unordered_map<string, NET*>& netMap) {
    // cout << "cal" << endl;
    RECORD path;
    // add dummy
    EDGE* newEdge;
    int dummyCount = 0;
    float currentX = 25.0/2;

    for (auto& net : netMap) {
        net.second->Left = 0;
        net.second->Right = 0;
        net.second->Y = -1;
    }

    EulerPath[0]->Drain->Net->setLoc(false, currentX, Y);
    othergraph->EulerPath[0]->Drain->Net->setLoc(true, currentX, Y);
    currentX = currentX + 25.0/2 + 20 + 34.0/2;
    // cout << EulerPath[0]->Drain->Name << endl;
    // cout << othergraph->EulerPath[0]->Drain->Name << endl;
    // cout << othergraph->EulerPath[0]->Drain->Left << " " << othergraph->EulerPath[0]->Drain->Right << endl;

    // cout << othergraph->EulerPath.size() << endl;
    // cout << EulerPath.size() << endl;
    for (int i=0; i<othergraph->EulerPath.size()-1; i++) {
        // cout << i << " " << currenX << endl;

        path.ThisMosList.push_back(*EulerPath[i]);
        path.OtherMosList.push_back(*othergraph->EulerPath[i]);

        EulerPath[i]->Source->Net->setLoc(false, currentX, Y);
        othergraph->EulerPath[i]->Source->Net->setLoc(true, currentX, Y);
        currentX = currentX + 34 + 20;
        if (EulerPath[i+1]->Gate.compare("Dummy") == 0) currentX = currentX + 54;
        // cout << EulerPath[i]->Source->Name << " " <<  EulerPath[i+1]->Drain->Name << endl;
        // cout << othergraph->EulerPath[i]->Source->Name << endl;
        // cout << othergraph->EulerPath[i+1]->Name << endl;
        // cout << othergraph->EulerPath[i+1]->Drain->Name << endl;
        
        if (othergraph->EulerPath[i]->Source != othergraph->EulerPath[i+1]->Drain) {
            // cout << "here " << othergraph->EulerPath[i]->Source->Name << " " <<  othergraph->EulerPath[i+1]->Drain->Name << endl;
            // cout << othergraph->EulerPath[i]->Drain->Name << " " <<  othergraph->EulerPath[i+1]->Source->Name << endl;
            newEdge = new EDGE("Dummy", "Dummy", EulerPath[i+1]->Drain, EulerPath[i]->Source);
            path.ThisMosList.push_back(*newEdge);
            newEdge = new EDGE("Dummy", "Dummy", othergraph->EulerPath[i+1]->Drain, othergraph->EulerPath[i]->Source);
            path.OtherMosList.push_back(*newEdge);
            dummyCount ++;
            currentX = currentX + 34 + 20;
            EulerPath[i+1]->Drain->Net->setLoc(false, currentX, Y);
            othergraph->EulerPath[i+1]->Drain->Net->setLoc(true, currentX, Y);
            currentX = currentX + 34 + 20;
        }
    }

    path.ThisMosList.push_back(*EulerPath.back());
    path.OtherMosList.push_back(*othergraph->EulerPath.back());

    currentX = currentX + 25.0/2 - 34.0/2;
    path.ThisMosList.back().Source->Net->setLoc(false, currentX, Y);
    path.OtherMosList.back().Source->Net->setLoc(true, currentX, Y);

    path.HPWL = 0;
    // cout << "----------" << endl;
    // cout << netMap.size() << endl;

    for (auto& net : netMap) { // netlist
        // cout << net.second->Name << " " << net.second->Left << ", " << net.second->Right << endl; 
        path.HPWL += (net.second->Right - net.second->Left);
        if (net.second->Y > 0) {
            path.HPWL += net.second->Y;
        }
    }
    // cout << "HPWL = " << path.HPWL << endl;
    // path.printRecord();
    // // if need store newdummy in edgelist we need to add a tmp vector to store it
    // cout << "???" << endl;
    return path;
}