#include <iostream>
#include <list>
#include <stack>
#include <cmath>
#include <cstring>
#include <sstream>

using namespace std;

const bool DEBUG = false;

int min_cost = -2;
int max_cost = INT32_MAX;

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
    if(DEBUG) cout << "To cycle: " << e->toString() << endl;

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
            if(DEBUG) cout << "Merged1: " << e->toString() << endl;
        }
        else return nullptr;
    }

    if (!edgesFrom[e->from]->empty()) {
        returned = trimAway(edgesFrom, edgesTo, edgesFrom[e->from]->front(), visited);
        if (returned != nullptr) {
            toReturn->merge(*returned);
            if(DEBUG) cout << "Merged2: " << e->toString() << endl;
        }
        else return nullptr;
    }

    if (edgesFrom[e->to]->size() == 1) {
        returned = trimToCycle(edgesFrom, edgesTo, edgesFrom[e->to]->front(), visited);
        if (returned != nullptr) {
            toReturn->merge(*returned);
            if(DEBUG) cout << "Merged3: " << e->toString() << endl;
        }
        else return nullptr;
    }

    if (edgesTo[e->from]->size() == 1) {
        returned = trimToCycle(edgesFrom, edgesTo, edgesTo[e->from]->front(), visited);
        if (returned != nullptr) {
            toReturn->merge(*returned);
            if(DEBUG) cout << "Merged4: " << e->toString() << endl;
        }
        else return nullptr;
    }

    delete returned;
    return toReturn;
}

list<Edge*>* trimAway(list<Edge*>* edgesFrom[], list<Edge*>* edgesTo[], Edge* e, bool visited[]){
    if(DEBUG) cout << "Away : " << e->toString() << endl;

    auto* toReturn = new list<Edge*>();
    visited[e->from] = true;
    visited[e->to] = true;
    edgesFrom[e->from]->remove(e);
    edgesTo[e->to]->remove(e);

    list<Edge*>* returned = nullptr;
    if (!edgesTo[e->to]->empty()) {
        returned = trimToCycle(edgesFrom, edgesTo, edgesTo[e->to]->front(), visited);
        if (returned != nullptr) {
            toReturn->merge(*returned);
            if(DEBUG) cout << "Merged5: " << e->toString() << endl;
        }
        else return nullptr;
    }

    if (!edgesFrom[e->from]->empty()) {
        returned = trimToCycle(edgesFrom, edgesTo, edgesFrom[e->from]->front(), visited);
        if (returned != nullptr) {
            toReturn->merge(*returned);
            if(DEBUG) cout << "Merged6: " << e->toString() << endl;
        }
        else return nullptr;
    }

    return toReturn;
}

list<Edge*>* trimGraph(list<Edge*>* edgesFrom[], list<Edge*>* edgesTo[], int V){
    bool* visited = new bool[V+1]();
    for(int i = 0; i <= V; ++i)
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
                if (DEBUG) cout << "Edges from " << i << " trimmed to cycle";
                if (returned != nullptr) {
                    toReturn->merge(*returned);
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
                if (DEBUG) cout << "Edgdes to " << i << " trimmed to cycle";
                if (returned != nullptr) {
                    toReturn->merge(*returned);
                }
                else return nullptr;
            }
        }
    }
    delete[] visited;
    return toReturn;
}

list<Edge*>* findCycles(list<Edge*>* edgesFrom[], list<Edge*>* edgesTo[], int V){
    auto* toReturn = trimGraph(edgesFrom, edgesTo, V);
    if (toReturn == nullptr) return nullptr;
    if(DEBUG) cout << "Graph trimmed" << endl;
    Edge* tmp1;
    Edge* tmp2;
    list<Edge*>* returned = nullptr;
    for (int n = 0; n < V; ++n){
        for (int i = 1; i <= V; ++i) {
            if (!edgesFrom[i]->empty()) {
                tmp1 = edgesFrom[i]->front();
                tmp2 = edgesFrom[i]->back();
                if (tmp1->max < min_cost || tmp1->min > max_cost) {
                    edgesFrom[tmp1->from]->remove(tmp1);
                    edgesTo[tmp1->to]->remove(tmp1);
                }
                if (tmp2->max < min_cost || tmp2->min > max_cost) {
                    edgesFrom[tmp2->from]->remove(tmp2);
                    edgesTo[tmp2->to]->remove(tmp2);
                }
                if (edgesFrom[i]->empty())
                    return nullptr;
                if (edgesFrom[i]->size() == 1) {
                    returned = trimToCycle(edgesFrom, edgesTo, edgesFrom[i]->front(), new bool[V + 1]);
                    if (returned == nullptr) return nullptr;
                    else toReturn->merge(*returned);
                }
            }
        }
    }
    return toReturn;
}

int main() {
    int N;
    int V, E;
    int from, to, min, max;
    Edge* tmp;
    cin >> N;
    for(int n = 0; n < N; ++n) {
        cin >> V >> E;
        min_cost = 0;
        max_cost = INT32_MAX;
        auto *edgesFrom = new list<Edge *> *[V + 1];
        auto *edgesTo = new list<Edge *> *[V + 1];
        for (int i = 0; i < V + 1; ++i) {
            edgesFrom[i] = new list<Edge *>();
            edgesTo[i] = new list<Edge *>();
        }
        for (int i = 0; i < E; ++i) {
            cin >> from >> to >> min >> max;
            tmp = new Edge(from, to, min, max);
            edgesFrom[from]->push_back(tmp);
            edgesTo[to]->push_back(tmp);
        }
        auto edgesList = findCycles(edgesFrom, edgesTo, V);
        if (edgesList == nullptr) cout << -1 << endl;
        else if(edgesList->empty()) cout << -1 << endl;
        else {
            cout << min_cost << endl;
            for (auto e: *edgesList) cout << e->toString() << endl;
        }
        for (int i = 0; i < V+1; ++i){
            delete edgesFrom[i];
            delete edgesTo[i];
        }
        delete[] edgesFrom;
        delete[] edgesTo;
    }
    return 0;
}
