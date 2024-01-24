#include "classtype.h"
using namespace std;

void BST::Construct(unordered_map<string, BLOCK*>& blockmap) {
    queue<NODE*> leaf;
    NODE* node;
    int nodeIndex = 0;
    leaf.push(Root);
    for (auto& block : blockmap) {
        node = leaf.front();
        leaf.pop();

        node->Block = block.second;
        NodeMap.insert(pair<int, NODE*>(nodeIndex, node));
        node->Index = nodeIndex;
        // cout << nodeIndex << " " << block.second->Name << " " << block.second->Width << " " << block.second->Height << endl;

        node->Left = new NODE('l');
        node->Left->Parent = node;
        leaf.push(node->Left);

        node->Right = new NODE('r');
        node->Right->Parent = node;
        leaf.push(node->Right);

        nodeIndex++;
    }
    while (!leaf.empty()) {
        node = leaf.front();
        if (node->Child =='r') {
            node->Parent->Right = nullptr;
            delete node;
        }
        else {
            node->Parent->Left = nullptr;
            delete node;
        }
        leaf.pop();
    }
    return;
}

void BST::Preorder(NODE *current){
    BLOCK* parentBlock;
    CONTOUR* tmpContour;
    int prevX, maxHeight;
    if (current != nullptr) {
        // cout << current->Block->Name << " ";   // V
        if (current->Child == 'i') {
            current->Block->ContourY = new CONTOUR(0, current->Block->Width, current->Block->Height);
            ContourRoot->Next = current->Block->ContourY;
            current->Block->ContourY->Prev = ContourRoot;
            current->Block->ContourY->Block = current->Block;
            current->Block->Pos = pair<int, int>(0, 0);
        }
        else if (current->Child == 'l') {
            parentBlock = current->Parent->Block;
            prevX = parentBlock->Pos.first + parentBlock->Width;
            if (parentBlock->ContourY->Next == nullptr) {
                maxHeight = 0;
                current->Block->ContourY = new CONTOUR(prevX, prevX + current->Block->Width, current->Block->Height);
            }
            else {
                tmpContour = parentBlock->ContourY->Next;
                maxHeight = tmpContour->Y;
                while (tmpContour != nullptr) {
                    if (tmpContour->Y > maxHeight) maxHeight = tmpContour->Y;
                    if (tmpContour->X2 <= prevX + current->Block->Width) {
                        tmpContour->Delete();
                        tmpContour = tmpContour->Next;
                    }
                    else {
                        break;
                    }
                }
                current->Block->ContourY = new CONTOUR(prevX, prevX + current->Block->Width, maxHeight + current->Block->Height);
                if (tmpContour != nullptr) tmpContour->X1 = prevX + current->Block->Width;
            }
            parentBlock->ContourY->InsertBack(current->Block->ContourY);         // insert Contour
            current->Block->ContourY->Block = current->Block;                    // Contour->block
            current->Block->Pos = pair<int, int>(prevX, maxHeight);
        }
        else { // current->Child == 'r'
            parentBlock = current->Parent->Block;
            prevX = parentBlock->Pos.first;
            tmpContour = parentBlock->ContourY;
            maxHeight = parentBlock->ContourY->Y;

            current->Block->ContourY = new CONTOUR(prevX, prevX + current->Block->Width, maxHeight + current->Block->Height); // y is wrong here
            parentBlock->ContourY->InsertFront(current->Block->ContourY);   // insert Contour
            
            while (tmpContour != nullptr) {
                if (tmpContour->Y > maxHeight) maxHeight = tmpContour->Y;
                if (tmpContour->X2 <= prevX + current->Block->Width) {
                    tmpContour->Delete();
                    tmpContour = tmpContour->Next;
                }
                else {
                    break;
                }
            }
            if (tmpContour != nullptr) tmpContour->X1 = prevX + current->Block->Width;
            
            current->Block->ContourY->Y = maxHeight + current->Block->Height;
            current->Block->ContourY->Block = current->Block;                    // Contour->block
            current->Block->Pos = pair<int, int>(prevX, maxHeight);
        }
        Preorder(current->Left);       // L
        Preorder(current->Right);      // R
    }
    return;
}

void BST::PrintContour() {
    cout << "-----" << endl;
    CONTOUR* tmpContour = ContourRoot->Next;
    while (tmpContour != nullptr) {
        cout << "(" << tmpContour->X1 << ", " << tmpContour->X2 << ") " << tmpContour->Y << endl;
        tmpContour = tmpContour->Next;
    }
    cout << "-----" << endl;
    return;
}

void BST::PrintTree(NODE *current) {
    if (current != nullptr) {
        cout << current->Index << " " << current->Block->Name << " " << current->Child << "p: ";
        if (current->Child != 'i') cout << current->Parent->Index << endl; else cout << endl;
        PrintTree(current->Left);
        PrintTree(current->Right);
    }
    return;
}

void BST::FindWH() {
    CONTOUR* tmpContour = ContourRoot->Next;
    MaxHeight = ContourRoot->Next->Y;
    MaxWidth = ContourRoot->Next->X2;
    while (tmpContour->Next != nullptr) {
        if (tmpContour->Y > MaxHeight) MaxHeight = tmpContour->Y;
        tmpContour = tmpContour->Next;
    }
    if (tmpContour->Y > MaxHeight) MaxHeight = tmpContour->Y;
    MaxWidth = tmpContour->X2;
    // cout << "W, H = " << MaxWidth << ", " << MaxHeight << endl;
    Area = MaxWidth * MaxHeight;
    Ratio = MaxWidth / MaxHeight;
    // cout << "W, H = " << MaxWidth << ", " << MaxHeight << ", A = " << Area << endl;
    return;
}

