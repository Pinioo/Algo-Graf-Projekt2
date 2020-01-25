#include <iostream>
#include <list>
#include <stack>
#include <cmath>
#include <cstring>
#include <sstream>
#include <unordered_map>

using namespace std;

enum logic{
    UNDEFINED, FALSE, TRUE, DEFINITELY_TRUE, DEFINITELY_FALSE
};

int min_cost = -2;
int max_cost = INT32_MAX;

class Edge;

class EdgesCycle {
public:
    list<Edge*>* edges;
    int min_cost;
    int max_cost;
    int logicalValue;

    EdgesCycle(){
        this->edges = new list<Edge*>();
        this->min_cost = -2;
        this->max_cost = INT32_MAX;
        this->logicalValue = logic::UNDEFINED;
    }
};

class Edge {
public:
    int from;
    int to;
    int min;
    int max;

    Edge(int from, int to, int min, int max){
        this->from = from;
        this->to = to;
        this->min = min;
        this->max = max;
    }

    string toString(){
        ostringstream sStream;
        sStream << this->from << " " << this->to;
        return sStream.str();
    }
};

list<Edge*>* trimAway(list<Edge*>* edgesFrom[], list<Edge*>* edgesTo[], Edge* e, bool visited[]);
list<Edge*>* trimToCycle(list<Edge*>* edgesFrom[], list<Edge*>* edgesTo[], Edge* e, bool visited[]);

list<Edge*>* trimToCycle(list<Edge*>* edgesFrom[], list<Edge*>* edgesTo[], Edge* e, bool visited[]){
    min_cost = max(min_cost, e->min);
    max_cost = min(max_cost, e->max);
    if(max_cost < min_cost)
        return nullptr;

    auto* toReturn = new list<Edge*>();
    visited[e->from] = true;
    visited[e->to] = true;
    edgesFrom[e->from]->remove(e);
    edgesTo[e->to]->remove(e);
    toReturn->push_back(e);

    list<Edge*>* returned = nullptr;
    if (!edgesTo[e->to]->empty()) {
        returned = trimAway(edgesFrom, edgesTo, edgesTo[e->to]->front(), visited);
        if (returned != nullptr) {
            toReturn->merge(*returned);
        }
        else return nullptr;
    }

    if (!edgesFrom[e->from]->empty()) {
        returned = trimAway(edgesFrom, edgesTo, edgesFrom[e->from]->front(), visited);
        if (returned != nullptr) {
            toReturn->merge(*returned);
        }
        else return nullptr;
    }

    if (edgesFrom[e->to]->size() == 1) {
        returned = trimToCycle(edgesFrom, edgesTo, edgesFrom[e->to]->front(), visited);
        if (returned != nullptr) {
            toReturn->merge(*returned);
        }
        else return nullptr;
    }

    if (edgesTo[e->from]->size() == 1) {
        returned = trimToCycle(edgesFrom, edgesTo, edgesTo[e->from]->front(), visited);
        if (returned != nullptr) {
            toReturn->merge(*returned);
        }
        else return nullptr;
    }

    delete returned;
    return toReturn;
}

list<Edge*>* trimAway(list<Edge*>* edgesFrom[], list<Edge*>* edgesTo[], Edge* e, bool visited[]){
    list<Edge*>* toReturn = nullptr;
    visited[e->from] = true;
    visited[e->to] = true;
    edgesFrom[e->from]->remove(e);
    edgesTo[e->to]->remove(e);

    list<Edge*>* returned = nullptr;
    if (!edgesTo[e->to]->empty()) {
        returned = trimToCycle(edgesFrom, edgesTo, edgesTo[e->to]->front(), visited);
        if (returned != nullptr) {
            toReturn = returned;
        }
        else return nullptr;
    }

    if (!edgesFrom[e->from]->empty()) {
        returned = trimToCycle(edgesFrom, edgesTo, edgesFrom[e->from]->front(), visited);
        if (returned != nullptr) {
            if(toReturn != nullptr) {
                toReturn->merge(*returned);
                delete returned;
            }
            else toReturn = returned;
        }
        else return nullptr;
    }

    return toReturn;
}

