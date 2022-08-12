#pragma once
#include "PSO.h"
#include <math.h>
#include <fstream>
#include <string>
#include <iostream>
using namespace std;
#define PI 3.1415926



double FitnessFunction(Particle& particle);

double FitnessFunction(Particle& particle)
{
	extern double Temp[100];
	extern int parameterNum;
	extern double CalculationError();
	for (int i = 0; i < parameterNum; i++)
		Temp[i] = particle.position_[i];
	}

	extern double zhuanghao[100]; extern double section[400][2][100]; extern int num_dui[100]; extern double cl[100];
	extern int irrNum; extern int sectionNum[100];
	extern int sum_section;//断面数
	extern string sectionShape[100], sectionName[100];
	extern double* getCo(double width, double bottom, double x1, double y1, double x2, double y2);

	for (int i = 0; i < parameterNum / 2; i++)
	{
		double* sectionTemp = getCo(100, Temp[i * 2],
			section[irrNum - 2][0][sectionNum[i] - 1], section[irrNum - 2][1][sectionNum[i] - 1],
			section[irrNum + 1][0][sectionNum[i] - 1], section[irrNum + 1][1][sectionNum[i] - 1]);//需要改变的两个点坐标
		section[irrNum - 1][0][sectionNum[i] - 1] = sectionTemp[0]; section[irrNum - 1][1][sectionNum[i] - 1] = sectionTemp[1];
		section[irrNum][0][sectionNum[i] - 1] = sectionTemp[2]; section[irrNum][1][sectionNum[i] - 1] = sectionTemp[3];
	}

	ofstream output;
	output.open("河道地形.txt");
	output << "河道地形" << endl;
	output << sum_section << endl;
	for (int i = 0; i < sum_section; i++)
	{
		output << i + 1 << '\t' << sectionName[i] << '\t' << zhuanghao[i] << '\t' << Temp[1] << endl;
		output << sectionShape[i] << '\t';
		if (sectionShape[i] == "不规则形")
		{
			output << "1" << '\t' << num_dui[i] << '\t';
			for (int j = 0; j < num_dui[i]; j++)
			{
				output << section[j][0][i] << '\t' << section[j][1][i] << '\t';
			}
			output << endl;
		}
		else if (sectionShape[i] == "梯形")
		{
			output << "1" << '\t';
			for (int j = 0; j < 4; j++)
			{
				output << section[j][0][i] << '\t';
			}
			output << endl;
		}
	}
	output.close();

	double fitness = CalculationError();//Run the hydrodynamic model and calculate the average error of hydrodynamic simulation

	return fitness;
}
