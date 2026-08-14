#include <bits/stdc++.h>
#define rep(i,a,b) for (int i=a;i<b;i++)

using namespace std;

int main() {
    int n; cin >> n;
    vector <vector <int>> matriz(n, vector <int> (n));

    vector <bool> visited(n, false);

    rep(i,0,n) 
        rep(j,0,n) 
            cin >> matriz[i][j];
    
    int ones = 0;
    rep(i,0,n) {
        rep(j,0,n) {
            if (matriz[i][j] == 1) ones++;
        }
    }

    int row = 0;
    bool ok = false;
    int inicial = -1;
    rep(j,0,n) {
        if (matriz[row][j] != 1) continue;
        else {
            if (!ok) {
                inicial = row;
                ok = true;
            } 
            visited[j] = true;
            visited[row] = true;
            if (j == inicial) break;
        }
        row = j;
        j = 0;
    }

    int visiteds = 0;
    for (auto w : visited) {
        if (w) visiteds++;
    }

    if (visiteds == ones) cout << "Eba!" << endl;
    else cout << "F..." << endl;

}