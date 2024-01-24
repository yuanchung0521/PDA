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

vector<NET*> NetList;

void PrintLinkedList (vector<CELL*> &linkedlist) {
    for (int i=0; i<linkedlist.size(); i++) {
        if (1) { //linkedlist[i]->next!= nullptr
            cout << i << ": ";
            CELL *tmpptr = linkedlist[i];
            while (tmpptr != nullptr) {
                cout << tmpptr->CellIndex << " -> ";
                tmpptr = tmpptr->next;
            }
            cout << endl;
        }
    }
}
void PrintNetList () {
    for (int i=0;i<NetList.size();i++) {
        cout << "NET" << NetList[i]->NetIndex << " {l=" << NetList[i]->LeftNum << " r=" << NetList[i]->RightNum << "} : ";
        cout << "Left[ ";
        for (int j=0;j<NetList[i]->CellList.size();j++) {
            if (NetList[i]->CellList[j]->Left) cout << "C" << NetList[i]->CellList[j]->CellIndex << "(" << NetList[i]->CellList[j]->gain << ") ";
        }
        cout << "]  Right[ ";
        for (int j=0;j<NetList[i]->CellList.size();j++) {
            if (!NetList[i]->CellList[j]->Left) cout << "C" << NetList[i]->CellList[j]->CellIndex << "(" << NetList[i]->CellList[j]->gain << ") ";
        }
        cout << "]" << endl;
    }
}

