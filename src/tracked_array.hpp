#pragma once

#include <bitset>
#include <stdexcept>

// Forward iterator for tracked_array. Will only iterate through filled spots.
template <typename ArrayType>
class tracked_array_iterator {
public:
	using Iterator = typename tracked_array_iterator;
	using DataType = typename ArrayType::DataType;
	
	bool operator==(const Iterator& other) {
		return (this->ptr_ == other.ptr_);
	}
	bool operator!=(const Iterator& other) {
		return !(this->ptr_ == other.ptr_);
	}

	Iterator& operator++() {
		for (size_t i = location_ + 1; i < ArrayType::size_; i++) {
			if (trackedArray_->arrayField_[i] == 1) {
				location_ = i;
				this->ptr_ = &trackedArray_->array_[i];
					return *this;
			}
		}
		ptr_ = &trackedArray_->array_[ArrayType::size_];
		return *this;
	}
	Iterator operator++(int) {
		Iterator temp = *this;
		for (size_t i = location_ + 1; i < ArrayType::size_; i++) {
			if (trackedArray_->arrayField_[i] == 1) {
				location_ = i;
				ptr_ = &trackedArray_->array_[i];
				return temp;
			}
		}
		location_ = ArrayType::size_;
		ptr_ = &trackedArray_->array_[ArrayType::size_];
		return temp;
	}

	DataType* operator->() {
		return ptr_;
	}

	DataType& operator*() {
		return *ptr_;
	}

	tracked_array_iterator(ArrayType* array, size_t location)
		: trackedArray_(array)
		, ptr_(&trackedArray_->array_[location])
		, location_(location) {}

private:
	ArrayType* trackedArray_;
	DataType* ptr_;
	size_t location_;
};

// Array that keeps track of empty/filled indices, or slots.
// Empty slots contain default constructed objects, and are not accessible.
// Push/Emplace operations insert an object into the first empty slot of the array.
// This enables objects to be stored somewhat contiguously on the memory without
// needless copy operations on pushes/removes like some other contiguous DataType structures.
// Type must have a default constructor, a move constructor, and a move assignment operator.
template <typename Type, size_t Size>
class tracked_array {
public:
	using Iterator = typename tracked_array_iterator<tracked_array<Type, Size>>;
	using DataType = typename Type;

	// Constructs a new object in the first empty slot of the array.
	// Does not do a copy or move.
	// @return Index of slot item was moved into.
	// @exception std::out_of_range if array is full.
	template<typename... Args>
	size_t emplace(Args&&... args) {
		for (size_t i = 0; i < size_; i++) {
			if (arrayField_[i] == 0) {
				arrayField_[i] = 1;
				array_[i].~Type();
				new(&array_[i]) Type(std::forward<Args>(args)...);
				return i;
			}
		}
		throw std::out_of_range("Array is full.");
	}

	// Copies the element into the first empty slot of the array.
	// @return Index of slot item was moved into.
	// @exception std::out_of_range if array is full.
	size_t push(DataType& element) {
		for (size_t i = 0; i < size_; i++) {
			if (arrayField_[i] == 0) {
				arrayField_[i] = 1;
				array_[i] = element;
				return i;
			}
		}
		throw std::out_of_range("Array is full.");
	}

	// Moves the element into the first empty slot of the array.
	// @return Index of slot item was moved into.
	// @exception std::out_of_range if array is full.
	size_t push(DataType&& element) {
		for (size_t i = 0; i < size_; i++) {
			if (arrayField_[i] == 0) {
				arrayField_[i] = 1;
				array_[i] = std::move(element);
				return i;
			}
		}
		throw std::out_of_range("Array is full.");
	}

	// Destructs the element at index and frees the slot. 
	// Does nothing if slot was empty.
	void remove(size_t index) {
		if (arrayField_[index] == 1) {
			arrayField_[index] = 0;
			array_[index].~Type();
			new(&array_[index]) Type();
		}
	}

	// @return Reference to element at index.
	// @exception std::out_of_range if index is empty.
	DataType& operator[](size_t index) {
		if (arrayField_[index] == 1) {
			return array_[index];
		}
		throw std::out_of_range("Index is empty.");
	}
	// @return Const reference to element at index.
	// @exception std::out_of_range if index is empty.
	const DataType& operator[](size_t index) const {
		if (arrayField_[index] == 1) {
			return array_[index];
		}
		throw  std::out_of_range("Index is empty.");
	}

	constexpr size_t size() { return size_; }

	// @return Iterator at the beginning if at least one slot is full. Else returns an iterator at the end.
	Iterator begin() {
		for (size_t i = 0; i < size_; i++) {
			if (arrayField_[i] == 1) {
				return Iterator(this, i);
			}
		}
		return Iterator(this, size_);
	}
	Iterator end() {
		return Iterator(this, size_);
	}

	friend Iterator;
private:
	static const size_t size_ = Size;
	DataType array_[size_];
	std::bitset<size_> arrayField_;
};