void BST::Swap(int a, int b) {
    BLOCK* tmpBlock;
    tmpBlock = NodeMap[a]->Block;
    NodeMap[a]->Block = NodeMap[b]->Block;
    NodeMap[b]->Block = tmpBlock;
    return;
}

void BST::Rotate(int index) {
    NodeMap[index]->Block->Rotate = !NodeMap[index]->Block->Rotate;
    int tmp;
    tmp = NodeMap[index]->Block->Width;
    NodeMap[index]->Block->Width = NodeMap[index]->Block->Height;
    NodeMap[index]->Block->Height = tmp;
}

pair<bool, int> BST::Move(int a, int b) {
    int leaf = -1;
    NODE* source = NodeMap[a];
    bool nodeup = false;
    bool dir;
    if (source->Child == 'l') dir = true;
    else if (source->Child == 'r') dir = false;
    else dir = (source->Left != nullptr) ? true : false;

    NODE* dest = NodeMap[b];
    NODE* tmpNode = (dir) ? source->Left : source->Right;
    
    if (tmpNode == nullptr) leaf = source->Parent->Index;
    while (tmpNode != nullptr) {
        Swap(source->Index, tmpNode->Index);
        source = (dir) ? source->Left : source->Right;
        tmpNode = (dir) ? tmpNode->Left : tmpNode->Right;
    }
    if (source == dest) return pair<bool, int>(false, leaf);
    if (dir) {
        if (source->Right != nullptr) {               // there are no left child, if exist right child here, move it up to this deleted node space
            source->Right->Parent = source->Parent;
            source->Parent->Left = source->Right;
            source->Right->Child = 'l';
            source->Right = nullptr;
            nodeup = true;
        }
        else {
            source->Parent->Left = nullptr;
        }
    }
    else {
        if (source->Left != nullptr) {
            source->Left->Parent = source->Parent;
            source->Parent->Right = source->Left;
            source->Left->Child = 'r';
            source->Left = nullptr;
            nodeup = true;
        }
        else {
            source->Parent->Right = nullptr;
        }
    }
    source->Parent = dest;

    tmpNode = (dir) ? dest->Left : dest->Right;
    if (tmpNode == source) return pair<bool, int>(false, leaf);

    if (dir) dest->Left = source;
    else dest->Right = source;
    if (tmpNode != nullptr) tmpNode->Parent = source;
    if (dir) source->Left = tmpNode;
    else source->Right = tmpNode;
    return pair<bool, int>(nodeup, leaf);
}

void BST::MoveBack(int a, int b, pair<bool, int> moved) {
    NODE* source = (moved.second == -1) ? NodeMap[a] : NodeMap[moved.second];
    NODE* dest = NodeMap[b];
    NODE* changeNode;
    NODE* tmpNode;
    bool dir;

    if (NodeMap[a]->Child == 'l') dir = true;
    else if (NodeMap[a]->Child == 'r') dir = false;
    else dir = (NodeMap[a]->Left != nullptr) ? true : false;

    tmpNode = (dir) ? source->Left : source->Right;
    while (tmpNode != nullptr) {
        source = (dir) ? source->Left : source->Right;
        tmpNode = (dir) ? tmpNode->Left : tmpNode->Right;
    }

    changeNode = (dir) ? dest->Left : dest->Right; // this is the node we move to, no we need to release it to the original place
    // now source is the node which were the right (dir = true), now we need to make it back
    if (source == dest || source == changeNode) { // or its parent
        tmpNode = source->Parent;
        while (source != NodeMap[a]) {
            Swap(source->Index, tmpNode->Index);
            source = source->Parent;
            tmpNode = tmpNode->Parent;
        }
        return;
    }
    
    if (dir) {
        if (!moved.first) {
            if (changeNode->Left != nullptr) changeNode->Left->Parent = dest;
            dest->Left = changeNode->Left;
            changeNode->Left = nullptr;
            changeNode->Parent = source;
            source->Left = changeNode; 
        }
        else {
            changeNode->Parent = source->Parent;
            source->Parent->Left = changeNode;
            source->Parent = changeNode;
            changeNode->Right = source;
            source->Child = 'r';
        }
    }
    else {
        if (!moved.first) {
            if (changeNode->Right != nullptr) changeNode->Right->Parent = dest;
            dest->Right = changeNode->Right;
            changeNode->Right = nullptr;
            changeNode->Parent = source; 
            source->Right = changeNode; 
        }
        else {
            changeNode->Parent = source->Parent;
            source->Parent->Right = changeNode;
            source->Parent = changeNode;
            changeNode->Left = source;
            source->Child = 'l';
        }
    }
    if (moved.second == -1) {
        tmpNode = changeNode->Parent;
        while (changeNode !=  NodeMap[a]) {
            Swap(changeNode->Index, tmpNode->Index);
            changeNode = changeNode->Parent;
            tmpNode = tmpNode->Parent;
        }
    }
    else {
        tmpNode = changeNode->Parent;
        while (tmpNode !=  NodeMap[moved.second]) {
            Swap(changeNode->Index, tmpNode->Index);
            changeNode = changeNode->Parent;
            tmpNode = tmpNode->Parent;
        }
    }
    return;
}