#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>
#include <ctime>
#include <thread>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

using byte_t = unsigned char;
using gen_t = char;
static const size_t dna_size = 15; // number of gens per dna
static const size_t pop_size = 1024; // number of gens per population

using dna_t = gen_t[dna_size + 1];
using pop_t = dna_t[pop_size];
using fitness_t = size_t;
using pop_fitness_t = fitness_t[pop_size];
using pop_fitness_t = fitness_t[pop_size];

// static const dna_t goal_dna = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
static const dna_t goal_dna = "xxxxxxxxxxxxxxx";
static const float mutation_rate = 0.01f;

float rand_float()
{
	return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

char gen_char()
{
	return rand() % ('z' - 'a') + 'a';
}

void init_dna(dna_t* dna)
{
	for (size_t i = 0; i < dna_size; i++)
		(*dna)[i] = gen_char();
	(*dna)[dna_size] = '\0';
}

void init_pop(pop_t* pop)
{
	for (size_t i = 0; i < pop_size; i++)
		init_dna(&(*pop)[i]);
}

fitness_t eval_fitness(const dna_t& dna)
{
	fitness_t fitness = 0;
	for (size_t i = 0; i < dna_size; i++)
		if (dna[i] == goal_dna[i])
			fitness++;
	return fitness;
}

void eval_pop_fitness(const pop_t& pop, pop_fitness_t* pop_fitness)
{
	for (size_t i = 0; i < pop_size; i++)
		(*pop_fitness)[i] = eval_fitness(pop[i]);
}

fitness_t eval_sum_fitness(const pop_fitness_t& pop_fitness)
{
	fitness_t sum = 0;
	for (size_t i = 0; i < pop_size; i++)
		sum += pop_fitness[i];
	return sum;
}

size_t eval_max_fitness_index(const pop_fitness_t& pop_fitness)
{
	size_t max_index = 0;
	for (size_t i = 0; i < pop_size; i++)
		if (pop_fitness[max_index] < pop_fitness[i])
			max_index = i;
	return max_index;
}

void crossover_dna(const dna_t& left, const dna_t& right, dna_t* out)
{
	for (size_t i = 0; i < dna_size; i++)
		(*out)[i] = (rand() % 2) == 0 ? left[i] : right[i];
	(*out)[dna_size] = '\0';
}

void mutate_dna(dna_t* dna)
{
	for (size_t i = 0; i < dna_size; i++)
		if (rand_float() < mutation_rate)
			(*dna)[i] = gen_char();
}

void mutate_pop(pop_t* pop)
{
	for (size_t i = 0; i < pop_size; i++)
		mutate_dna(&(*pop)[i]);
}

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;
	std::srand(std::time(0));

	bool cur_pop = false;
	pop_t pop[2];
	init_pop(&pop[cur_pop]);

	pop_fitness_t pop_fitness;

	while (true) {
		eval_pop_fitness(pop[cur_pop], &pop_fitness);
		auto max_fitness_index = eval_max_fitness_index(pop_fitness);

		// for (size_t i = 0; i < dna_size; i++)
		// 	std::cout << pop[cur_pop][max_fitness_index][i];
		// std::cout << " " << pop_fitness[max_fitness_index] << std::endl;
		std::cout << pop[cur_pop][max_fitness_index] << " " << pop_fitness[max_fitness_index] << std::endl;

		auto sum_fitness = eval_sum_fitness(pop_fitness);

		for (size_t i = 0; i < pop_size; i++) {
			size_t parent_indices[2];

			for (size_t j = 0; j < 2; j++) {
				fitness_t sum = 0;
				fitness_t threshold = rand() % (sum_fitness + 1);

				for (size_t k = 0; k < pop_size; k++) {
					sum += pop_fitness[i];

					if (threshold <= sum) {
						parent_indices[j] = k;
						break;
					}
				}
			}

			crossover_dna(pop[cur_pop][parent_indices[0]], pop[cur_pop][parent_indices[1]], &pop[!cur_pop][i]);
		}

		cur_pop = !cur_pop;
		mutate_pop(&pop[cur_pop]);
	}
}
