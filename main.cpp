#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <set>
#include <numeric>
#include <string>
#include <cctype>
#include <iterator>
#include <cstring>
#include <random>
#include <cmath>
#include <functional> //std::mem_fn generates wrapper objects for pointers to members, which can store, copy, and invoke a pointer to member. 

#define MIN_BLOCK_SIZE 1000

using namespace std;

template<typename iterator, typename T>
void accumulate_func(iterator start, iterator end, T &ref) {
	ref = accumulate(start, end, 0);
}

template<typename iterator, typename T>
T parallel_accumulate(iterator start, iterator end, T& refs)
{
	int input_size = distance(start, end);
	int allowed_threads_by_elements = (input_size) / MIN_BLOCK_SIZE;
	int allowed_threads_by_hardware = thread::hardware_concurrency();
	int num_threads = min(allowed_threads_by_elements, allowed_threads_by_hardware);
	int block_size = (input_size + 1) / num_threads;

	vector<T> results(num_threads);
	vector<thread> threads(num_threads - 1);

	iterator last; //the last element of each data block

	for (int i = 0; i < num_threads - 1; ++i) {
		last = start;
		advance(last, block_size); //Increments given iterator *it by n elements. 
		threads[i] = thread(accumulate_func<iterator, T>, start, last, std::ref(results[i]));
		start = last;
	}

	results[num_threads - 1] = accumulate(start, end, 0);
	for_each(threads.begin(), threads.end(), mem_fn(&thread::join));
	
	return accumulate(results.begin(), results.end(), refs);
}


int main() 


	const int size = 8000;
	int* my_array = new int[size];
	int ref = 0;

	srand(0);

	for (int i = 0; i < size; ++i) {
		my_array[i] = rand() % 100;
	}

	int rer_val = parallel_accumulate<int*, int>(my_array, my_array + size, ref);
	cout << "Accumulated value: " << rer_val << endl;
}
