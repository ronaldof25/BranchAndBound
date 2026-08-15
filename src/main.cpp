#include <iostream>
#include <chrono>
using namespace std;

#include "Data.h"
#include "hungarian.h"
#include "bnb.h"

int main(int argc, char** argv) {

	Data data = Data(argc, argv[1]);
	data.read();

	string UB = argv[2];
	string estrategia = argv[3];

	auto inicio = chrono::high_resolution_clock::now();

	Node melhorSolucao = branchBound(data, UB, estrategia);

	auto fim = chrono::high_resolution_clock::now();
	chrono::duration<double> tempo = fim - inicio;

	cout << melhorSolucao.lower_bound << endl;
	cout << "Tempo: " << tempo.count() << "s" << endl;
	return 0;
}