list<Edge*>* trimGraph(list<Edge*>* edgesFrom[], list<Edge*>* edgesTo[], int V){
    bool* visited = new bool[V+1]();
    for(int i = 1; i <= V; ++i)
        visited[i] = false;
    auto* toReturn = new list<Edge*>();
    list<Edge*>* returned = nullptr;
    for (int i = 1; i <= V; ++i){
        if (!visited[i]) {
            if (edgesFrom[i]->empty())
                return nullptr;
            if (edgesFrom[i]->size() == 1){
                visited[i] = true;
                returned = trimToCycle(edgesFrom, edgesTo, edgesFrom[i]->front(), visited);
                if (returned != nullptr) {
                    toReturn->merge(*returned);
                    delete returned;
                }
                else return nullptr;
            }
        }
        if(!visited[i]){
            if (edgesTo[i]->empty())
                return nullptr;
            if (edgesTo[i]->size() == 1) {
                visited[i] = true;
                returned = trimToCycle(edgesFrom, edgesTo, edgesTo[i]->front(), visited);
                if (returned != nullptr) {
                    toReturn->merge(*returned);
                    delete returned;
                }
                else return nullptr;
            }
        }
    }
    delete[] visited;
    return toReturn;
}

void edgesDFS(unordered_map<int, list<int>*>& edgesGraph, int edge, int cycleNumber, unordered_map<int,int>& cyclesMap, unordered_map<int,bool>& visited){
    visited[edge] = true;

    cyclesMap[edge] = cycleNumber;
    for(int e: *edgesGraph[edge]){
        if(!visited[e]){
            edgesDFS(edgesGraph, e, cycleNumber, cyclesMap, visited);
        }
    }
}

bool setDefinitelyTrue(EdgesCycle** cycles, unordered_map<int, int>& mapToCycle, unordered_map<Edge*, int>& edgeToVertex, int cycle);
bool setDefinitelyFalse(EdgesCycle** cycles, unordered_map<int, int>& mapToCycle, unordered_map<Edge*, int>& edgeToVertex, int cycle);

bool setDefinitelyTrue(EdgesCycle** cycles, unordered_map<int, int>& mapToCycle, unordered_map<Edge*, int>& edgeToVertex, int cycle){
    cycles[cycle]->logicalValue = logic::DEFINITELY_TRUE;
    min_cost = max(min_cost, cycles[cycle]->min_cost);
    max_cost = min(max_cost, cycles[cycle]->max_cost);
    if(max_cost < min_cost)
        return false;
    Edge* e = cycles[cycle]->edges->front();
    if(cycles[mapToCycle[-edgeToVertex[e]]]->logicalValue == logic::UNDEFINED){
        if(!setDefinitelyFalse(cycles, mapToCycle, edgeToVertex, mapToCycle[-edgeToVertex[e]])){
            return false;
        }
    } else if (cycles[mapToCycle[-edgeToVertex[e]]]->logicalValue == logic::DEFINITELY_TRUE) {
        return false;
    }
    return true;
}

bool setDefinitelyFalse(EdgesCycle** cycles, unordered_map<int, int>& mapToCycle, unordered_map<Edge*, int>& edgeToVertex, int cycle){
    cycles[cycle]->logicalValue = logic::DEFINITELY_FALSE;
    Edge* e = cycles[cycle]->edges->front();
    if(cycles[mapToCycle[-edgeToVertex[e]]]->logicalValue == logic::UNDEFINED){
        if(!setDefinitelyTrue(cycles, mapToCycle, edgeToVertex, mapToCycle[-edgeToVertex[e]])) {
            return false;
        }
    } else if (cycles[mapToCycle[-edgeToVertex[e]]]->logicalValue == logic::DEFINITELY_FALSE) {
        return false;
    }
    return true;
}

