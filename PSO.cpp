#include "PSO.h"

PSOOptimizer::PSOOptimizer(PSOPara* pso_para, ComputeFitness fitness_fun)
{
	particle_num_ = pso_para->particle_num_;
	max_iter_num_ = pso_para->max_iter_num_;
	dim_ = pso_para->dim_;
	curr_iter_ = 0;

	dt_ = new double[dim_];
	wstart_ = new double[dim_];
	wend_ = new double[dim_];
	C1_ = new double[dim_];
	C2_ = new double[dim_];

	for (int i = 0; i < dim_; i++)
	{
		dt_[i] = pso_para->dt_[i];
		wstart_[i] = pso_para->wstart_[i];
		wend_[i] = pso_para->wend_[i];
		C1_[i] = pso_para->C1_[i];
		C2_[i] = pso_para->C2_[i];
	}

	if (pso_para->upper_bound_ && pso_para->lower_bound_)
	{
		upper_bound_ = new double[dim_];
		lower_bound_ = new double[dim_];
		range_interval_ = new double[dim_];

		for (int i = 0; i < dim_; i++)
		{
			upper_bound_[i] = pso_para->upper_bound_[i];
			lower_bound_[i] = pso_para->lower_bound_[i];
			range_interval_[i] = upper_bound_[i] - lower_bound_[i];
		}
	}

	particles_ = new Particle[particle_num_];
	w_ = new double[dim_];
	all_best_position_ = new double[dim_];

	results_dim_ = pso_para->results_dim_;

	if (results_dim_)
	{
		results_ = new double[results_dim_];
	}

	fitness_fun_ = fitness_fun;
}

PSOOptimizer::~PSOOptimizer()
{
	if (particles_) { delete[]particles_; }
	if (upper_bound_) { delete[]upper_bound_; }
	if (lower_bound_) { delete[]lower_bound_; }
	if (range_interval_) { delete[]range_interval_; }
	if (dt_) { delete[]dt_; }
	if (wstart_) { delete[]wstart_; }
	if (wend_) { delete[]wend_; }
	if (w_) { delete[]w_; }
	if (C1_) { delete[]C1_; }
	if (C2_) { delete[]C2_; }
	if (all_best_position_) { delete[]all_best_position_; }
	if (results_) { delete[]results_; }
}

void PSOOptimizer::InitialAllParticles()
{
	// Initializes the first particle parameter and sets the optimal value
	InitialParticle(0);
	all_best_fitness_ = particles_[0].best_fitness_;
	for (int j = 0; j < dim_; j++)
	{
		all_best_position_[j] = particles_[0].best_position_[j];
	}

	// Initialize the other particles and update the optimal value
	for (int i = 1; i < particle_num_; i++)
	{
		InitialParticle(i);
#ifdef MAXIMIZE_FITNESS
		if (particles_[i].best_fitness_ < all_best_fitness_&&particles_[i].best_fitness_ > 0)
#else
		if (particles_[i].best_fitness_ > all_best_fitness_&&particles_[i].best_fitness_>0)
#endif
		{
			all_best_fitness_ = particles_[i].best_fitness_;
			for (int j = 0; j < dim_; j++)
			{
				all_best_position_[j] = particles_[i].best_position_[j];
			}

			if (particles_[i].results_dim_ && results_dim_ == particles_[i].results_dim_)
			{
				for (int k = 0; k < results_dim_; k++)
				{
					results_[k] = particles_[i].results_[k];
				}
			}
			else if (results_dim_)
			{
				std::cout << "WARNING: the dimension of your saved results for every particle\nis not match with the dimension you specified for PSO optimizer ant no result is saved!" << std::endl;
			}
		}
	}
}

double PSOOptimizer::GetDoubleRand(int N)
{
	double temp = rand() % (N + 1) / (double)(N + 1);
	return temp;
}

double PSOOptimizer::GetFitness(Particle & particle)
{
	return fitness_fun_(particle);
}