void PrintCellList (map<long, CELL*> &celllist) {
    for (const auto& pair : celllist) {
        cout << "C" << pair.first << ": "<< pair.second->CellIndex << ", ";
        (pair.second->Left) ? cout << "left, " : cout << "right, ";
        (pair.second->Locked) ? cout << "locked, " : cout << "not locked, ";
        cout << pair.second->gain << ", G=" << pair.second->G << " || Net: ";
        for (int j=0; j<pair.second->Nets.size();j++) cout << pair.second->Nets[j]->NetIndex << " ";
        cout << endl;
    }
}
void CalGainNet (NET *tmpnet) {
    if (tmpnet->LeftNum == 1) {
        for (int i=0;i<tmpnet->CellList.size();i++) {
            if (tmpnet->CellList[i]->Left) {
                tmpnet->CellList[i]->gain++;
                break;
            }
        }
    }
    else if (tmpnet->LeftNum == 0) {
        for (int i=0;i<tmpnet->CellList.size();i++) {
            if (!tmpnet->CellList[i]->Left) tmpnet->CellList[i]->gain--;
        }
    }
    if (tmpnet->RightNum == 1) {
        for (int i=0;i<tmpnet->CellList.size();i++) {
            if (!tmpnet->CellList[i]->Left) {
                tmpnet->CellList[i]->gain++;
                break;
            }
        }
    }
    else if (tmpnet->RightNum == 0) {
        for (int i=0;i<tmpnet->CellList.size();i++) {
            if (tmpnet->CellList[i]->Left) tmpnet->CellList[i]->gain--;
        }
    }
}
void UpdateGain (CELL *cell, vector<CELL*> &leftlist, vector<CELL*> &rightlist, long Pmax) {
    long ToNum, FromNum;
    for (int i=0;i<cell->Nets.size();i++) {
        ToNum = (!cell->Left) ? cell->Nets[i]->RightNum : cell->Nets[i]->LeftNum;
        FromNum = (!cell->Left) ? cell->Nets[i]->LeftNum : cell->Nets[i]->RightNum;
        // cout << "N" << i << " f=" << FromNum << " t=" << ToNum << ": ";
        if (ToNum == 1) {
            // cout << "case 0";
            for(int j=0; j<cell->Nets[i]->CellList.size(); j++) {
                if (!cell->Nets[i]->CellList[j]->Locked) {
                    cell->Nets[i]->CellList[j]->gain++;
                    cell->Nets[i]->CellList[j]->RemoveCell();
                    if (cell->Nets[i]->CellList[j]->Left) {
                        leftlist[cell->Nets[i]->CellList[j]->gain+Pmax]->InsertBack(cell->Nets[i]->CellList[j]);
                    }
                    else {
                        rightlist[cell->Nets[i]->CellList[j]->gain+Pmax]->InsertBack(cell->Nets[i]->CellList[j]);
                    }
                }
            }
        }
        else if (ToNum == 2) {
            // cout << "case1 ";
            for (int j=0; j<cell->Nets[i]->CellList.size(); j++) {
                if (!cell->Nets[i]->CellList[j]->Locked) {
                    if (!cell->Left && cell->Nets[i]->CellList[j]->Left==false) {
                        cell->Nets[i]->CellList[j]->gain--;
                        cell->Nets[i]->CellList[j]->RemoveCell();
                        if (cell->Nets[i]->CellList[j]->Left) {
                            leftlist[cell->Nets[i]->CellList[j]->gain+Pmax]->InsertBack(cell->Nets[i]->CellList[j]);
                        }
                        else {
                            rightlist[cell->Nets[i]->CellList[j]->gain+Pmax]->InsertBack(cell->Nets[i]->CellList[j]);
                        }
                        break;
                    }
                    else if (cell->Left && cell->Nets[i]->CellList[j]->Left==true) {
                        cell->Nets[i]->CellList[j]->gain--;
                        cell->Nets[i]->CellList[j]->RemoveCell();
                        if (cell->Nets[i]->CellList[j]->Left) {
                            leftlist[cell->Nets[i]->CellList[j]->gain+Pmax]->InsertBack(cell->Nets[i]->CellList[j]);
                        }
                        else {
                            rightlist[cell->Nets[i]->CellList[j]->gain+Pmax]->InsertBack(cell->Nets[i]->CellList[j]);
                        }
                        break;
                    }
                } 
            }    
        }
        if (FromNum == 0) {
            // cout << "case2 ";
            for (int j=0; j<cell->Nets[i]->CellList.size(); j++) {
                if (!cell->Nets[i]->CellList[j]->Locked) {
                    cell->Nets[i]->CellList[j]->gain--;
                    cell->Nets[i]->CellList[j]->RemoveCell();
                    if (cell->Nets[i]->CellList[j]->Left) {
                        leftlist[cell->Nets[i]->CellList[j]->gain+Pmax]->InsertBack(cell->Nets[i]->CellList[j]);
                    }
                    else {
                        rightlist[cell->Nets[i]->CellList[j]->gain+Pmax]->InsertBack(cell->Nets[i]->CellList[j]);
                    }
                }
            }
        }
        else if (FromNum == 1) {
            // cout << "case3 ";
            for (int j=0; j<cell->Nets[i]->CellList.size(); j++) {
                if (!cell->Nets[i]->CellList[j]->Locked) {
                    if (!cell->Left && cell->Nets[i]->CellList[j]->Left==true) {
                        cell->Nets[i]->CellList[j]->gain++;
                        cell->Nets[i]->CellList[j]->RemoveCell();
                        if (cell->Nets[i]->CellList[j]->Left) {
                            leftlist[cell->Nets[i]->CellList[j]->gain+Pmax]->InsertBack(cell->Nets[i]->CellList[j]);
                        }
                        else {
                            rightlist[cell->Nets[i]->CellList[j]->gain+Pmax]->InsertBack(cell->Nets[i]->CellList[j]);
                        }
                        break;
                    }
                    else if (cell->Left && cell->Nets[i]->CellList[j]->Left==false) {
                        cell->Nets[i]->CellList[j]->gain++;
                        cell->Nets[i]->CellList[j]->RemoveCell();
                        if (cell->Nets[i]->CellList[j]->Left) {
                            leftlist[cell->Nets[i]->CellList[j]->gain+Pmax]->InsertBack(cell->Nets[i]->CellList[j]);
                        }
                        else {
                            rightlist[cell->Nets[i]->CellList[j]->gain+Pmax]->InsertBack(cell->Nets[i]->CellList[j]);
                        }
                        break;
                    }
                } 
            }   
        }
    }
    // cout << "C" << cell->CellIndex << ": " << cell->gain << endl;
    cell->RemoveCell();
    // (cell->Left) ? leftlist[cell->gain+Pmax]->InsertBack(cell) : rightlist[cell->gain+Pmax]->InsertBack(cell);
}

