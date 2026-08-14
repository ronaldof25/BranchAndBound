#include "bnb.h"
#include "Data.h"
#include "hungarian.h"
#include <bits/stdc++.h>

using namespace std;


Node isViavel(const vector <vector <int>>& matriz, int n) {
    Node s;
    vector <vector<int>> subtours;
    vector <bool> visited(n, false);
    for (int i=0;i<n;i++) {
        if (visited[i]) continue;

        vector <int> subtour_atual;

        int row = i;
        bool ok = false;
        int inicial = -1;
        for (int j=0;j<n;j++) {
            if (matriz[row][j] != 1) continue;
            else {
                if (!ok) {
                    inicial = row;
                    ok = true;
                } 
                visited[j] = true;
                visited[row] = true;
                subtour_atual.push_back(row);
                if (j == inicial) break;
            }
            row = j;
            j = -1;
        }
        subtours.push_back(subtour_atual);
    }

    if (subtours.size() == 1) {
        s.feasible = true;
        s.chosen = 0;
    }
    else {
        int mini = 1e9;
        int idx;
        for (int i=0;i<subtours.size();i++) {
            if (subtours[i].size() < mini) {
                mini = subtours[i].size();
                idx = i;
            }
        }
        s.chosen = idx;
        s.feasible = false;
    }
    s.subtour = subtours;
    return s;
}

vector<vector<int>> copiar(int** matriz, int n) {
    vector <vector <int>> resultado(n, vector <int> (n));
    for (int i=0;i<n;i++) {
        for (int j=0;j<n;j++) {
            resultado[i][j] = matriz[i][j];
        }
    }
    return resultado;
}

void updateNode(Node *node, Data *data) {
    int n = data->getDimension();
    hungarian_problem_t p;
    double** cost = data->getMatrixCost();
    
    //atualizar a matriz cost adicionando as restricoes com base em forbiddenarcs
    //cost[i][j] = inf, sendo i e j um arco de forbidden

    for (int i=0;i<node->forbidden_arcs.size();i++) {
        cost[node->forbidden_arcs[i].first][node->forbidden_arcs[i].second] = 99999999;
    }
    int mode = HUNGARIAN_MODE_MINIMIZE_COST;
    hungarian_init(&p, cost, n, n, mode);
    
    node->lower_bound = hungarian_solve(&p);

    vector<vector<int>> assignment = copiar(p.assignment, n); 

    Node result = isViavel(assignment, n);

    node->subtour = result.subtour;
    node->chosen = result.chosen;
    node->feasible = result.feasible;

    hungarian_free(&p);
}

Node branchingStrategy(list <Node>& tree, string estrategia) {
    if (estrategia == "DFS") {
        Node n = tree.front();
        tree.pop_front();
        return n;
    }
    else if (estrategia == "BFS") {
        Node n = tree.back();
        tree.pop_back();
        return n;
    }
    //BBS
    else {

    }
}

//implementacao da priority_queue

int parent(int i) {return (i-1)/2; }

int leftChild(int i) { return 2 * i + 1; }

int rightChild(int i) { return 2*i + 2; }

void shiftUp(int i, vector <int> &arr) {
    while (i > 0 && arr[parent(i)] > arr[i]) {
        swap(arr[parent(i)], arr[i]);
        i = parent(i);
    }
}


void shiftDown(int i, vector <int> &arr, int size) {
    int minIndex = i;
    int l  = leftChild(i);
    if (l < size && arr[l] < arr[minIndex]) minIndex = l;
    int r = rightChild(i);
    if (r < size && arr[r] < arr[minIndex]) minIndex = r;

    if (i != minIndex) {
        swap(arr[i], arr[minIndex]);
        shiftDown(minIndex, arr, size);
    }
}

void insert(int p, vector <int> &arr) {
    arr.push_back(p);
    shiftUp(arr.size()-1, arr);
}

int pop(vector<int> &arr) {
    int size = arr.size();
    if (size == 0) return -1;
    int result = arr[0];
    arr[0] = arr[size - 1];
    arr.pop_back();
    shiftDown(0, arr, arr.size());
    return result;
}

int getMin(vector <int> &arr) {
    if (arr.empty()) return -1;
    return arr[0];
}

void branchBound(Data data, string UB, string estrategia) {
    Node root; //no raiz
    updateNode(&root, &data); //resolver e atualizar a raiz a partir da instancia original

    //criacao da arvore
    list <Node> tree;
    tree.push_back(root);

    double upper_bound = stod(UB)+1; //// passar como argumento o valor otimo da instancia + 1

    while (!tree.empty()) {
        auto node = branchingStrategy(tree, estrategia); // escolher um dos nos da arvore e remove-lo

        if (node.feasible) {
            if (node.lower_bound < upper_bound) upper_bound = node.lower_bound;
            continue;
        }
        //Adicionando os filhos
        for (int i=0;i<node.subtour[node.chosen].size() - 1;i++) {
            //iterar por todos os arcos do subtour escolhido
            Node n;
            n.forbidden_arcs = node.forbidden_arcs;

            pair<int,int> forbidden_arc = {
                node.subtour[node.chosen][i], node.subtour[node.chosen][i+1]
            };

            n.forbidden_arcs.push_back(forbidden_arc);
            updateNode(&n, &data);

            if (n.lower_bound <= upper_bound) tree.push_back(n); //inserir novos nos na arvore
        }
    }
}

