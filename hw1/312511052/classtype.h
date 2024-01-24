#include <vector>
using namespace std;
struct CELL;
struct NET;

struct NET {
    long NetIndex;
    vector<CELL*> CellList;
    long LeftNum;
    long RightNum;
    NET(long index): NetIndex(index), LeftNum(0), RightNum(0) {}
};

struct CELL {
    long CellIndex, gain, G;
    bool Left, Locked;
    vector <NET*> Nets;
    CELL *next, *prev;
    bool Flag;
    CELL(long index, bool side): CellIndex(index), gain(0), G(0), Left(side), Locked(false), Nets(), next(nullptr), prev(nullptr), Flag(false) {}
    
    bool operator>(const CELL *other) {
        return CellIndex > other->CellIndex;
    }

    bool operator<(const CELL *other) {
        return CellIndex < other->CellIndex;
    }
    void ChangeSide () {
        this->Left = !this->Left;
        for (int i=0; i<this->Nets.size(); i++) {
            (this->Left) ? this->Nets[i]->LeftNum++ : this->Nets[i]->LeftNum--;
            (this->Left) ? this->Nets[i]->RightNum-- : this->Nets[i]->RightNum++;
        }
        this->Locked = true;
    }
    void InsertBack (CELL *other) {
        other->prev = this;
        other->next = this->next;
        if (this->next != nullptr) {
            this->next->prev = other;
        }
        this->next = other;
    }
    void RemoveCell () {
        if (this->next != nullptr) {
            this->next->prev = this->prev;
        }
        this->prev->next = this->next;
    }
    void Initialize() {
        if (!this->Flag && this->Locked) {
            this->ChangeSide();
        }
        this->gain = 0;
        this->G = 0;
        this->Locked = false;
        this->next = nullptr;
        this->prev = nullptr;
    }
};