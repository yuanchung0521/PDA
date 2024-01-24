#include "classtype.h"
#include <cmath> 

float CalCost(BST& bst, float areaNorm) {
    bst.FindWH();
    bool legal = false;
    float penalty = 0;
    if (bst.Ratio >= bst.Rl && bst.Ratio <= bst.Ru) legal = true;
    if (!legal) penalty = 0.5;
    // cout << "area " << bst.Area << " " << areaNorm << endl;
    return (bst.Area) * (1 + penalty);  // / areaNorm
}

pair<bool, float> PerturbAndCheck(BST& bst, float T, float areaNorm) {
    int a, b, op, dir; 
    float costNew, costOld;
    pair<bool, int> moved;
    bool accept = true;
    bst.ContourRoot->Clean();
    bst.Preorder(bst.Root);
    costOld = CalCost(bst, areaNorm);

    op = rand() % 3;
    a = rand() % bst.NodeMap.size();
    b = rand() % bst.NodeMap.size();
    // cout << a << b << endl;
    while (a == b) {
        b = rand() % bst.NodeMap.size();
    }
    // cout << "op = " << op << " " << a << " " << b << endl;
    if (op == 0) bst.Rotate(a);
    else if (op == 1) bst.Swap(a, b);
    else moved = bst.Move(a, b);

    bst.ContourRoot->Clean();
    bst.Preorder(bst.Root);
    costNew = CalCost(bst, areaNorm);
    // cout << "Cost " << costNew << " " << costOld << " P = " << exp(-(costNew-costOld)/T) << endl;
    if (costNew > costOld) {
        if (rand()/(RAND_MAX+1) > exp(-(costNew-costOld)/T)) {
            // cout << "CHANGE BACK!!!!!!!!!!!!!!!!!!" << endl;
            if (op == 0) bst.Rotate(a);
            else if (op == 1) bst.Swap(a, b);
            else bst.MoveBack(a, b, moved);
            accept = false;
        }
        else accept = true;
    }
    else accept = true;
    return pair<bool, float>(accept, costNew - costOld);
}
pair<float, float> GetParams (BST& bst) {
    float P = 0.999999;
    float costSum = 0, areaSum = 0;
    int count = 0;
    int iter = 5 * bst.NodeMap.size();
    float prevCost, newCost;

    // cout << "get T1 " << endl;

    for (int i=0; i<iter; i++) {
        int a, b, op, dir, costNew;
        
        op = rand() % 3;
        a = rand() % bst.NodeMap.size();
        b = rand() % bst.NodeMap.size();

        while (a == b) b = rand() % bst.NodeMap.size();
        // cout << a << " " << b << endl;
        if (op == 0) bst.Rotate(a);
        else if (op == 1) bst.Swap(a, b);
        else {
            // cout << "----------------" << endl;
            // bst.PrintTree(bst.Root);
            // cout << "----------------" << endl;
            pair<bool, int> moved = bst.Move(a, b);
            // if (count/3==0) {bst.MoveBack(a, b, moved); cout << "moved" << endl;}
            // cout << "----------------" << endl;
            // bst.PrintTree(bst.Root);
            // cout << "----------------" << endl;

        }


        bst.ContourRoot->Clean();
        bst.Preorder(bst.Root);
        // cout << "----------------" << endl;
        // bst.PrintTree(bst.Root);
        // cout << "----------------" << endl;
        // bst.PrintContour();
        newCost = CalCost(bst, 1); // random num
        if (newCost > prevCost) {
            costSum += (newCost - prevCost);
            areaSum += bst.Area;
            count ++;
        }

        prevCost = newCost;
    }
    return pair<float, float>(-(costSum / count) / log10(P), areaSum / count);
}

