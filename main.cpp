#include <iostream>

#include "PSO.h"
#include "Fitness.h"
#include<fstream>
#include<string>
#include<iomanip>
#include <ctime>
using namespace std;

double CalculationError();

double CalculationResult[10000];
int o_dt;
int iterationNum, particleNum, parameterNum = 0, duanmian;
int sectionNum[100]; double limit[100][2];
int dtCalculation, dtCompare, SectionSerial, nRow;
string strType;
double StageCompare[10000];
double Temp[100];

double zhuanghao[100]; double section[400][2][100]; int num_dui[100]; double cl[100];
int sum_section;
int irrNum;
string sectionShape[100], sectionName[100];

double* getCo(double width, double bottom, double x1, double y1, double x2, double y2)//计算坐标
{
	double* array = new double[4];
	double distance = (x2 - x1 - width)*((y1 - bottom) / (y1 - bottom + y2 - bottom));//梯形底面与第一个点的水平距离
	array[0] = x1 + distance; array[1] = bottom;
	array[2] = array[0] + width; array[3] = bottom;
	return array;
	delete[]array;
}

int main()
{
	string temp;

	//先读取
	ifstream input("河道地形.txt");
	input >> temp; 
	input >> sum_section;
	for (int i = 0; i < sum_section; i++)
	{
		input >> temp >> sectionName[i] >> zhuanghao[i] >> cl[i];
		input >> sectionShape[i] >> temp;
		if (sectionShape[i] == "不规则形")
		{
			input>> num_dui[i];
			for (int j = 0; j < num_dui[i]; j++)
			{
				input >> section[j][0][i] >> section[j][1][i];
			}
		}
		else if (sectionShape[i] == "梯形")
		{
			for (int j = 0; j < 4; j++)
			{
				input >> section[j][0][i];
			}
		}
	}
	input.close();
	
	input.open("优化条件.txt");
	input >> particleNum >> iterationNum;
	input >> duanmian;
	for (int i = 0; i < duanmian; i++)
	{
		input >> sectionNum[i];
	}
	for (int i = 0; i < duanmian; i++)
	{
		if (sectionShape[sectionNum[i]] == "矩形")
		{
			parameterNum += 2;
		}
		else if (sectionShape[sectionNum[i]] == "梯形")
		{
			parameterNum += 3;
		}
		else if (sectionShape[sectionNum[i]] == "圆形")
		{
			parameterNum += 2;
		}
		else if (sectionShape[sectionNum[i]] == "不规则形")
		{
			parameterNum += 2;
		}
	}
	input >> irrNum;
	for (int i = 0; i < parameterNum; i++)
	{
		input >> limit[i][0] >> limit[i][1];
	}
	input.close();

	input.open("优化序列.txt");
	input>> dtCalculation >> dtCompare;
	input >> temp >> strType >> nRow;
	string time1, time2;
	for (int i = 0; i < nRow; i++)
	{
		input >> time1 >> time2 >> StageCompare[i];
	}
	input.close();

	PSOPara psopara(parameterNum, true);
	psopara.particle_num_ = particleNum;
	psopara.max_iter_num_ = iterationNum;
	for (int i = 0; i < parameterNum; i++)
	{
		psopara.dt_[i] = 1.0;
		psopara.wstart_[i] = 0.9;
		psopara.wend_[i] = 0.4;	
		psopara.C1_[i] = 1.49445;
		psopara.C2_[i] = 1.49445;
		psopara.lower_bound_[i] = limit[i][0];
		psopara.upper_bound_[i] = limit[i][1];
	}
	
	
	PSOOptimizer psooptimizer(&psopara, FitnessFunction);

	std::srand((unsigned int)time(0));
	psooptimizer.InitialAllParticles();
	double fitness = psooptimizer.all_best_fitness_;
	double *result = new double[psooptimizer.max_iter_num_];

	ofstream output;
	output.open("result.txt");
	for (int i = 0; i < psooptimizer.max_iter_num_; i++)
	{
		psooptimizer.UpdateAllParticles();
		result[i] = psooptimizer.all_best_fitness_;
		output << "第" << i + 1 << "次迭代结果：" << endl;
		for (int i = 0; i < parameterNum; i++)
		{
			output << "第" << i + 1 << "个粒子为" << psooptimizer.all_best_position_[i] << ",";
			if (int(i + 1) % 3 == 0)
			{
				output << endl;
			}
		}
		output << "fitness = " << result[i] << std::endl << std::endl;
	}
	output.close();

	for (int i = 0; i < parameterNum / 2; i++)
	{
		double* sectionTemp = getCo(50, psooptimizer.all_best_position_[i * 2],
			section[irrNum - 2][0][sectionNum[i] - 1], section[irrNum - 1][1][sectionNum[i] - 1],
			section[irrNum + 1][0][sectionNum[i] - 1], section[irrNum + 1][1][sectionNum[i] - 1]);//需要改变的两个点坐标
		section[irrNum - 1][0][sectionNum[i] - 1] = sectionTemp[0]; section[irrNum - 1][1][sectionNum[i] - 1] = sectionTemp[1];
		section[irrNum][0][sectionNum[i] - 1] = sectionTemp[2]; section[irrNum][1][sectionNum[i] - 1] = sectionTemp[3];
	}

	output.open("河道地形.txt");
	output << "河道地形" << endl;
	output << sum_section << endl;
	for (int i = 0; i < sum_section; i++)
	{
		output << i + 1 << '\t' << sectionName[i] << '\t' << zhuanghao[i] << '\t' << psooptimizer.all_best_position_[i * 2 + 1] << endl;
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

	CalculationError();

	return 0;
	//system("pause");
}