bool setTrue(EdgesCycle** cycles, unordered_map<int, int>& mapToCycle, unordered_map<Edge*, int>& edgeToVertex, int cycle, int cycleNumber, int curMin, int curMax);
bool setFalse(EdgesCycle** cycles, unordered_map<int, int>& mapToCycle, unordered_map<Edge*, int>& edgeToVertex, int cycle, int cycleNumber, int curMin, int curMax);

bool setTrue(EdgesCycle** cycles, unordered_map<int, int>& mapToCycle, unordered_map<Edge*, int>& edgeToVertex, int cycle, int cycleNumber, int curMin, int curMax){
    if(cycle == cycleNumber){
        min_cost = curMin;
        max_cost = curMax;
        return true;
    }
    else {
        int counterLogicalValue = cycles[mapToCycle[-edgeToVertex[cycles[cycle]->edges->front()]]]->logicalValue;
        if (counterLogicalValue == logic::TRUE){
            return false;
        }
        else {
            bool result;
            curMin = max(curMin, cycles[cycle]->min_cost);
            curMax = min(curMax, cycles[cycle]->max_cost);
            if (curMin > curMax) return false;

            cycles[cycle]->logicalValue = logic::TRUE;
            do {
                ++cycle;
            } while(cycle < cycleNumber && cycles[cycle]->logicalValue != UNDEFINED);
            result = setFalse(cycles, mapToCycle, edgeToVertex, cycle, cycleNumber, curMin, curMax) || setTrue(cycles, mapToCycle, edgeToVertex, cycle, cycleNumber, curMin, curMax);
            if(result)
                return true;
            cycles[cycle]->logicalValue = logic::UNDEFINED;
            return false;
        }
    }
}

bool setFalse(EdgesCycle** cycles, unordered_map<int, int>& mapToCycle, unordered_map<Edge*, int>& edgeToVertex, int cycle, int cycleNumber, int curMin, int curMax){
    if(cycle >= cycleNumber){
        min_cost = curMin;
        max_cost = curMax;
        return true;
    }
    else {
        int counterLogicalValue = cycles[mapToCycle[-edgeToVertex[cycles[cycle]->edges->front()]]]->logicalValue;
        if (counterLogicalValue == logic::FALSE){
            return false;
        }
        else {
            bool result;
            cycles[cycle]->logicalValue = logic::FALSE;
            do {
                ++cycle;
            } while(cycle < cycleNumber && cycles[cycle]->logicalValue != UNDEFINED);
            result = setFalse(cycles, mapToCycle, edgeToVertex, cycle, cycleNumber, curMin, curMax) || setTrue(cycles, mapToCycle, edgeToVertex, cycle, cycleNumber, curMin, curMax);
            if(result)
                return true;
            cycles[cycle]->logicalValue = logic::UNDEFINED;
            return false;
        }
    }
}

