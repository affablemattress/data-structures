#include "tracked_array.hpp"
#include "binary_search_tree.hpp"

#include <iostream>
#include <chrono>
#include <cstdint>

#define TIMER_START {auto _TStartTime = std::chrono::high_resolution_clock::now();
#define TIMER_END(timerName) auto _TCurrentTime = std::chrono::high_resolution_clock::now(); std::cerr << "<<< [" << timerName << "] ran for: " << (_TCurrentTime - _TStartTime) << " >>>\n";}

#define ITERATE_TIMER_START(_ITNumberOfIterations) { unsigned int _ITIterationCount = _ITNumberOfIterations; unsigned long long _ITTotalTime = 0; for (size_t _ITCount = 0; _ITCount < _ITIterationCount; _ITCount++) { auto _ITStartTime = std::chrono::high_resolution_clock::now();
#define ITERATE_TIMER_END(_ITTimerName) auto _ITCurrentTime = std::chrono::high_resolution_clock::now(); _ITTotalTime += (_ITCurrentTime - _ITStartTime).count();} std::cerr << "<<< [" << _ITTimerName << "] ran for an average of: " << (_ITTotalTime / _ITIterationCount) << "ns >>>\n";}

#define LOG(msg) std::cerr << msg << "\n";

//#define TRACE
class Dummy {
public:
	Dummy(const Dummy& other)
		: order(count++)
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
		: order(count++)
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
		count++;
	}
	Dummy()
		: order(count++)
		, array(new int[10]) {
	}

	int order;
	int* array;
private:
	inline static int count = 0;
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
		: order(count++)
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
		count++;
#ifdef TRACE
		std::cout << "Constructed [" << this->order << "]\n";
#endif // TRACE
	}
	Tracer()
		: order(count++) 
		, array(new int[10])
		, dummy(new Dummy()) {
		count++;
#ifdef TRACE
		std::cout << "Default Constructed [" << this->order << "]\n";
#endif // !TRACE
	}

	int order;
	int* array;
	Dummy* dummy;
private:
	inline static int count = 0;
};

constexpr size_t iter = 1000;
constexpr size_t size = 10;

namespace BSTUtilities{
	void PushLevel(binary_search_tree<int, Tracer>& bst, size_t treeHeight, size_t level) {
		uint32_t numberOfElementsInLevel = 1 << (treeHeight - level);;
		uint32_t firstElementsKey = 1 << (level - 1);
		uint32_t increment = 1 << level;
		for (uint32_t i = 0; i < numberOfElementsInLevel; i++)
		{
			uint32_t key = firstElementsKey + (increment * i);
			bst.emplace(key, key);
#ifdef TRACE
			std::cerr << ">>>Emplaced " << key << "\n";
#endif // !TRACE
		}
	}
	binary_search_tree<int, Tracer> CreatePerfectTree(int treeHeight) {
		binary_search_tree<int, Tracer> bst;
		for (size_t i = treeHeight; i > 0; i--)
		{
			PushLevel(bst, treeHeight, i);
		}
		return bst;
	}
}

int main(int argc, char** args) {
	unsigned int counter = 0;

	binary_search_tree<int, Tracer> bstXXL = BSTUtilities::CreatePerfectTree(size);
	ITERATE_TIMER_START(iter)
		counter += bstXXL.search(1)->data.order;
	ITERATE_TIMER_END("Full Depth Search with Tree of Depth " << size)
}