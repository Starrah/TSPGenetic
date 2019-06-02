#pragma once
#include <iostream>
#include <algorithm>

//标示道路的结构体，相当于道路矩阵中的一列：包含起点、终点、权值


class Graph {
public:
	int** matrix = nullptr;
	int roadCount = 0;//道路计数
	int pointCount = 0;//结点计数
	Graph(std::istream& inputStream) {
		inputStream >> pointCount;
		double* x = new double[pointCount];
		double* y = new double[pointCount];
		int ttt;
		for (int pp = 0; pp < pointCount; pp++) {
			//inputStream >> ttt >> x[pp] >> y[pp];
		}
		int ttRoadCount = pointCount * (pointCount - 1) / 2;
		int tempWeight;
		matrix = new int*[pointCount];
		for (int i = 0; i < pointCount; i++) {
			int* curLine = new int[pointCount];
			matrix[i] = curLine;
			for (int j = 0; j < pointCount; j++) {
				//tempWeight = (int)(sqrt((x[i] - x[j])*(x[i] - x[j]) + (y[i] - y[j])*(y[i] - y[j]))) + 1;
				inputStream >> tempWeight;
				curLine[j] = tempWeight;
			}
		}
		delete[] x;
		delete[] y;
		std::ofstream file("1.txt");
		file << pointCount << std::endl;
		for (int i = 0; i < pointCount; i++) {
			for (int j = 0; j < pointCount; j++) {
				file << matrix[i][j] << " ";
			}
			file << std::endl;
		}
	}
	Graph(const Graph& g) = delete;
	~Graph() {
		for (int i = 0; i < pointCount; i++) {
			delete[] matrix[i];
		}
		delete[] matrix;
	}

	int calculateRoadLength(int* c) {
		int totalLength = 0;
		for (int i = 0; i < pointCount - 1; i++) {
			totalLength += matrix[c[i]][c[i + 1]];
		}
		totalLength += matrix[c[pointCount - 1]][c[0]];
		return totalLength;
	}
};