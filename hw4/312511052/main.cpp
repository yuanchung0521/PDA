#include "classtype.h"

vector<Boundary> topBoundaries, bottomBoundaries;
map<int, Net*> nets;
vector<Segment*> segments;
vector<Track> availableTrack;
int maxTop=0, maxBot=0, xlen, cNum;

void printCurrent () {
    cout << "===== Boundaries =====" << endl;
    cout << "top" << endl;
    for (Boundary b : topBoundaries) {
        cout << b.index << " " << b.start << "-" << b.end << endl;
    }
    cout << "bottom" << endl;
    for (Boundary b : bottomBoundaries) {
        cout << b.index << " " << b.start << "-" << b.end << endl;
    }
    cout << endl;

    cout << "===== Segments =====" << endl;
    for (Segment* seg : segments) {
        cout << seg->net->id << "-" << seg->segId << "(" << seg->start << ", " << seg->end << ") ";
        for (Segment* segg : seg->vc) {
            cout << segg->net->id << "-" << segg->segId << " ";
        }
        cout << seg->track << endl;
    }

    cout << "===== Nets ======" << endl;
    for (auto& net : nets) {
        cout << net.second->id << ": ";
        for (Segment* seg : net.second->segments) {
            cout << seg->segId << "(" << seg->start << ", " << seg->end << ") ";
        }
        cout << endl;
    }
}

void readInput (ifstream &InputFile) {
    string line, line2;
    string type;
    int start, end, netId1, netId2, pos;
    Net* net;
    Net* tmpNet;
    Segment* tmpSeg;

    while (getline(InputFile, line)) {
        stringstream ss(line);
    
        if (line[0] == 'T') {
            ss >> type >> start >> end;
            topBoundaries.push_back({type[0], stoi(type.substr(1)), start, end});
            if (stoi(type.substr(1)) > maxTop) maxTop = stoi(type.substr(1));
        } else if (line[0] == 'B') {
            ss >> type >> start >> end;
            bottomBoundaries.push_back({type[0], stoi(type.substr(1)), start, end});
            if (stoi(type.substr(1)) > maxBot) maxBot = stoi(type.substr(1));
        }
        else {
            pos = 0;
            getline(InputFile, line2);
            stringstream ss2(line2);
            while ((ss >> netId1) && (ss2 >> netId2)) {
                tmpSeg = nullptr;
                tmpNet = nullptr;
                // cout << netId1 << " " << netId2 << endl;
                if (netId1 != 0) {
                    if (nets.find(netId1) == nets.end()) {
                        net = new Net(netId1, pos);
                        nets.insert(pair<int, Net*>(netId1, net));
                    } else net = nets[netId1];
                    
                    if (net->tmpSeg != nullptr) {
                        net->tmpSeg->end = pos;
                        segments.push_back(net->tmpSeg);
                    }
                    tmpSeg = net->tmpSeg;
                    tmpNet = net;
                    net->tmpSeg = new Segment(net->segments.size(), pos, -1, net);
                }

                if (netId2 != 0) {
                    if (nets.find(netId2) == nets.end()) {
                        net = new Net(netId2, pos);
                        nets.insert(pair<int, Net*>(netId2, net));
                    } else net = nets[netId2];

                    if (net->tmpSeg != nullptr) {
                        net->tmpSeg->end = pos;
                        if (tmpNet != nullptr) net->tmpSeg->vc.push_back(tmpNet->tmpSeg);
                        if (tmpSeg != nullptr) net->tmpSeg->vc.push_back(tmpSeg);
                        segments.push_back(net->tmpSeg);
                    }
                    
                    net->tmpSeg = new Segment(net->segments.size(), pos, -1, net);
                    if (tmpNet != nullptr) net->tmpSeg->vc.push_back(tmpNet->tmpSeg);
                    if (tmpSeg != nullptr) net->tmpSeg->vc.push_back(tmpSeg);
                }
                pos ++;
                // cout << pos << endl;
            }

            for (Segment* seg : segments) {
                for (int i=0; i<seg->vc.size(); i++) {
                    if (seg->vc[i]->end == -1) {
                        seg->vc.erase(seg->vc.begin()+i);
                    }
                }
            }
        }
    }
    xlen = pos;
}

