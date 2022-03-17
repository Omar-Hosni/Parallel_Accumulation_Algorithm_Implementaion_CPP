#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <queue>
#include <exception>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>

#define MIN_BLOCK_SIZE 1000

template<typename iterator, typename T>
void accumulate_func(iterator start, iterator end, T& ref)
{
	//ref will be used as data transfer mechanism
	ref = std::accumulate(start, end, 0); // here the ref stores the accumulate value of start and end
}

template<typename iterator, typename T>
T parallel_accumulate(iterator start, iterator end, T& ref)
{
	int input_size = std::distance(start, end);

	//number of blocks allowed
	int allowed_threads_by_elements = (input_size) / MIN_BLOCK_SIZE;
	
	//allowed threads by machine
	int allowed_threads_by_hardware = std::thread::hardware_concurrency();

	//calculating num of threads will eleminate the problem of sponing large amount of threads in case we have large dataset
	int num_threads = std::min(allowed_threads_by_elements, allowed_threads_by_hardware);
	
	int block_size = (input_size + 1) / num_threads;

	std::vector<T> results(num_threads);
	std::vector<std::thread> threads(num_threads-1);

	iterator last;//last element of each data block
	
	for (size_t i = 0; i < num_threads - 1; i++)
	{
		last = start; //last element's initial place is at start
		std::advance(last, block_size); //we advance last by block size so it becomes the last element of the block

		//we initialize each thread by letting it accumulate from start to end and store it in results vector
		threads[i] = std::thread(accumulate_func<iterator, T>, start, last, std::ref(results[i]));

		//for the next iteration, start pointer should be the end pointer for current iteration
		start = last;
	}
		/*
		notice in the loop we are iterating only num_threads-1 
		as we are going to call accumulate on last block in the
		calling thread. so let's call accumulate on last data block in this thread 
		*/
		results[num_threads - 1] = std::accumulate(start, end, 0);
		
		//mem_fn (member function) generates wrapper objects for pointers to members
		std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join)); //for each thread call member function join
		
		//now that results vector has all final results of each block, then lets accumulate it all
		return std::accumulate(results.begin(), results.end(), ref);
}


int main()
{
	const int size = 8000;
	int* my_array = new int[size];
	int ref = 0;

	srand(0);

	for (size_t i = 0; i < size; i++)
	{
		my_array[i] = rand() % 10;
	}

	int rer_val = parallel_accumulate<int*, int>(my_array, my_array + size, ref);
	printf("Accumulated value : %d \n", rer_val);
}
