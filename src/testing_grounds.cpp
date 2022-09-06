#include "tracked_array.hpp"
#include "binary_search_tree.hpp"
#include "avl_tree.hpp"

#include <iostream>
#include <chrono>
#include <cstdint>
#include <vector>
#include <ctime>

#define TIMER_START {auto _TStartTime = std::chrono::high_resolution_clock::now();
#define TIMER_END(timerName) auto _TCurrentTime = std::chrono::high_resolution_clock::now(); std::cerr << "[" << timerName << "]\nRan for: " << (_TCurrentTime - _TStartTime) << " \n\n";}

#define HEADLESS_ITERATE_TIMER_START(_ITNumberOfIterations) { unsigned int _ITIterationCount = _ITNumberOfIterations; unsigned long long _ITTotalTime = 0; for (size_t _ITCount = 0; _ITCount < _ITIterationCount; _ITCount++) { auto _ITStartTime = std::chrono::high_resolution_clock::now();
#define HEADLESS_ITERATE_TIMER_END(_ITTimerName) auto _ITCurrentTime = std::chrono::high_resolution_clock::now(); _ITTotalTime += (_ITCurrentTime - _ITStartTime).count();} std::cerr << "[" << _ITTimerName << "]\nRan for an average of: " << (_ITTotalTime / _ITIterationCount) << "ns\n\n";}

#define ITERATE_TIMER_START(_ITNumberOfIterations) { unsigned int _ITIterationCount = _ITNumberOfIterations; unsigned long long _ITTotalTime = 0; for (size_t _ITCount = 0; _ITCount < _ITIterationCount; _ITCount++) { 
#define ITERATE_TIMER_HEADER_END auto _ITStartTime = std::chrono::high_resolution_clock::now();
#define ITERATE_TIMER_END(_ITTimerName) auto _ITCurrentTime = std::chrono::high_resolution_clock::now(); _ITTotalTime += (_ITCurrentTime - _ITStartTime).count();} std::cerr << "[" << _ITTimerName << "]\nRan for an average of: " << (_ITTotalTime / _ITIterationCount) << "ns\n\n";}

#define LOG(msg) std::cerr << msg << "\n";

//#define TRACE // Define TRACE to enable Tracer console output.
class Dummy {
public:
	Dummy(const Dummy& other)
		: order(count_++)
		, array(new int[10]) {
		for (size_t i = 0; i < 10; i++)
			this->array[i] = other.array[i];
	}
	Dummy& operator=(const Dummy& other) {
		if (this != &other) {
			this->order = other.order;
			for (size_t i = 0; i < 10; i++)
				array[i] = other.array[i];
			return *this;
		}
	}
	Dummy(Dummy&& other) noexcept
		: order(count_++)
		, array(other.array) {
		other.array = nullptr;
	}
	Dummy& operator=(Dummy&& other) noexcept {
		if (this != &other) {
			this->order = other.order;
			delete[] this->array;
			this->array = other.array;
			other.array = nullptr;
			return *this;
		}
	}
	~Dummy() {
		delete[] array;
	}

	Dummy(int order_)
		: order(order_)
		, array(new int[10]) {
		count_++;
	}
	Dummy()
		: order(count_++)
		, array(new int[10]) {
	}

	int order;
	int* array;
private:
	inline static int count_ = 0;
};
class Tracer {
public:
	Tracer(const Tracer& other) 
		: order(other.order) 
		, array(new int[10])
		, dummy(new Dummy(*(other.dummy))) {
#ifdef TRACE
		std::cout << "Copy Constructed [" << other.order << "]\n";
#endif // TRACE
		for (size_t i = 0; i < 5; i++)
			this->array[i] = other.array[i];
	}
	Tracer& operator=(const Tracer& other) {
#ifdef TRACE
		std::cout << "Copy Assigned [" << other.order << "]\n";
#endif // TRACE
		this->order = other.order;
		for (size_t i = 0; i < 5; i++)
			array[i] = other.array[i];
		*(this->dummy) = *(other.dummy);
		return *this;
	}
	Tracer(Tracer&& other) noexcept
		: order(count_++)
		, array(other.array) 
		, dummy(other.dummy) {
		other.array = nullptr;
		other.dummy = nullptr;
#ifdef TRACE
		std::cout << "Move Constructed [" << other.order << "]\n";
#endif // TRACE
	}
	Tracer& operator=(Tracer&& other) noexcept {
#ifdef TRACE
		std::cout << "Move Assigned [" << other.order << "]\n";
#endif // TRACE
		if (this != &other) {
			this->order = other.order;
			delete[] this->array;
			delete this->dummy;
			this->array = other.array;
			this->dummy = other.dummy;
			other.array = nullptr;
			other.dummy = nullptr;
			return *this;
		}
	}
	~Tracer() {
		delete[] array;
		delete dummy;
#ifdef TRACE
		std::cout << "Destroyed [" << this->order << "]\n";
#endif // TRACE
	}