bool compareByStart (Segment* a, Segment* b) {
    if (a->start == b->start) {
        return a->end < b->end;
    } else return a->start < b->start;
}

void constructGrid () {
    for (int i=maxTop; i>=0; i--) {
        Track track("T" + to_string(i));
        availableTrack.push_back(track);
    }

    Track track("C");
    track.boundaries.push_back(pair<int, int>(0, xlen-1));
    availableTrack.push_back(track);
    cNum = 1;

    for (int i=0; i<=maxBot; i++) {
        Track track("B" + to_string(i));
        availableTrack.push_back(track);
    }

    for (int i=0; i<topBoundaries.size(); i++) {
        if (i==0) {
            if (topBoundaries.size()>1) {
                if (topBoundaries[i+1].index<topBoundaries[i].index) {
                    for (int j=maxTop-(topBoundaries[i].index-1); j<=maxTop; j++) {
                        availableTrack[j].boundaries.push_back(pair<int, int>(topBoundaries[i].start, topBoundaries[i].end-1));
                    }
                } else {
                    for (int j=maxTop-(topBoundaries[i].index-1); j<=maxTop; j++) {
                        availableTrack[j].boundaries.push_back(pair<int, int>(topBoundaries[i].start, topBoundaries[i].end));
                    }
                }
            } else {
                for (int j=maxTop-(topBoundaries[i].index-1); j<=maxTop; j++) {
                    availableTrack[j].boundaries.push_back(pair<int, int>(topBoundaries[i].start, topBoundaries[i].end));
                }
            }
        } else if (i==topBoundaries.size()-1) {
            if (topBoundaries[i-1].index<topBoundaries[i].index) {
                for (int j=maxTop-(topBoundaries[i].index-1); j<=maxTop; j++) {
                    availableTrack[j].boundaries.push_back(pair<int, int>(topBoundaries[i].start+1, topBoundaries[i].end));
                }
            } else {
                for (int j=maxTop-(topBoundaries[i].index-1); j<=maxTop; j++) {
                    availableTrack[j].boundaries.push_back(pair<int, int>(topBoundaries[i].start, topBoundaries[i].end));
                }
            }
        } else {
            if (topBoundaries[i-1].index<topBoundaries[i].index) {
                if (topBoundaries[i+1].index<topBoundaries[i].index) {
                    for (int j=maxTop-(topBoundaries[i].index-1); j<=maxTop; j++) {
                        availableTrack[j].boundaries.push_back(pair<int, int>(topBoundaries[i].start+1, topBoundaries[i].end-1));
                        cout << topBoundaries[i].start+1 << " " << topBoundaries[i].end-1 << endl;
                    }
                } else {
                    for (int j=maxTop-(topBoundaries[i].index-1); j<=maxTop; j++) {
                        availableTrack[j].boundaries.push_back(pair<int, int>(topBoundaries[i].start+1, topBoundaries[i].end));
                    }
                }
            } else {
                if (topBoundaries[i+1].index<topBoundaries[i].index) {
                    for (int j=maxTop-(topBoundaries[i].index-1); j<=maxTop; j++) {
                        availableTrack[j].boundaries.push_back(pair<int, int>(topBoundaries[i].start, topBoundaries[i].end-1));
                        cout << topBoundaries[i].start+1 << " " << topBoundaries[i].end-1 << endl;
                    }
                } else {
                    for (int j=maxTop-(topBoundaries[i].index-1); j<=maxTop; j++) {
                        availableTrack[j].boundaries.push_back(pair<int, int>(topBoundaries[i].start, topBoundaries[i].end));
                    }
                }
            }
        }
    }

    for (int i=0; i<bottomBoundaries.size(); i++) {
        if (i==0) {
            if (bottomBoundaries.size()>1) {
                if (bottomBoundaries[i+1].index<bottomBoundaries[i].index) {
                    for (int j=maxTop+2+(bottomBoundaries[i].index-1); j>maxTop+1; j--) {
                        availableTrack[j].boundaries.push_back(pair<int, int>(bottomBoundaries[i].start, bottomBoundaries[i].end-1));
                    }
                } else {
                    for (int j=maxTop+2+(bottomBoundaries[i].index-1); j>maxTop+1; j--) {
                        availableTrack[j].boundaries.push_back(pair<int, int>(bottomBoundaries[i].start, bottomBoundaries[i].end));
                    }
                }
            } else {
                for (int j=maxTop+2+(bottomBoundaries[i].index-1); j>maxTop+1; j--) {
                    availableTrack[j].boundaries.push_back(pair<int, int>(bottomBoundaries[i].start, bottomBoundaries[i].end));
                }
            }
        } else if (i==bottomBoundaries.size()-1) {
            if (bottomBoundaries[i-1].index<bottomBoundaries[i].index) {
                for (int j=maxTop+2+(bottomBoundaries[i].index-1); j>maxTop+1; j--) {
                    availableTrack[j].boundaries.push_back(pair<int, int>(bottomBoundaries[i].start+1, bottomBoundaries[i].end));
                }
            } else {
                for (int j=maxTop+2+(bottomBoundaries[i].index-1); j>maxTop+1; j--) {
                    availableTrack[j].boundaries.push_back(pair<int, int>(bottomBoundaries[i].start, bottomBoundaries[i].end));
                }
            }
        } else {
            if (topBoundaries[i-1].index<topBoundaries[i].index) {
                if (topBoundaries[i+1].index<topBoundaries[i].index) {
                    for (int j=maxTop+2+(bottomBoundaries[i].index-1); j>maxTop+1; j--) {
                        availableTrack[j].boundaries.push_back(pair<int, int>(bottomBoundaries[i].start+1, bottomBoundaries[i].end-1));
                    }
                } else {
                    for (int j=maxTop+2+(bottomBoundaries[i].index-1); j>maxTop+1; j--) {
                        availableTrack[j].boundaries.push_back(pair<int, int>(bottomBoundaries[i].start+1, bottomBoundaries[i].end));
                    }
                }
            } else {
                if (topBoundaries[i+1].index<topBoundaries[i].index) {
                    for (int j=maxTop+2+(bottomBoundaries[i].index-1); j>maxTop+1; j--) {
                        availableTrack[j].boundaries.push_back(pair<int, int>(bottomBoundaries[i].start, bottomBoundaries[i].end-1));
                    }
                } else {
                    for (int j=maxTop+2+(bottomBoundaries[i].index-1); j>maxTop+1; j--) {
                        availableTrack[j].boundaries.push_back(pair<int, int>(bottomBoundaries[i].start, bottomBoundaries[i].end));
                    }
                }
            }
        }
    }

    for (int i=0; i<availableTrack.size(); i++) {
        for (int j=0; j<availableTrack.size(); j++) {
            if (j+1<availableTrack[i].boundaries.size()) {
                // cout << availableTrack[i].boundaries[j].second << " " << availableTrack[i].boundaries[j+1].first << endl;
                if (availableTrack[i].boundaries[j].second == availableTrack[i].boundaries[j+1].first-1) {
                    availableTrack[i].boundaries[j].second = availableTrack[i].boundaries[j+1].second;
                    availableTrack[i].boundaries.erase(availableTrack[i].boundaries.begin()+j+1);
                }
            }
        }
    }
}