void FastSA (BST& bst, unordered_map<string, BLOCK*>& BlockMap, ofstream& OutputFile) {
    int k = 10, c = 10000, n = 0, count;
    float T, T1, deltaCostAvg = 0;
    float bestArea = 0;;
    float bestRatio = 0;
    vector<BLOCK> bestResult;
    pair<float, float> params;
    pair<bool, float> uphill;
    srand((unsigned) time(NULL));
    params = GetParams(bst);
    bestArea = bst.Area*100;
    if ((bst.Area < bestArea) && (bst.Ratio >= bst.Rl && bst.Ratio <= bst.Ru)) {
        bestResult.clear();
        bestArea = bst.Area;
        bestRatio = bst.Ratio;
        for (auto& block : BlockMap) {
            bestResult.push_back(*(block.second));
        }
    }
    T1 = params.first/params.second*100;
    T = T1;
    cout << "T1: " << T1 << endl;
    while (n <= 2) { 
        uphill = PerturbAndCheck(bst, T, params.second);
        
        if (uphill.first) {
            if (count != 0) deltaCostAvg = deltaCostAvg/count;
            if (deltaCostAvg == 0) deltaCostAvg = 1;
            if (n == 0) T = T1;
            else if (n <= k) T = (deltaCostAvg * T1) / (n * c);
            else T = (deltaCostAvg * T1) / n;
            n++;

            cout << n << " " <<  T << " t1:"  << T1  << " " << deltaCostAvg << " bst:" << bst.Area << " " << bst.Ratio << " wl" << bst.MaxWidth << " " << bst.MaxHeight << " a:" << bestArea << " r:" << bst.Ru << " " << bst.Rl << endl;
            if ((bst.Area < bestArea) && (bst.Ratio >= bst.Rl && bst.Ratio <= bst.Ru)) {
                bestResult.clear();
                bestArea = bst.Area;
                bestRatio = bst.Ratio;
                for (auto& block : BlockMap) {
                    bestResult.push_back(*(block.second));
                }
            }
            deltaCostAvg = 0;
            count = 0;
        }
        else {
            deltaCostAvg += abs(uphill.second);
            count ++;
        }
    }
    // bst.ContourRoot->Clean();
    // bst.Preorder(bst.Root);
    cout << "========= Result =========" << endl;
    // cout << CalCost(bst, params.second) << endl;
    cout << bestArea << endl;
    cout << bestRatio << endl;
    OutputFile << "A = " << bestArea << endl;
    OutputFile << "R = " << bestRatio << endl;
    // bst.PrintContour();
    for (BLOCK block : bestResult) {
        cout << block.Name << " (" << block.Pos.first << ", " << block.Pos.second << ") " << block.Rotate << " (" << block.Width << ", " << block.Height << ")" << endl;
        OutputFile << block.Name << " " << block.Pos.first << " " << block.Pos.second << ((block.Rotate) ? " R" : "")<< endl;
    }
}

ANSWER SA (BST& bst, unordered_map<string, BLOCK*>& BlockMap, ofstream& OutputFile) {
    float T = 100;
    int stop = 10000;
    int bestCount = 0;
    int bestWidth, bestHeight;
    srand((unsigned) time(NULL));
    float bestArea = INT_FAST32_MAX;
    float bestRatio = 0;
    vector<BLOCK> bestResult;
    pair<float, float> params;
    pair<bool, float> accept;
    // params = GetParams(bst);
    int iter = 0;
    while (iter != stop) {
        accept = PerturbAndCheck(bst, T, 0);
        // cout << "====================" << endl;
        // bst.PrintContour();
        // bst.PrintTree(bst.Root);
        // for (auto& block : BlockMap) {
        //     cout << block.second->Name << " (" << block.second->Pos.first << ", " << block.second->Pos.second << ") " << block.second->Rotate << endl;
        // }
        // cout << "====================" << endl;
        if (accept.first) {
            // cout << "accept" << endl;
            T = T*0.9;
            if ((bst.Area < bestArea) && (bst.Ratio >= bst.Rl && bst.Ratio <= bst.Ru)) {
                bestResult.clear();
                bestArea = bst.Area;
                bestRatio = bst.Ratio;
                bestWidth = bst.MaxWidth;
                bestHeight = bst.MaxHeight;
                for (auto& block : BlockMap) {
                    bestResult.push_back(*(block.second));
                }
                if (bst.NodeMap.size()<5) break;
                if (iter != 0 && iter < 10000) stop = iter*5;
                else {
                    stop = iter + 10;
                }
                if (bestCount > 20) {
                    break;
                }
                break;
                // cout << "find " << stop << endl;
            }
            // cout << T << " bst:" << bst.Area << " " << bst.Ratio << " a:" << bestArea << " r:" << bst.Ru << " " << bst.Rl << endl;
        }
        // cout << iter << " " << stop << endl;
        iter++;
        // if (iter%100 == 0) cout << iter << endl;
    }
    // cout << "========= Result =========" << endl;
    // // cout << CalCost(bst, params.second) << endl;
    // cout << bestArea << endl;
    // cout << bestRatio << endl;
    // cout << bestWidth << endl;
    // cout << bestHeight << endl;
    
    // bst.PrintTree(bst.Root);
    ANSWER bestAnswer(bestRatio, bestWidth, bestHeight, bestArea);
    // OutputFile << fixed << "A = " << bestArea << endl;
    // OutputFile << "R = " << bestRatio << endl;

    bestAnswer.BestResult = bestResult;

    // for (BLOCK block : bestResult) {
    //     cout << block.Name << " (" << block.Pos.first << ", " << block.Pos.second << ") " << block.Rotate << " (" << block.Width << ", " << block.Height << ")" << endl;
    //     // OutputFile << block.Name << " " << block.Pos.first << " " << block.Pos.second << ((block.Rotate) ? " R" : "")<< endl;
    // }

    return bestAnswer;
}