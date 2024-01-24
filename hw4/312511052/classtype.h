#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <map>

using namespace std;

struct Boundary;
struct Net;
struct Segment;
struct Track;

struct Boundary {
    char type;  // 'T' for top, 'B' for bottom
    int index, start, end;
};

struct Net {
    int id;
    vector<Segment*> segments;
    int tmpEnd;
    Segment* tmpSeg;
    
    Net(int _id, int _tmpEnd) : id(_id), tmpEnd(_tmpEnd), tmpSeg(nullptr) {}
};

struct Segment {
    int segId;
    int start, end;
    Net* net;
    bool used;
    string track;
    vector<Segment*> vc;

    Segment(int _segId, int _start, int _end, Net* _net) : segId(_segId), start(_start), end(_end), net(_net), track("No"), used(false) {
        net->segments.push_back(this);
        net->tmpEnd = end;
    }
};

struct Track {
    string name;
    vector<pair<int, int>> boundaries;
    vector<Segment*> segments;
    Track(string _name) : name(_name) {}
};