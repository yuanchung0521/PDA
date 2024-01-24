#include "classtype.h"
using namespace std;

void CONTOUR::InsertBack(CONTOUR* nextContour) {
    if (this->Next != nullptr) {
        this->Next->Prev = nextContour;
        nextContour->Next = this->Next;
    }
    this->Next = nextContour;
    nextContour->Prev = this;
}

void CONTOUR::InsertFront(CONTOUR* prevContour) {
    this->Prev->Next = prevContour;
    prevContour->Prev = this->Prev;
    this->Prev = prevContour;
    prevContour->Next = this;
}

void CONTOUR::Delete() {
    if (this->Next != nullptr) {
        this->Next->Prev = this->Prev;
        this->Prev->Next = this->Next;
    }
    else {
        this->Prev->Next = nullptr;
    }
    this->Block->ContourY = nullptr;
    delete this;
}

void CONTOUR::Clean() {
    CONTOUR* tmpContour = this->Next;
    CONTOUR* nextContour;
    while (tmpContour != nullptr) { 
        nextContour = tmpContour->Next;
        tmpContour->Delete();
        tmpContour = nextContour;
    }
    return;
}