	Tracer(int order_)
		: order(order_)
		, array(new int[10])
		, dummy(new Dummy()) {
		count_++;
#ifdef TRACE
		std::cout << "Constructed [" << this->order << "]\n";
#endif // TRACE
	}
	Tracer()
		: order(count_++) 
		, array(new int[10])
		, dummy(new Dummy()) {
		count_++;
#ifdef TRACE
		std::cout << "Default Constructed [" << this->order << "]\n";
#endif // !TRACE
	}

	int order;
	int* array;
	Dummy* dummy;
private:
	inline static int count_ = 0;
};


namespace AVLUtilities {
	size_t* GetRandomizedArrayOfSize(size_t size) {
		size_t* array = new size_t[size];
		for (size_t i = 0; i < size; i++) {
			array[i] = i;
		}
		for (size_t i = 0; i < size; i++) {
			const size_t firstIndex = rand() % size;
			size_t first = array[firstIndex];
			size_t secondIndex = rand() % size;
			while (secondIndex == firstIndex) {
				secondIndex = rand() % size;
			}
			size_t second = array[secondIndex];

			array[firstIndex] = second;
			array[secondIndex] = first;
		}
		return array;
	}

	avl_tree<int, Tracer> RandomlyCreatePerfectTreeOfHeight(size_t treeHeight) {
		avl_tree<int, Tracer> avl;
		const size_t numberOfElements = (1 << treeHeight) - 1;
		const size_t* indices = GetRandomizedArrayOfSize(numberOfElements);

		for (size_t i = 0; i < numberOfElements; i++) {
			avl.emplace(indices[i], indices[i]);
		}

		delete[] indices;
		return avl;
	}
	void RandomlyClearPerfectTreeOfHeight(avl_tree<int, Tracer>& avl, size_t treeHeight) {
		const size_t numberOfElements = (1 << treeHeight) - 1;
		const size_t* indices = GetRandomizedArrayOfSize(numberOfElements);

		for (size_t i = 0; i < numberOfElements; i++) {
			avl.remove(indices[i]);
		}

		delete[] indices;
	}
	
	avl_tree<int, Tracer> CreateRandomTreeOfSize(size_t size) {
		avl_tree<int, Tracer> avl;
		const size_t* indices = GetRandomizedArrayOfSize(size);

		for (size_t i = 0; i < size; i++) {
			avl.emplace(indices[i], indices[i]);
		}

		delete[] indices;
		return avl;
	}
	void RandomlyClearTreeOfSize(avl_tree<int, Tracer>& avl, size_t size) {
		const size_t* indices = GetRandomizedArrayOfSize(size);

		for (size_t i = 0; i < size; i++) {
			avl.remove(indices[i]);
		}

		delete[] indices;
	}
}
namespace BSTUtilities{
	namespace {
		void PushLevel(binary_search_tree<int, Tracer>&bst, size_t treeHeight, size_t level) {
			uint32_t numberOfElementsInLevel = 1 << (treeHeight - level);;
			uint32_t firstElementsKey = 1 << (level - 1);
			uint32_t increment = 1 << level;
			for (uint32_t i = 0; i < numberOfElementsInLevel; i++)
			{
				uint32_t key = firstElementsKey + (increment * i);
				bst.emplace(key - 1, key - 1);
#ifdef TRACE
				std::cerr << ">>>Emplaced " << key << "\n";
#endif // !TRACE
			}
		}
	}
	binary_search_tree<int, Tracer> CreatePerfectTreeOfHeight(size_t treeHeight) {
		binary_search_tree<int, Tracer> bst;
		for (size_t i = treeHeight; i > 0; i--)
		{
			PushLevel(bst, treeHeight, i);
		}
		return bst;
	}
	void RandomlyClearPerfectTreeOfHeight(binary_search_tree<int, Tracer>& bst, size_t treeHeight) {
		size_t numberOfElements = (1 << treeHeight) - 1;
		const size_t* indices = AVLUtilities::GetRandomizedArrayOfSize(numberOfElements);

		for (size_t i = 0; i < numberOfElements; i++) {
			bst.remove(indices[i]);
		}

		delete[] indices;
	}

