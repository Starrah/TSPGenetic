#pragma once
#include "Graph.h"
#include <random>
#include <vector>

struct solution{
	int pc;
	int generation;
	int length;
	int* plan;
	int lastDis;
	int wantDis;
	double DisFactor;
	int curWantDis;
	struct totalSolution {
		int generation;
		int length;
		int* plan = nullptr;
	};
	totalSolution totalS;
	solution(int ppc, int p_wantDis, int p_DisFactor) {
		pc = ppc;
		plan = new int[pc];
		totalS.plan = new int[pc];
		length = INT_MAX;
		totalS.length = INT_MAX;
		generation = -1;
		lastDis = 0;
		wantDis = p_wantDis;
		DisFactor = p_DisFactor;
		curWantDis = wantDis;
	}
};

std::ofstream fileOut("out.txt");

bool saveBest(std::pair<int*, int> newBest, int newGen, solution& oldBest) {
	if (newBest.second < oldBest.length) {
		memcpy(oldBest.plan, newBest.first, sizeof(int)*oldBest.pc);
		oldBest.length = newBest.second;
		oldBest.generation = newGen;
		std::cout << "newBest\t" << newGen << "\t" << oldBest.length << std::endl;
		if (oldBest.length < oldBest.totalS.length) {
			oldBest.totalS.length = oldBest.length;
			oldBest.totalS.generation = oldBest.generation;
			memcpy(oldBest.totalS.plan, oldBest.plan, sizeof(int)*oldBest.pc);
		}
		oldBest.curWantDis = std::max(oldBest.curWantDis, (int)((newGen - oldBest.lastDis)*oldBest.DisFactor));
		oldBest.lastDis = newGen;
		fileOut << newGen << "\t" << oldBest.length << std::endl;
		return false;
	}else{
		if(newGen % 1000 == 0){ fileOut << newGen << "\t" << oldBest.length << std::endl; }
		if (newGen - oldBest.lastDis > oldBest.curWantDis) {
			oldBest.lastDis = newGen;
			oldBest.curWantDis = oldBest.wantDis;
			oldBest.length = INT_MAX;
			std::cout << "Disaster\t" << newGen << std::endl;
			return true;
		}
		return false;
	}
}

void vari(int* c, int pc, std::default_random_engine& engine, double randomRate) {
	std::uniform_int_distribution<int> disSwapCount(1, pc*randomRate);
	std::uniform_int_distribution<int> disPlace(0, pc - 1);
	int swapTotal = disSwapCount(engine);
	for (int i = 0; i < swapTotal; i++) {
		int p1 = disPlace(engine);
		int p2 = disPlace(engine);
		std::swap(c[p1], c[p2]);
	}
}

int* simpleRandom(Graph& g, std::default_random_engine& engine) {
	int* cycle = new int[g.pointCount];
	std::vector<int> pointsList;
	for (int i = 0; i < g.pointCount; i++) {
		pointsList.push_back(i);
	}
	for (int i = 0; i < g.pointCount; i++) {
		std::uniform_int_distribution<int> dis(0, g.pointCount - 1 - i);
		int ri = dis(engine);
		auto iter = pointsList.begin() + ri;
		cycle[i] = *iter;
		pointsList.erase(iter);
	}
	return cycle;
}

bool compareRoadWithRes(std::pair<int*, int>& a, std::pair<int*, int>& b) {
	return a.second < b.second;
}