void printGrid () {
    for (int i=0; i<availableTrack.size(); i++) {
        cout << availableTrack[i].name << ": ";
        for (auto &j : availableTrack[i].boundaries) {
            cout << "(" << j.first << ", " << j.second << ") ";
        }
        for (auto &j : availableTrack[i].segments) {
            cout << j->net->id << "-" << j->segId << " ";
        }
        cout << endl;
    }
}

void printUsed () {
    for (Segment* seg : segments) {
        cout << seg->net->id << "-" << seg->segId << " " << seg->used << endl;
    }
}

void constrainedLeftEdge () {
    bool finished = false;
    int usedSeg = 0;
    int currentTrack = 0;
    int watermark;
    Net* tmpNet = nullptr;
    sort(segments.begin(), segments.end(), compareByStart);
    while (!finished) {
        for (int i=currentTrack; i<availableTrack.size(); i++) {
            // cout << "cuurent: " << i << ", " << availableTrack[i].name << endl;
            watermark = -1;
            if (availableTrack[i].boundaries.empty()) continue;
            for (int j=0; j<segments.size(); j++) {
                bool prepared = true;
                // cout << segments[j]->net->id << "-" << segments[j]->segId << endl;
                if (!segments[j]->used && (segments[j]->start>watermark || (segments[j]->net == tmpNet && segments[j]->start>=watermark))){
                    for (int k=0; k<availableTrack[i].boundaries.size(); k++) {
                        if (segments[j]->start<availableTrack[i].boundaries[k].first) {
                            continue;
                        } else {
                            if (segments[j]->end>availableTrack[i].boundaries[k].second) {
                                break;
                            } else {
                                // cout << "find!!!" << segments[j]->start << " " << segments[j]->end << endl;
                                // cout << availableTrack[i].boundaries[k].first << availableTrack[i].boundaries[k].second << endl;
                                for (int l=0; l<segments[j]->vc.size(); l++) {
                                    if (!segments[j]->vc[l]->used) {
                                        prepared = false;
                                        break;
                                    }
                                }
                                if (!prepared) {
                                    break;
                                } else {
                                    // cout << "used" << endl;
                                    availableTrack[i].segments.push_back(segments[j]);
                                    segments[j]->track = availableTrack[i].name;
                                    usedSeg++;
                                    segments[j]->used = true;
                                    watermark = segments[j]->end;
                                    tmpNet = segments[j]->net;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (usedSeg != segments.size()) {
            Track track("C");
            track.boundaries.push_back(pair<int, int>(0, xlen-1));
            availableTrack.insert(availableTrack.begin()+maxTop+cNum+1, track);
            cNum ++;
            for (int i=availableTrack.size()-1; i>maxTop+cNum; i--) {
                for (int j=0; j< availableTrack[i].segments.size(); j++) {
                    availableTrack[i].segments[j]->track = "No";
                    usedSeg--;
                    availableTrack[i].segments[j]->used = false;
                }   
                availableTrack[i].segments.clear();
            }
            currentTrack = maxTop + cNum;
            // cout << "added: " << cNum << ", used: " << usedSeg << ", size: " << segments.size() << endl;
        } else {
            finished = true;
        }
    }

    for (int i=maxTop+cNum; i>maxTop; i--) {
        availableTrack[i].name = "C" + to_string(cNum-(i-maxTop)+1);
        for (Segment* seg : availableTrack[i].segments) {
            seg->track = availableTrack[i].name;
        }
    }
    // printUsed();
    // cout << "added: " << cNum << ", used: " << usedSeg << ", size: " << segments.size() << endl;
}

void outputResult (ofstream & OutputFile) {
    OutputFile << "Channel density: " << cNum << endl;
    for (auto& net : nets) {
        OutputFile << "Net " << net.first << endl;
        for (int i=0; i<net.second->segments.size()-1; i++) {
            OutputFile << net.second->segments[i]->track << " " << net.second->segments[i]->start << " " << net.second->segments[i]->end << endl;
            if (i-1>=0) {
                if (net.second->segments[i]->start == net.second->segments[i-1]->end) {
                    OutputFile << "Dogleg " << net.second->segments[i]->start << endl;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    
    ifstream InputFile(argv[1]);
    ofstream OutputFile(argv[2]);
    
    readInput(InputFile);
    // printCurrent();

    constructGrid();
    // printGrid();
     
    constrainedLeftEdge();
    // printCurrent();
    // printGrid();

    outputResult(OutputFile);
    return 0;
}