list<Edge*>* findCycles(list<Edge*>* edgesFrom[], list<Edge*>* edgesTo[], int V, int E){
    auto *toReturn = new list<Edge*>();
    if(E != 2*V) {
        toReturn = trimGraph(edgesFrom, edgesTo, V);
        if (toReturn == nullptr) return nullptr;
        if (toReturn->size() == V) return toReturn;
    }
    unordered_map<Edge*, int> edgeToVertex;
    unordered_map<int, Edge*> vertexToEdge;
    unordered_map<int, list<int>*> edgesGraph;
    unordered_map<int, bool> visited;
    for (int i = 1; i <= V; ++i){
        if (!edgesFrom[i]->empty()){
            edgeToVertex[edgesFrom[i]->front()] = -i;
            edgeToVertex[edgesFrom[i]->back()] = i;

            vertexToEdge[-i] = edgesFrom[i]->front();
            vertexToEdge[i] = edgesFrom[i]->back();

            edgesGraph[-i] = new list<int>();
            edgesGraph[i] = new list<int>();

            visited[-i] = false;
            visited[i] = false;
        }
    }
    int index1;
    int index2;
    for (int i = 1; i <= V; ++i) {
        if(!edgesTo[i]->empty()){
            index1 = edgeToVertex[edgesTo[i]->front()];
            index2 = edgeToVertex[edgesTo[i]->back()];
            edgesGraph[-index1]->push_back(index2);
            edgesGraph[-index2]->push_back(index1);
        }
    }
    int cycleNumber = 0;
    unordered_map<int, int> mapToCycle;
    for (pair<int, Edge*> p: vertexToEdge){
        if(!visited[p.first]){
            edgesDFS(edgesGraph, p.first, cycleNumber, mapToCycle, visited);
            ++cycleNumber;
        }
    }

    auto** cycles = new EdgesCycle*[cycleNumber];
    for(int i = 0; i < cycleNumber; ++i){
        cycles[i] = new EdgesCycle();
    }

    for(pair<int, int> p: mapToCycle){
        cycles[p.second]->edges->push_back(vertexToEdge[p.first]);
        cycles[p.second]->min_cost = max(cycles[p.second]->min_cost, vertexToEdge[p.first]->min);
        cycles[p.second]->max_cost = min(cycles[p.second]->max_cost, vertexToEdge[p.first]->max);
    }

    for(int i = 0; i < cycleNumber; ++i) {
        if (cycles[i]->max_cost < cycles[i]->min_cost || cycles[i]->max_cost < min_cost || cycles[i]->min_cost > max_cost) {
            if(cycles[i]->logicalValue == logic::DEFINITELY_TRUE){
                return nullptr;
            } else if(cycles[i]->logicalValue == logic::UNDEFINED) {
                if(!setDefinitelyFalse(cycles, mapToCycle, edgeToVertex, i)) return nullptr;
            }
        }
    }

    int cycle = 0;
    while(cycle < cycleNumber && (cycles[cycle]->logicalValue == logic::DEFINITELY_TRUE || cycles[cycle]->logicalValue == logic::DEFINITELY_FALSE)){
        ++cycle;
    }

    bool result = setFalse(cycles, mapToCycle, edgeToVertex, cycle, cycleNumber, min_cost, max_cost) || setTrue(cycles, mapToCycle, edgeToVertex, cycle, cycleNumber, min_cost, max_cost);
    if(result) {
        for(int i = 0; i < cycleNumber; ++i){
            if(cycles[i]->logicalValue == logic::DEFINITELY_TRUE || cycles[i]->logicalValue == logic::TRUE)
                toReturn->merge(*cycles[i]->edges);
        }
        return toReturn;
    } else
        return nullptr;
}

int main() {
    int N;
    int V, E;
    int from, to, min, max;
    Edge* tmp;
    cin >> N;
    for(int n = 0; n < N; ++n) {
        cin >> V >> E;
        min_cost = -100;
        max_cost = INT32_MAX;
        auto *edgesFrom = new list<Edge *> *[V + 1];
        auto *edgesTo = new list<Edge *> *[V + 1];
        for (int i = 1; i <= V; ++i) {
            edgesFrom[i] = new list<Edge *>();
            edgesTo[i] = new list<Edge *>();
        }
        for (int i = 0; i < E; ++i) {
            cin >> from >> to >> min >> max;
            tmp = new Edge(from, to, min, max);
            edgesFrom[from]->push_back(tmp);
            edgesTo[to]->push_back(tmp);
        }
        auto edgesList = findCycles(edgesFrom, edgesTo, V, E);
        if (edgesList == nullptr) cout << -1 << endl;
        else if(edgesList->empty()) cout << -1 << endl;
        else {
            cout << min_cost << endl;
            for (auto e: *edgesList) cout << e->toString() << endl;
        }
        for (int i = 1; i <= V; ++i){
            delete edgesFrom[i];
            delete edgesTo[i];
        }
        delete[] edgesFrom;
        delete[] edgesTo;
    }
    return 0;
}