int main (int argc, char *argv[]) {
    // cout << "This is FM partition method" << endl;
    ifstream InputFile(argv[1]);
    ofstream OutputFile(argv[2]);
    string s;
    float BalanceFactor; 
    map<long, CELL*> CellList;
    NET* tmpnet = nullptr;
    CELL *tmpcell = nullptr;
    bool left;
    long Pmax = 0; 
    long index;
    float LeftSum = 0;
    float RightSum = 0;

    srand(static_cast<unsigned>(time(nullptr)));
    // read inputfile
    InputFile >> s;
    BalanceFactor = stof(s);
    while (InputFile>>s) {
        if (s == "NET") {
            InputFile >> s; // find net name ex: n1, and substr "n"
            tmpnet = new NET(stoi(s.substr(1)));
        }
        else if (s == ";") { 
            sort(tmpnet->CellList.begin(), tmpnet->CellList.end());
            tmpnet->CellList.erase(unique(tmpnet->CellList.begin(), tmpnet->CellList.end()), tmpnet->CellList.end());
            CalGainNet(tmpnet);
            NetList.push_back(tmpnet);
        }
        else {
            // initial random for cell
            index = stoi(s.substr(1));
            
            auto it = CellList.find(index);
            if (it == CellList.end()) {
                // left = (rand()%2 == 1);
                left = 1; // initial make in one side;
                tmpcell = new CELL(index, left);
                CellList.insert(pair<long, CELL*>(index, tmpcell));
            }
            else {
                tmpcell = CellList[index];
            }
            tmpcell->Nets.push_back(tmpnet); // I should erase the duplicates
            sort(tmpcell->Nets.begin(), tmpcell->Nets.end());
            // if (unique(tmpcell->Nets.begin(), tmpcell->Nets.end()) != tmpcell->Nets.end()) {
            //     cout << tmpcell->CellIndex << endl;
            //     cout << (*unique(tmpcell->Nets.begin(), tmpcell->Nets.end()))->NetIndex << endl;
            //     }
            tmpcell->Nets.erase(unique(tmpcell->Nets.begin(), tmpcell->Nets.end()), tmpcell->Nets.end());
            
            tmpnet->CellList.push_back(tmpcell);
            (tmpcell->Left) ? tmpnet->LeftNum++ : tmpnet->RightNum++;            
        }
    }
    // finish read process
    // Calculate Pmax
    for (const auto& pair : CellList) {
        if (pair.second->Nets.size() > Pmax) {
            Pmax = pair.second->Nets.size();
        }
    }
    // cout << "Pmax= " << Pmax << endl;
    // cout << "CellNum= " << CellList.size() << endl;
    // cout << "NetNum= " << NetList.size() << endl; 

    // make a CellList sort by gain
    vector<CELL*> CellListLeft(Pmax*2+1);
    vector<CELL*> CellListRight(Pmax*2+1);
    
    // initialize the two linked list
    for (int i=0;i<(Pmax*2+1);i++) {
        CellListLeft[i] = new CELL(0, true);
        CellListRight[i] = new CELL(0, false);
    }
    // construct the first linked list, we store the cell in the back
    for (const auto& pair : CellList) {
        if (pair.second->Left) {
            LeftSum++;
            CellListLeft[pair.second->gain+Pmax]->InsertBack(pair.second);
        }  
        else {
            RightSum++;
            CellListRight[pair.second->gain+Pmax]->InsertBack(pair.second);
        }
    }
    // cout << "----------first-----------" << endl;
    // PrintCellList(CellList);
    // PrintNetList();

    int count, maxIndex;
    vector<int> G(CellList.size());
    vector<CELL*> CellListPast;
    int iter = 0;
    int pos, MaxGain;
    bool stop = false, balanced = false;
    CELL *othercell, *leftcell, *rightcell;
    vector<vector<bool>> tmppos(CellList.size(), vector<bool>(CellList.size()));
    auto maxG = max_element(G.begin(), G.end());

    while (!stop) {
        // cout << "====================================== iter" << iter << " ======================================" << endl;
        // cout << "----------first-----------" << endl;
        // PrintCellList(CellList);
        // PrintNetList();
        for (int i=0; i<G.size(); i++) G[i] = 0;
        for (int count=0; count<CellList.size(); count++) {
            othercell = nullptr;
            tmpcell = nullptr;
            // cout << "L=" << LeftSum << " R=" << RightSum << endl;
            if ((LeftSum)/(LeftSum+RightSum) > (1+BalanceFactor)/2) {
                while ((LeftSum)/(LeftSum+RightSum) > (1+BalanceFactor)/2) {
                    // cout << LeftSum << " " << RightSum << " " << (LeftSum)/(LeftSum+RightSum) << " " << (1+BalanceFactor)/2 << endl;
                    // cout << "Left is too much" << endl;
                    for (int i=CellListLeft.size()-1; i>=0; i--) {
                        if (CellListRight[i]->next != nullptr) {
                            othercell = CellListRight[i]->next;
                        }
                        if (CellListLeft[i]->next != nullptr) {
                            tmpcell = CellListLeft[i]->next;
                            break;
                        }
                    }
                    if (tmpcell == nullptr) {
                        cout << "no left cell" << endl;
                        cout << "something wrong" << endl;
                        return 1;
                    }
                    else {
                        if (othercell != nullptr){
                            if (LeftSum/(RightSum+LeftSum) <= (1+BalanceFactor)/2 
                                && LeftSum/(RightSum+LeftSum) >= (1-BalanceFactor)/2 
                                && RightSum/(RightSum+LeftSum) <= (1+BalanceFactor)/2
                                && RightSum/(RightSum+LeftSum) >= (1-BalanceFactor)/2
                                && othercell->gain<=0
                                && tmpcell->gain<=0) {
                                    // break;
                            }
                        }
                        // cout << "changeside" << endl;
                        tmpcell->ChangeSide();
                        tmpcell->Flag = true;
                        LeftSum--;
                        RightSum++;

                        // cout << tmpcell->CellIndex << endl;
                        // cout << "Update Gain" << endl;

                        UpdateGain(tmpcell, CellListLeft, CellListRight, Pmax);

                        if (count == 0) {
                            G[count] = tmpcell->gain;
                        }
                        else {
                            G[count] = G[count-1] + tmpcell->gain ;
                        }
                    }
                }
                break;
            }
            else if ((RightSum)/(LeftSum+RightSum) > (1+BalanceFactor)/2) {
                // cout << "Right is too much" << endl;
                for (int i=CellListRight.size()-1; i>=0; i--) {
                    if (CellListLeft[i]->next != nullptr) {
                        othercell = CellListLeft[i]->next;
                    }
                    if (CellListRight[i]->next != nullptr) {
                        tmpcell = CellListRight[i]->next;
                        break;
                    }
                }
                if (tmpcell == nullptr) {
                    cout << "no right cell" << endl;
                    break;
                }
                else {
                    if (othercell != nullptr){
                        if (LeftSum/(RightSum+LeftSum) <= (1+BalanceFactor)/2 
                            && LeftSum/(RightSum+LeftSum) >= (1-BalanceFactor)/2 
                            && RightSum/(RightSum+LeftSum) <= (1+BalanceFactor)/2
                            && RightSum/(RightSum+LeftSum) >= (1-BalanceFactor)/2
                            && othercell->gain<=0
                            && tmpcell->gain<=0) {
                                // break;
                        }
                    }
                    tmpcell->ChangeSide();
                    RightSum--;
                    LeftSum++;
                    // cout << "C" << tmpcell->CellIndex << " ";
                    // cout << "Update Gain" << endl;
                    UpdateGain(tmpcell, CellListLeft, CellListRight, Pmax);
                    if (count == 0) {
                        G[count] = tmpcell->gain;
                    }
                    else {
                        G[count] = G[count-1] + tmpcell->gain ;
                    }
                    pos = 0;
                    // for (auto &pair : CellList) {
                    //     tmppos[count][pos] = pair.second->Left;
                    //     pos++;
                    // }
                }
            }
            else {
                // cout << "balanced " << LeftSum << " " << RightSum << endl;
                balanced = true;
                for (int i=CellListLeft.size()-1; i>=0; i--) {
                    if (CellListLeft[i]->next != nullptr) {
                        leftcell = CellListLeft[i]->next;
                    }
                    if (CellListRight[i]->next != nullptr) {
                        rightcell = CellListRight[i]->next;
                    }
                    if (CellListLeft[i]->next != nullptr && CellListRight[i]->next != nullptr) {
                        
                        if (((RightSum+1)/(LeftSum+RightSum) > (1+BalanceFactor)/2) || ((LeftSum-1)/(LeftSum+RightSum) < (1-BalanceFactor)/2)) {
                            // reach the left limit
                            tmpcell = CellListRight[i]->next;
                            break;
                        }
                        else if (((LeftSum+1)/(LeftSum+RightSum) > (1+BalanceFactor)/2) || ((RightSum-1)/(LeftSum+RightSum) < (1-BalanceFactor)/2)) {
                            // reach the right limit
                            tmpcell = CellListLeft[i]->next;
                            break;
                        }
                        else {
                            tmpcell = (CellListRight[i]->next->CellIndex > CellListLeft[i]->next->CellIndex) ? CellListLeft[i]->next : CellListRight[i]->next;
                            break;
                        }
                    }
                    else if (CellListLeft[i]->next != nullptr && ((RightSum+1)/(LeftSum+RightSum) <= (1+BalanceFactor)/2) && ((LeftSum-1)/(LeftSum+RightSum) >= (1-BalanceFactor)/2)) {
                        tmpcell = CellListLeft[i]->next;
                        break;
                    }
                    else if (CellListRight[i]->next != nullptr && ((LeftSum+1)/(LeftSum+RightSum) <= (1+BalanceFactor)/2) && ((RightSum-1)/(LeftSum+RightSum) >= (1-BalanceFactor)/2)) {
                        tmpcell = CellListRight[i]->next;
                        break;
                    }
                    else {
                        tmpcell = nullptr;
                        // cout << "no available cell" << endl;
                        // cout << "L=" << LeftSum << " R=" << RightSum << endl;
                        // cout << "balance: (1+BalanceFactor)/2=" << (1+BalanceFactor)/2 << " (1-BalanceFactor)/2=" << (1-BalanceFactor)/2 << endl;
                    }
                }
                if (tmpcell == nullptr) {
                    cout << "no available cell" << endl;
                    // cout << "L=" << LeftSum << " R=" << RightSum << endl;
                    // cout << "balance: (1+BalanceFactor)/2=" << (1+BalanceFactor)/2 << " (1-BalanceFactor)/2=" << (1-BalanceFactor)/2 << endl;
                    break;
                }
                else {
                    // cout << "C" << tmpcell->CellIndex << " ";
                    // cout << leftcell->gain << " " << rightcell->gain << endl;
                    if (leftcell!=nullptr && rightcell!=nullptr) {
                        if (LeftSum/(RightSum+LeftSum) <= (1+BalanceFactor)/2 
                            && LeftSum/(RightSum+LeftSum) >= (1-BalanceFactor)/2 
                            && RightSum/(RightSum+LeftSum) <= (1+BalanceFactor)/2
                            && RightSum/(RightSum+LeftSum) >= (1-BalanceFactor)/2
                            && leftcell->gain<=0
                            && rightcell->gain<=0) {
                                // break;
                        }
                    }
                    
                    tmpcell->ChangeSide();
                    (tmpcell->Left) ? LeftSum++ : LeftSum--;
                    (tmpcell->Left) ? RightSum-- : RightSum ++;
                    
                    // cout << "Update Gain" << endl;
                    // cout << count << endl;
                    // cout << tmpcell->CellIndex << " " << tmpcell->Nets.size() << " " << tmpcell->prev->CellIndex;
                    // PrintCellList(CellList);
                    // PrintNetList();
                    UpdateGain(tmpcell, CellListLeft, CellListRight, Pmax);
                    if (count == 0) {
                        G[count] = tmpcell->gain;
                    }
                    else {
                        G[count] = G[count-1] + tmpcell->gain ;
                    }
                    if (count == 150750) {
                        cout << tmpcell->CellIndex << " " << tmpcell->gain << endl;
                    }
                    tmpcell->G = G[count];
                    pos = 0;
                    // for (auto &pair : CellList) {
                    //     tmppos[count][pos] = pair.second->Left;
                    //     pos++;
                    // }
                }
            }
            CellListPast.push_back(tmpcell);
            // cout << "--------------------------" << endl;
            // cout << "C" << tmpcell->CellIndex << endl;
            // PrintCellList(CellList);
            // PrintNetList();
            // cout << "--------------------------" << endl;
            // cout << "LEFT" << endl;
            // PrintLinkedList(CellListLeft);
            // cout << "RIGHT" << endl;
            // PrintLinkedList(CellListRight);
            // // cout << "=====================================" << count << "========================================" << endl;
        }
        iter ++;

        // PrintCellList(CellList);
        // PrintNetList();

        // cout << endl;
        // unlock the cell
        pos = 0;
        // for (int i=0;i<G.size(); i++) {
        //     cout << G[i] << " ";
        // }
        // cout << endl;
        auto maxG = max_element(G.begin(), G.end());
        stop = (*maxG<=0 && balanced) ? true : false;
        // cout << balanced << endl;
        if (stop) break;
        maxIndex = distance(G.begin(), maxG);

        // cout << "L=" << LeftSum << " R=" << RightSum << endl;
        // cout << "G=" << *maxG << " Index=" << maxIndex << endl;
        
        for (int i=0; i<CellListPast.size(); i++) {
            CellListPast[i]->Flag = (i <= maxIndex) ? true : false;
        }
        while (!CellListPast.empty()) {
            CellListPast.pop_back();
        }

        for (auto &pair : CellList) {
            pair.second->Initialize();
        }
        for (int i=0; i<NetList.size(); i++) {
            NetList[i]->LeftNum = 0;
            NetList[i]->RightNum = 0;
            for (int j=0; j<NetList[i]->CellList.size(); j++) {
                (NetList[i]->CellList[j]->Left) ? NetList[i]->LeftNum++ : NetList[i]->RightNum++;
            }
            CalGainNet(NetList[i]);
        }
        for (int i=0;i<(Pmax*2+1);i++) {
            CellListLeft[i]->next = nullptr;
            CellListRight[i]->next = nullptr;
        }
        LeftSum = 0;
        RightSum = 0;
        for (const auto& pair : CellList) {
            if (pair.second->Left) {
                LeftSum++;
                CellListLeft[pair.second->gain+Pmax]->InsertBack(pair.second);
            }  
            else {
                RightSum++;
                CellListRight[pair.second->gain+Pmax]->InsertBack(pair.second);
            }
        }
        // PrintCellList(CellList);
        // PrintNetList();
        // PrintLinkedList(CellListLeft);
        // PrintLinkedList(CellListRight);
        
        // cout << (*maxG>0) << endl;
        
    }
   
    vector<CELL*> G1;
    vector<CELL*> G2;
    long CutNum = 0;
    for (auto &pair : CellList) {
        (pair.second->Left) ? G1.push_back(pair.second) : G2.push_back(pair.second);
    }
    for (int i=0;i<NetList.size();i++) {
        if (NetList[i]->LeftNum!=0 && NetList[i]->RightNum!=0) CutNum++;
    }
    // cout << "CutNum= " << CutNum << endl;
    OutputFile << "Cutsize = " << CutNum << endl;
    OutputFile << "G1 " << G1.size() << endl;
    for (int i=0; i<G1.size(); i++) {
        OutputFile << "c" << G1[i]->CellIndex << " ";
    } 
    OutputFile << ";" << endl;
    OutputFile << "G2 " << G2.size() << endl;
    for (int i=0; i<G2.size(); i++) {
        OutputFile << "c" << G2[i]->CellIndex << " ";
    } 
    OutputFile << ";" << endl;
    // cout << "Count= " << count << endl;
    // PrintCellList(CellList);
    // PrintNetList();
    return 0;
}