void PSOOptimizer::UpdateAllParticles()
{
	GetInertialWeight();
	for (int i = 0; i < particle_num_; i++)
	{
		UpdateParticle(i);
#ifdef MAXIMIZE_FITNESS
		if (particles_[i].best_fitness_ < all_best_fitness_&&particles_[i].best_fitness_ > 0)
#else
		if (particles_[i].best_fitness_ > all_best_fitness_)
#endif
		{
			all_best_fitness_ = particles_[i].best_fitness_;
			for (int j = 0; j < dim_; j++)
			{
				all_best_position_[j] = particles_[i].best_position_[j];
			}

			if (particles_[i].results_dim_ && results_dim_ == particles_[i].results_dim_)
			{
				for (int k = 0; k < results_dim_; k++)
				{
					results_[k] = particles_[i].results_[k];
				}
			}
			else if (results_dim_)
			{
				std::cout << "WARNING: the dimension of your saved results for every particle\nis not match with the dimension you specified for PSO optimizer ant no result is saved!" << std::endl;
			}
		}
	}
	curr_iter_++;
}

void PSOOptimizer::UpdateParticle(int i)
{
	// Computes the weight of the current iteration
	for (int j = 0; j < dim_; j++)
	{
		double last_position = particles_[i].position_[j];
		//Speed and position
		particles_[i].velocity_[j] = w_[j] * particles_[i].velocity_[j] +
			C1_[j] * GetDoubleRand() * (particles_[i].best_position_[j] - particles_[i].position_[j]) +
			C2_[j] * GetDoubleRand() * (all_best_position_[j] - particles_[i].position_[j]);
		particles_[i].position_[j] += dt_[j] * particles_[i].velocity_[j];

		if (upper_bound_ && lower_bound_)
		{
			if (particles_[i].position_[j] > upper_bound_[j])
			{
				double thre = GetDoubleRand(99);
				if (last_position == upper_bound_[j])
				{
					particles_[i].position_[j] = GetDoubleRand() * range_interval_[j] + lower_bound_[j];
				}
				else if (thre < 0.5)
				{
					particles_[i].position_[j] = upper_bound_[j] - (upper_bound_[j] - last_position) * GetDoubleRand();
				}
				else
				{
					particles_[i].position_[j] = upper_bound_[j];
				}
			}
			if (particles_[i].position_[j] < lower_bound_[j])
			{
				double thre = GetDoubleRand(99);
				if (last_position == lower_bound_[j])
				{
					particles_[i].position_[j] = GetDoubleRand() * range_interval_[j] + lower_bound_[j];
				}
				else if (thre < 0.5)
				{
					particles_[i].position_[j] = lower_bound_[j] + (last_position - lower_bound_[j]) * GetDoubleRand();
				}
				else
				{
					particles_[i].position_[j] = lower_bound_[j];
				}
			}
		}
	}
	particles_[i].fitness_ = GetFitness(particles_[i]);

#ifdef MAXIMIZE_FITNESS
	if (particles_[i].fitness_ < particles_[i].best_fitness_&&particles_[i].fitness_ > 0)
#else
	if (particles_[i].fitness_ > particles_[i].best_fitness_)
#endif
	{
		particles_[i].best_fitness_ = particles_[i].fitness_;
		for (int j = 0; j < dim_; j++)
		{
			particles_[i].best_position_[j] = particles_[i].position_[j];
		}
	}
}


void PSOOptimizer::GetInertialWeight()
{
	double temp = curr_iter_ / (double)max_iter_num_;
	temp *= temp;
	for (int i = 0; i < dim_; i++)
	{
		w_[i] = wstart_[i] - (wstart_[i] - wend_[i]) * temp;
	}
}


void PSOOptimizer::InitialParticle(int i)
{
	particles_[i].position_ = new double[dim_];
	particles_[i].velocity_ = new double[dim_];
	particles_[i].best_position_ = new double[dim_];

	//if (results_dim_)
	//{
	//	particles_[i].results_ = new double[results_dim_];
	//}

	for (int j = 0; j < dim_; j++)
	{
		if (range_interval_)
		{
			particles_[i].position_[j] = GetDoubleRand() * range_interval_[j] + lower_bound_[j];
			particles_[i].velocity_[j] = GetDoubleRand() * range_interval_[j] / 300;
		}
		else
		{
			particles_[i].position_[j] = GetDoubleRand() * 2;
			particles_[i].velocity_[j] = GetDoubleRand() * 0.5;
		}
	}

	particles_[i].fitness_ = GetFitness(particles_[i]);

	for (int j = 0; j < dim_; j++)
	{
		particles_[i].best_position_[j] = particles_[i].position_[j];
	}
	particles_[i].best_fitness_ = particles_[i].fitness_;
}