	binary_search_tree<int, Tracer> CreateRandomTreeOfSize(size_t size) {
		binary_search_tree<int, Tracer> bst;
		const size_t* indices = AVLUtilities::GetRandomizedArrayOfSize(size);

		for (size_t i = 0; i < size; i++) {
			bst.emplace(indices[i], indices[i]);
		}

		delete[] indices;
		return bst;
	}
	void RandomlyClearTreeOfSize(binary_search_tree<int, Tracer>& bst, size_t size) {
		const size_t* indices = AVLUtilities::GetRandomizedArrayOfSize(size);

		for (size_t i = 0; i < size; i++) {
			bst.remove(indices[i]);
		}

		delete[] indices;
	}
}

constexpr size_t size = 10000;

int main(int argc, char** args) {
	srand(std::time(NULL));

	//Leak Tests
	{
		size_t iter = 100;
		size_t size = 1000;

		HEADLESS_ITERATE_TIMER_START(iter)
			binary_search_tree<int, Tracer> bst = BSTUtilities::CreateRandomTreeOfSize(size);
			BSTUtilities::RandomlyClearTreeOfSize(bst, size);
		HEADLESS_ITERATE_TIMER_END("BST Leak Test: Randomly Create then Clear Tree of Size " << size)

		HEADLESS_ITERATE_TIMER_START(iter)
			avl_tree<int, Tracer> avl = AVLUtilities::CreateRandomTreeOfSize(size);
			AVLUtilities::RandomlyClearTreeOfSize(avl, size);
		HEADLESS_ITERATE_TIMER_END("AVL Leak Test: Randomly Create then Clear Tree of Size " << size)
	}

	//Operation Time Complexity Tests
	{
		size_t iter = 1000;
		size_t size = 25000;

		const size_t* randomKeyArray = AVLUtilities::GetRandomizedArrayOfSize(size);
		ITERATE_TIMER_START(iter)
			delete[] randomKeyArray;
			binary_search_tree<int, Tracer> bst = BSTUtilities::CreateRandomTreeOfSize(size);
			randomKeyArray = AVLUtilities::GetRandomizedArrayOfSize(size);
			const size_t index = _ITCount % size;
		ITERATE_TIMER_HEADER_END
			bst.search(randomKeyArray[index]);
			bst.remove(randomKeyArray[index]);
			bst.emplace(randomKeyArray[index], randomKeyArray[index]);
		ITERATE_TIMER_END("BST Operation Time Complexity Test: Search/Remove/Insert To Random Tree of Size " << size)

		ITERATE_TIMER_START(iter)
			delete[] randomKeyArray;
			avl_tree<int, Tracer> avl = AVLUtilities::CreateRandomTreeOfSize(size);
			randomKeyArray = AVLUtilities::GetRandomizedArrayOfSize(size);
			const size_t index = _ITCount % size;
		ITERATE_TIMER_HEADER_END
			avl.search(randomKeyArray[index]);
			avl.remove(randomKeyArray[index]);
			avl.emplace(randomKeyArray[index], randomKeyArray[index]);
		ITERATE_TIMER_END("AVL Operation Time Complexity Test: Search/Remove/Insert To Random Tree of Size " << size)
	}
}