void calculate(Graph& g, int popuCount, double aliveRate, double variProb, int generationCount, double randomRate, solution& tsolution) {
	std::default_random_engine engine;
	int** population = new int*[popuCount];
	for (int p = 0; p < popuCount; p++) {
		population[p] = simpleRandom(g, engine);
	}
	solution hsolution = tsolution;
	for (int cur = 0; cur < generationCount; cur++) {
		
		std::pair<int*, int>* oneLength = new std::pair<int*, int>[popuCount];
		for (int p = 0; p < popuCount; p++) {
			int cycleLength = g.calculateRoadLength(population[p]);
			oneLength[p] = std::make_pair(population[p], cycleLength);
		}

		//排序
		std::sort(oneLength, oneLength + popuCount, compareRoadWithRes);
		//刷新最优解
		bool isDis = saveBest(oneLength[0], cur, tsolution);
		if (isDis) {
			//发生灾变
			for (int p = 0; p < popuCount; p++) {
				delete[] population[p];
				population[p] = simpleRandom(g, engine);
			}
			continue;
		}
		//1:个体淘汰
		//计算累计概率；第一个保证存活，不参加计算
		long long totalLength = 0;
		long long* simuLength = new long long[popuCount];
		for (int p = 1; p < popuCount; p++) {
			totalLength += (oneLength[p].second-(oneLength[0].second*0/2));
			simuLength[p] = totalLength;
		}

		//根据累计概率，随机淘汰个体
		bool* dieMark = new bool[popuCount];
		memset(dieMark, false, sizeof(bool)*popuCount);
		int toDieCount = (1-aliveRate)*popuCount;
		int haveDeadCount = 0;

		//随机出被淘汰的个体
		std::uniform_int_distribution<long long> disDie(1, totalLength);
		while (haveDeadCount < toDieCount) {
			long long toDieIndex = disDie(engine);
			int i;
			for (i = 1; i < popuCount; i++) {
				if (toDieIndex <= simuLength[i])break;
			}
			if (!dieMark[i]) {
				dieMark[i] = true;
				haveDeadCount++;
			}
		}

		//未被淘汰的个体，允许进入下一代
		int curAliveCopy = 0;
		for (int i = 0; i < popuCount; i++) {
			if (!dieMark[i]) {
				population[curAliveCopy++] = oneLength[i].first;
			}
			else {
				delete[] oneLength[i].first;
			}
		}
		delete[] dieMark;
		delete[] simuLength;
		delete[] oneLength;


		//2：个体生产
		int toBorn = popuCount - curAliveCopy;
		std::uniform_int_distribution<int> disBorn(0, curAliveCopy - 1);
		for (int b = 0; b < toBorn; b++) {
			int parent1 = disBorn(engine);
			int parent2 = disBorn(engine);
			if (parent1 > parent2) {
				int tempSwap = parent1;
				parent1 = parent2;
				parent2 = tempSwap;
			}
			//从子代中，选择1-2条、长度不低于10%、不高于40%；且总长度不高于50%的片段作为保留片段。
			std::uniform_int_distribution<int> disPartLength(g.pointCount*0.1, g.pointCount*0.4);
			int partLength1 = disPartLength(engine);
			std::uniform_int_distribution<int> disPartLength2(std::min(g.pointCount*0.4, g.pointCount*0.5 - partLength1) - g.pointCount*0.4, std::min(g.pointCount*0.4, g.pointCount*0.5 - partLength1));
			int partLength2 = disPartLength2(engine);
			int* newC = new int[g.pointCount];
			population[b + curAliveCopy] = newC;
			bool* occupied = new bool[g.pointCount];
			memset(occupied, false, sizeof(bool)*g.pointCount);
			if (partLength2 > 0) {
				std::uniform_int_distribution<int> disPart1(0, g.pointCount / 2 - partLength1);
				int part1Begin = disPart1(engine);
				std::uniform_int_distribution<int> disPart2(part1Begin+partLength1, g.pointCount -partLength2);
				int part2Begin = disPart2(engine);
				memcpy(newC + part1Begin, population[parent2] + part1Begin, sizeof(int)*partLength1);
				memcpy(newC + part2Begin, population[parent2] + part2Begin, sizeof(int)*partLength2);
				memset(occupied + part1Begin, true, sizeof(bool)*partLength1);
				memset(occupied + part2Begin, true, sizeof(bool)*partLength2);
			}
			else {
				std::uniform_int_distribution<int> disPart1(0, g.pointCount - partLength1);
				int part1Begin = disPart1(engine);
				memcpy(newC + part1Begin, population[parent2] + part1Begin, sizeof(int)*partLength1);
				memset(occupied + part1Begin, true, sizeof(bool)*partLength1);
			}
			bool* used = new bool[g.pointCount];
			memset(used, false, sizeof(bool)*g.pointCount);
			for (int i = 0; i < g.pointCount; i++) {
				if (occupied[i]) {
					used[newC[i]] = true;
				}
			}
			int curNewPlace = 0;
			for (int i = 0; i < g.pointCount; i++) {
				if (!used[population[parent1][i]]) {
					while (occupied[curNewPlace]) {
						curNewPlace++;
					}
					newC[curNewPlace] = population[parent1][i];
					used[population[parent1][i]] = true;
					occupied[curNewPlace++] = true;
				}
			}
			delete[] used;
			delete[] occupied;
		}


		//3:个体变异
		//最优的解保持不变性
		std::uniform_real_distribution<double> disVari(0, 1);
		for (int v = 1; v < popuCount; v++) {
			double vv = disVari(engine);
			if (vv <= variProb) {
				vari(population[v], g.pointCount, engine, randomRate);
			}
		}
		//一轮迭代完成，继续循环
	}
	
	for (int p = 0; p < popuCount; p++) {
		delete[] population[p];
	}
	delete[] population;
}