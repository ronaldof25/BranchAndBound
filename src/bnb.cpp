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

double** copiarCost(double** original, int n) {
    double** copia = new double*[n];
    for (int i = 0; i < n; i++) {
        copia[i] = new double[n];
        for (int j = 0; j < n; j++) {
            copia[i][j] = original[i][j];
        }
    }
    return copia;
}

void updateNode(Node *node, Data *data) {
    int n = data->getDimension();
    hungarian_problem_t p;
    double** cost = copiarCost(data->getMatrixCost(), n);
    
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

    for (int i = 0; i < n; i++) delete[] cost[i];
    delete[] cost;
}

Node branchingStrategy(list <Node>& tree, string estrategia) {
    if (estrategia == "DFS") {
        Node n = tree.front();
        tree.pop_front();
        return n;
    }
    else {
        Node n = tree.back();
        tree.pop_back();
        return n;
    }
}

Node branchingBBS(priority_queue <Node>& pq) {
    Node n = pq.top();
    pq.pop();
    return n;
}



Node branchBound(Data data, string UB, string estrategia) {
    Node bestNode;
    Node root; //no raiz
    updateNode(&root, &data); //resolver e atualizar a raiz a partir da instancia original
    
    //criacao da arvore
    list<Node> tree;
    priority_queue<Node> pq;

    if (estrategia != "BBS") tree.push_back(root);
    else pq.push(root);

    double upper_bound = stod(UB)+1; //// passar como argumento o valor otimo da instancia + 1


    while (!tree.empty() || !pq.empty()) {
        // escolher um dos nos da arvore e remove-lo
        Node node;
        if (estrategia != "BBS") node = branchingStrategy(tree, estrategia);
        else node = branchingBBS(pq);
        if (node.feasible) {
            if (node.lower_bound < upper_bound) {
                upper_bound = node.lower_bound;
                bestNode = node;
            }
            continue;
        }
        if (node.lower_bound <= upper_bound) {
        //Adicionando os filhos
        for (int i=0;i<node.subtour[node.chosen].size();i++) {
            //iterar por todos os arcos do subtour escolhido
            Node n;
            n.forbidden_arcs = node.forbidden_arcs;

            int a = node.subtour[node.chosen][i];
            int b = node.subtour[node.chosen][(i+1) % node.subtour[node.chosen].size()];

            pair<int,int> forbidden_arc = {a, b};

            n.forbidden_arcs.push_back(forbidden_arc);
            updateNode(&n, &data);
            
            //inserir novos nos na arvore
            if (n.lower_bound <= upper_bound) {
                if (estrategia != "BBS") tree.push_back(n);
                else pq.push(n);
            }
        }
    }
    }
    return bestNode;
}

