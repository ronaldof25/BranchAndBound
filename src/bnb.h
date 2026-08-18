#ifndef BNB_H
#define BNB_H

#include "Data.h"
#include <vector>

struct Node {
    vector <pair<int,int>> forbidden_arcs;
    vector <vector<int>> subtour;
    double lower_bound;
    int chosen;
    bool feasible;

    bool operator<(const Node& other) const {
        return lower_bound > other.lower_bound;
    }
};

Node isViavel(const vector <vector <int>>& matriz, int n);

Node branchBound(Data data, string UB, string estrategia);





#endif