#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <unordered_map>
#include <cstdlib>
#include <ctime> 
#include <iomanip>  
using namespace std;

struct BLOCK;
struct BST;
struct NODE;
struct CONTOUR;
struct ANSWER;

struct BLOCK {
    string Name;
    int Width, Height;
    bool Rotate;
    CONTOUR* ContourY;
    pair<long, long> Pos;
    
    BLOCK (string name, int width, int height) : Pos(pair<int, int>(-1, -1)), Name(name), Width(width), Height(height), Rotate(false) {}
};

struct NODE {
    char Child;
    int Index;

    BLOCK* Block;
    NODE* Parent;
    NODE* Left;
    NODE* Right;

    NODE(char child) : Block(nullptr), Left(nullptr), Right(nullptr), Parent(nullptr), Child(child), Index(0) {}
};

struct CONTOUR {
    long X1, X2, Y;
    CONTOUR* Next;
    CONTOUR* Prev;
    BLOCK* Block;

    CONTOUR(int x1, int x2, int y) : X1(x1), X2(x2), Y(y), Next(nullptr), Prev(nullptr), Block(nullptr) {}
    void InsertBack(CONTOUR* next);
    void InsertFront(CONTOUR* prev);
    void Delete();
    void Clean();
};

struct BST {
    NODE* Root;
    unordered_map<long, NODE*> NodeMap;
    CONTOUR* ContourRoot;
    double MaxWidth, MaxHeight, Area;
    float Ratio, Rl, Ru;

    BST(float rl, float ru) : Root(new NODE('i')), MaxWidth(-1), MaxHeight(-1), Area(0), Ratio(0), Rl(rl), Ru(ru) {
        ContourRoot = new CONTOUR(0, 0, 0);
    }

    void Construct(unordered_map<string, BLOCK*>& blockmap);
    void Preorder(NODE* node);
    void PrintContour();
    void PrintTree(NODE* node);
    void FindWH();
    void Swap(int a, int b);
    void Rotate(int index);
    pair<bool, int> Move(int a, int b);
    void MoveBack(int a, int b, pair<bool, int> moved);
};

struct ANSWER {
    vector<BLOCK> BestResult;
    float BestRatio;
    int BestWidth, BestHeight, BestArea;

    ANSWER(float r, int w, int h, int a) : BestRatio(r), BestWidth(w), BestHeight(h), BestArea(a) {};
};