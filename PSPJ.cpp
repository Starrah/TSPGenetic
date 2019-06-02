#include <iostream>
#include <fstream>
#include "Calculate.h"

void printResult(Graph& g, int pointCount, int* bestSolution, std::ostream &outputStream) {
	for (int i = 0; i < pointCount-1; i++) {
		outputStream << bestSolution[i] << "->";
	}
	outputStream << bestSolution[pointCount - 1];
	outputStream << std::endl;
}

int main()
{
	std::ifstream file("Test103.txt");
	Graph g(file);
	solution s(g.pointCount, 500, 3);
	calculate(g, 10, 0.4, 0.7, 10000, 0.3, s);
	std::cout << s.totalS.generation << " " << s.totalS.length<<" ";
	printResult(g, g.pointCount, s.totalS.plan, std::cout);
}

