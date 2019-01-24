#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>
#include <ctime>
#include <thread>
#include <cstring>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

using gen_t = char;
static constexpr const gen_t* alphabet = "abcdefghijklmnopqrstuvwxyz #!?()";
static constexpr const gen_t* goal_dna = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
static const size_t dna_size = strlen(goal_dna); // number of gens per dna
static const size_t pop_size = 512; // number of gens per population

using dna_t = gen_t[dna_size + 1];
using pop_t = dna_t[pop_size];
using fitness_t = size_t;
using pop_fitness_t = fitness_t[pop_size];
using pop_fitness_t = fitness_t[pop_size];

// static const dna_t goal_dna = "aaaaaaaaaaaaaaa";
// static const dna_t goal_dna = {'a', 'a', 'a', 'a', 'a', '\0'};
static const float mutation_rate = 0.01f;
static const size_t fitness_factor = 1;
static const fitness_t goal_fitness = dna_size * fitness_factor;

void print_pop_fitness(const pop_fitness_t& pop_fitness)
{
	for (size_t i = 0; i < pop_size; i++) {
		auto fitness = pop_fitness[i] / fitness_factor;
		if (fitness == 0)
			cout << " ";
		else if (fitness == goal_fitness / fitness_factor)
			cout << "!";
		else
			cout << (fitness * 10 / dna_size);
	}
	cout << endl;
}

size_t index_of_best_gen(const pop_fitness_t& pop_fitness)
{
	size_t index = 0;
	for (size_t i = 0; i < pop_size; i++)
		if (pop_fitness[index] < pop_fitness[i])
			index = i;
	return index;
}

float rand_float()
{
	return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

char gen_char()
{
	return alphabet[rand() % strlen(alphabet)];
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
			fitness += fitness_factor;
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

// void crossover_dna(const dna_t& left, const dna_t& right, dna_t* out)
// {
// 	for (size_t i = 0; i < dna_size; i++)
// 		(*out)[i] = rand() % 2 == 0 ? left[i] : right[i];
// 	(*out)[dna_size] = '\0';
// }

void crossover_dna(const dna_t& left, const dna_t& right, dna_t* out)
{
	auto split_index = rand() % dna_size;
	for (size_t i = 0; i < split_index; i++)
		(*out)[i] = left[i];
	for (size_t i = split_index; i < dna_size; i++)
		(*out)[i] = right[i];
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

	bool front_pop_index;
	pop_t pops[2];
	pop_t* front_pop;
	pop_t* back_pop;
	auto swap_pops = [&] () {
		front_pop_index = !front_pop_index;
		front_pop = &pops[front_pop_index];
		back_pop = &pops[!front_pop_index];
	};
	swap_pops();
	init_pop(front_pop);
	init_pop(back_pop); // unnecessary

	pop_fitness_t pop_fitness;

	size_t backlog = 1;
	size_t count = 0;
	size_t best;
	size_t first_found = 0;
	fitness_t sum_fitness;

	using clock = std::chrono::steady_clock;
	auto elapsed = 0ms;
	auto elapsed_found = 0ms;

	while (true) {
		auto then = clock::now();

		for (size_t b = 0; b < backlog; b++) {
			eval_pop_fitness(*front_pop, &pop_fitness);
			sum_fitness = eval_sum_fitness(pop_fitness);
			best = index_of_best_gen(pop_fitness);

			if (pop_fitness[best] == dna_size * fitness_factor) {
				cout << (*front_pop)[best] << " " << count << " :)" << endl;
				if (first_found == 0) {
					first_found = count;
					elapsed_found = duration_cast<milliseconds>(clock::now() - then + elapsed);
				}
			} else {
				cout << (*front_pop)[best] << " " << count << endl;
			}

			// for (size_t i = 0; i < pop_size; i++) {
			// 	size_t parent_indices[2];
			// 	size_t c = 0;
      //
			// 	for (size_t j = 0; j < 2; j++) {
			// 		fitness_t sum = 0;
			// 		fitness_t threshold = rand() % (sum_fitness + 1);
      //
			// 		for (size_t k = 0; k < pop_size; k++) {
			// 			sum += pop_fitness[k];
      //
			// 			if (threshold <= sum) {
			// 				parent_indices[j] = k;
			// 				c++;
			// 				break;
			// 			}
			// 		}
			// 	}
      //
			// 	assert(c == 2);
			// 	crossover_dna((*front_pop)[parent_indices[0]], (*front_pop)[parent_indices[1]], &(*back_pop)[i]);
			// }
			
			for (size_t i = 0; i < pop_size / 2; i++) {
				size_t a = rand() % pop_size;
				size_t b = rand() % pop_size;
				size_t c = rand() % pop_size;
				size_t d = rand() % pop_size;

				size_t x = pop_fitness[a] < pop_fitness[b] ? b : a;
				size_t y = pop_fitness[c] < pop_fitness[d] ? d : c;

			 	crossover_dna((*front_pop)[x], (*front_pop)[y], &(*back_pop)[i*2]);
			 	crossover_dna((*front_pop)[x], (*front_pop)[y], &(*back_pop)[i*2+1]);
			}

			mutate_pop(back_pop);
			memcpy((*back_pop) + (rand() % pop_size), *front_pop + best, dna_size * sizeof(gen_t));
			swap_pops();
			count++;
		}

		elapsed += duration_cast<milliseconds>(clock::now() - then);

		print_pop_fitness(pop_fitness);
		cout << "elapsed: " << elapsed.count() << "ms" << endl;
		if (first_found != 0) {
			cout << "first found: " << first_found << endl;
			cout << "elapsed found: " << elapsed_found.count() << "ms" << endl;
		}
		cin >> backlog;
	}
}
