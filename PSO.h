#pragma once
#include <stdlib.h>
#include <iostream>
#include <vector>


// 适应度是越大越好还是越小越好
//#define MINIMIZE_FITNESS
#define MAXIMIZE_FITNESS

struct PSOPara
{
	int dim_;							// Parameter dimension
	int particle_num_;					// number of particles
	int max_iter_num_;					// number of iterations

	double *dt_ = nullptr;
	double *wstart_ = nullptr;
	double *wend_ = nullptr;
	double *C1_ = nullptr;
	double *C2_ = nullptr;

	double *upper_bound_ = nullptr;					// Upper search range
	double *lower_bound_ = nullptr;					// Lower search range
	double *range_interval_ = nullptr;				// Search interval length

	int results_dim_ = 0;

	PSOPara() {}

	PSOPara(int dim, bool hasBound = false)
	{
		dim_ = dim;

		dt_ = new double[dim_];
		wstart_ = new double[dim_];
		wend_ = new double[dim_];
		C1_ = new double[dim_];
		C2_ = new double[dim_];
		if (hasBound)
		{
			upper_bound_ = new double[dim_];
			lower_bound_ = new double[dim_];
			range_interval_ = new double[dim_];
		}
	}

	~PSOPara()
	{
		if (upper_bound_) { delete[]upper_bound_; }
		if (lower_bound_) { delete[]lower_bound_; }
		if (range_interval_) { delete[]range_interval_; }
		if (dt_) { delete[]dt_; }
		if (wstart_) { delete[]wstart_; }
		if (wend_) { delete[]wend_; }
		if (C1_) { delete[]C1_; }
		if (C2_) { delete[]C2_; }
	}
};

struct Particle
{
	int dim_;
	double fitness_;
	double *position_ = nullptr;
	double *velocity_ = nullptr;

	double *best_position_ = nullptr;
	double best_fitness_;
	double *results_ = nullptr;
	int results_dim_ = 0;

	Particle() {}

	~Particle()
	{
		if (position_) { delete[]position_; }
		if (velocity_) { delete[]velocity_; }
		if (best_position_) { delete[]best_position_; }
		if (results_) { delete[]results_; }
	}
};

typedef double(*ComputeFitness)(Particle& particle);

class PSOOptimizer
{
public:
	int particle_num_;
	int max_iter_num_;
	int curr_iter_;

	int dim_;

	Particle *particles_ = nullptr;

	double *upper_bound_ = nullptr;
	double *lower_bound_ = nullptr;
	double *range_interval_ = nullptr;

	double *dt_ = nullptr;
	double *wstart_ = nullptr;
	double *wend_ = nullptr;
	double *w_ = nullptr;
	double *C1_ = nullptr;
	double *C2_ = nullptr;

	double all_best_fitness_;
	double *all_best_position_ = nullptr;
	double *results_ = nullptr;
	int results_dim_ = 0;

	ComputeFitness fitness_fun_ = nullptr;

public:
	PSOOptimizer() {}

	PSOOptimizer(PSOPara* pso_para, ComputeFitness fitness_fun);

	~PSOOptimizer();

	void InitialAllParticles();

	void InitialParticle(int i);

	double GetDoubleRand(int N = 9999);

	double GetFitness(Particle& particle);

	void UpdateAllParticles();

	void UpdateParticle(int i);

	void GetInertialWeight();
};