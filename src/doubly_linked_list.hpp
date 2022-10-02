#pragma once
#include <concepts>
#include <utility>
#include <stdexcept>



// <<<-------------------------------------------------->>>
// <<<----------- Class forward declarations ----------->>>
// <<<-------------------------------------------------->>>
template <typename DataType>
	requires std::copyable<DataType>
class doubly_linked_list_iterator;
template <typename DataType>
	requires std::copyable<DataType>
class doubly_linked_list_node;
template <typename DataType>
	requires std::copyable<DataType>
class doubly_linked_list;



template <typename DataType>
	requires std::copyable<DataType>
class doubly_linked_list_iterator {
	using Node = typename doubly_linked_list_node<DataType>;
	using Iterator = typename doubly_linked_list_iterator;
public:
	bool operator==(const doubly_linked_list_iterator& other) const {
		return (this->ptr_ == other->ptr_);
	}
	bool operator!=(const doubly_linked_list_iterator& other) const {
		return (this->ptr_ != other->ptr_);
	}

	Iterator& operator++() {
		this->ptr_ = this->ptr_->next_;
		return *this;
	}
	Iterator operator++(int) {
		Iterator temp = *this;
		++this;
		return temp;
	}

	Iterator& operator--() {
		this->ptr_ = this->ptr_->previous_;
		return *this;
	}
	Iterator operator--(int) {
		Iterator temp = *this;
		--this;
		return temp;
	}

	DataType& operator*() {
		return ptr_->data_;
	}
	DataType* operator->() {
		return ptr_->data_;
	}

	doubly_linked_list_iterator(Node* node) 
		: ptr_(node) {}
private:
	Node* ptr_ = nullptr;
};

template <typename DataType>
	requires std::copyable<DataType>
class doubly_linked_list_node {
	friend doubly_linked_list_iterator;
	friend doubly_linked_list;

	using Node = typename doubly_linked_list_node;
public:
	doubly_linked_list_node(const DataType& data) 
		: data_(data) {}
	doubly_linked_list_node(DataType&& data)
		: data_(std::move(data)) {}
	template<typename... Args>
	doubly_linked_list_node(Args... args)
		: data_(std::forward(args)...) {}
private:
	DataType data_;
	Node* previous_ = nullptr;
	Node* next_ = nullptr;
};

template <typename DataType>
	requires std::copyable<DataType>
class doubly_linked_list {
	using Node = typename doubly_linked_list_node<DataType>;
	using Iterator = typename doubly_linked_list_iterator<DataType>;
public:
	DataType& front() {
		if (this->size_ == 0) {
			throw std::out_of_range();
		}
		else {
			return this->head_->data_;
		}
	}
	const DataType& front() const {
		if (this->size_ == 0) {
			throw std::out_of_range();
		}
		else {
			return this->head_->data_;
		}
	}

	DataType& back() {
		if (this->size_ == 0) {
			throw std::out_of_range();
		}
		else {
			return this->tail_->data_;
		}
	}
	const DataType& back() const {
		if (this->size_ == 0) {
			throw std::out_of_range();
		}
		else {
			return this->tail_->data_;
		}
	}

	DataType& at(size_t index) {
		if ((index >= this->size_) || (index < 0)) {
			throw std::out_of_range();
		}
		else {
			Node* returnNode = this->head_;
			for (size_t i = index; i < this->size_; i++) {
				returnNode = returnNode->next_;
			}
			return returnNode->data_;
		}
	}
	const DataType& at(size_t index) const {
		if ((index >= this->size_) || (index < 0)) {
			throw std::out_of_range();
		}
		else {
			Node* returnNode = this->head_;
			for (size_t i = index; i < this->size_; i++) {
				returnNode = returnNode->next_;
			}
			return returnNode->data_;
		}
	}

	void push_back(const DataType& data) {
		Node* newNode = new Node(data);
		if (this->size_ == 0) {
			this->head_ = newNode;
			this->tail_ = newNode;
		}
		else{
			this->tail_->next_ = newNode;
			newNode->previous_ = this->tail_;
			this->tail_ = newNode;
		}
		++this->size_;
	}
	void push_back(DataType&& data) {
		Node* newNode = new Node(std::move(data));
		if (this->size_ == 0) {
			this->head_ = newNode;
			this->tail_ = newNode;
		}
		else {
			this->tail_->next_ = newNode;
			newNode->previous_ = this->tail_;
			this->tail_ = newNode;
		}
		++this->size_;
	}
	template<typename... Args>
	void emplace_back(Args... args) {
		Node* newNode = new Node(std::forward(args)...);
		if (this->size_ == 0) {
			this->head_ = newNode;
			this->tail_ = newNode;
		}
		else {
			this->tail_->next_ = newNode;
			newNode->previous_ = this->tail_;
			this->tail_ = newNode;
		}
		++this->size_;
	}

	void push_front(const DataType& data) {
		Node* newNode = new Node(data);
		if (this->size_ == 0) {
			this->head_ = newNode;
			this->tail_ = newNode;
		}
		else {
			this->head_->previous_ = newNode;
			newNode->next_ = this->head_;
			this->head_ = newNode;
		}
		++this->size_;
	}
	void push_front(DataType&& data) {
		Node* newNode = new Node(std::move(data));
		if (this->size_ == 0) {
			this->head_ = newNode;
			this->tail_ = newNode;
		}
		else {
			this->head_->previous_ = newNode;
			newNode->next_ = this->head_;
			this->head_ = newNode;
		}
		++this->size_;
	}
	template<typename... Args>
	void emplace_front(Args... args) {
		Node* newNode = new Node(std::forward(args)...);
		if (this->size_ == 0) {
			this->head_ = newNode;
			this->tail_ = newNode;
		}
		else {
			this->head_->previous_ = newNode;
			newNode->next_ = this->head_;
			this->head_ = newNode;
		}
		++this->size_;
	}

	void insert(size_t index, const DataType& data) {
		if ((index > this->size_) || (index < 0)) {
			throw std::out_of_range();
		}
		else {
			Node* newNode = new Node(data);
			if (this->size_ == 0) {
				this->head_ = newNode;
				this->tail_ = newNode;
			}
			else if (index == 0) {
				this->head_->previous_ = newNode;
				newNode->next_ = this->head_;
				this->head_ = newNode;
			}
			else if (index == this->size_) {
				this->tail_->next_ = newNode;
				newNode->previous_ = this->tail_;
				this->tail_ = newNode;
			}
			else {
				Node* nextNode = this->head_;
				for (size_t i = index; i < this->size_; i++) {
					nextNode = nextNode->next_;
				}
				nextNode->previous_->next_ = newNode;
				newNode->previous_ = nextNode->previous_;
				nextNode->previous_ = newNode;
				newNode->next_ = nextNode;
			}
			++this->size_;
		}
	}
	void insert(size_t index, DataType&& data) {
		if ((index > this->size_) || (index < 0)) {
			throw std::out_of_range();
		}
		else {
			Node* newNode = new Node(std::move(data));
			if (this->size_ == 0) {
				this->head_ = newNode;
				this->tail_ = newNode;
			}
			else if (index == 0) {
				this->head_->previous_ = newNode;
				newNode->next_ = this->head_;
				this->head_ = newNode;
			}
			else if (index == this->size_) {
				this->tail_->next_ = newNode;
				newNode->previous_ = this->tail_;
				this->tail_ = newNode;
			}
			else {
				Node* nextNode = this->head_;
				for (size_t i = index; i < this->size_; i++) {
					nextNode = nextNode->next_;
				}
				nextNode->previous_->next_ = newNode;
				newNode->previous_ = nextNode->previous_;
				nextNode->previous_ = newNode;
				newNode->next_ = nextNode;
			}
			++this->size_;
		}
	}
	template<typename... Args>
	void emplace(size_t index, Args... args) {
		if ((index > this->size_) || (index < 0)) {
			throw std::out_of_range();
		}
		else {
			Node* newNode = new Node(std::forward(args)...);
			if (this->size_ == 0) {
				this->head_ = newNode;
				this->tail_ = newNode;
			}
			else if (index == 0) {
				this->head_->previous_ = newNode;
				newNode->next_ = this->head_;
				this->head_ = newNode;
			}
			else if (index == this->size_) {
				this->tail_->next_ = newNode;
				newNode->previous_ = this->tail_;
				this->tail_ = newNode;
			}
			else {
				Node* nextNode = this->head_;
				for (size_t i = index; i < this->size_; i++) {
					nextNode = nextNode->next_;
				}
				nextNode->previous_->next_ = newNode;
				newNode->previous_ = nextNode->previous_;
				nextNode->previous_ = newNode;
				newNode->next_ = nextNode;
			}
			++this->size_;
		}
	}

	void pop_back() {
		if (this->size_ == 0) {
			return;
		}
		else if (this->size_ == 1) {
			delete this->tail_;
			this->head_ = nullptr;
			this->tail_ = nullptr;
		}
		else {
			Node* tail = this->tail_;
			tail->previous_->next_ = nullptr;
			this->tail_ = tail->previous_;
			delete tail;
		}
		--this->size_;
	}
	void pop_front() {
		if (this->size_ == 0) {
			return;
		}
		else if (this->size_ == 1) {
			delete this->head_;
			this->head_ = nullptr;
			this->tail_ = nullptr;
		}
		else {
			Node* head = this->tail_;
			head->next_->previous_ = nullptr;
			this->head_ = head->next_;
			delete head;
		}
		--this->size_;
	}
	void remove(size_t index) {
		if ((index >= this->size_) || (index < 0)) {
			throw std::out_of_range();
		}
		else if (this->size_ == 1) {
			delete this->head_;
			this->head_ = nullptr;
			this->tail_ = nullptr;
		}
		else {
			Node* node = this->head_;
			for (size_t i = index; i < this->size_; i++) {
				node = node->next_;
			}

			if (node->previous_) {
				node->previous_->next_ = node->next_;
			}
			else {
				this->head_ = node->next_;
				node->next_->previous_ = nullptr;
			}

			if (node->next_) {
				node->next_->previous_ = node->previous_;
			}
			else {
				this->tail_ = node->previous_;
				node->previous_->next_ = nullptr;
			}

			delete node;
		}
		this->size_--;
	}

	void clear() {
		if (this->size_ != 0) {
			Node* thisPtr = nullptr;
			Node* nextPtr = this->head_;
			for (size_t i = 0; i < (this->size_ - 1); i++) {
				delete thisPtr;
				thisPtr = nextPtr;
				nextPtr = thisPtr->next_;
			}
			delete nextPtr;

			this->size_ = 0;
			this->head_ = nullptr;
			this->tail_ = nullptr;
		}
	}

	Iterator begin() {
		if (this->size_ == 0) {
			return this->end();
		}
		else {
			return Iterator(this->head_);
		}
	}
	Iterator end() {
		return Iterator(nullptr);
	}

	doubly_linked_list(const doubly_linked_list& other) {
		this->size_ = other.size_;
		if (other.size_ == 0) {
			return;
		}
		else if (other.size_ == 1) {
			Node* newNode = new Node(other.head_);
			this->head_ = newNode;
			this->tail_ = newNode;
		}
		else {
			Node* currentNode = new Node(other.head_); 
			Node* newNode = nullptr;
			this->head_ = currentNode;
			for (Node* iterNode = other.head_->next_; iterNode != nullptr; iterNode = iterNode->next_) {
				newNode = new Node(iterNode);
				currentNode->next_ = newNode;
				newNode->previous_ = currentNode;
				currentNode = newNode;
			}
			this->tail_ = currentNode;
		}
	}
	doubly_linked_list& operator=(const doubly_linked_list& other) {
		this->clear();
		this->size_ = other.size_;
		if (other.size_ == 0) {
			return;
		}
		else if (other.size_ == 1) {
			Node* newNode = new Node(other.head_);
			this->head_ = newNode;
			this->tail_ = newNode;
		}
		else {
			Node* currentNode = new Node(other.head_);
			Node* newNode = nullptr;
			this->head_ = currentNode;
			for (Node* iterNode = other.head_->next_; iterNode != nullptr; iterNode = iterNode->next_) {
				newNode = new Node(iterNode);
				currentNode->next_ = newNode;
				newNode->previous_ = currentNode;
				currentNode = newNode;
			}
			this->tail_ = currentNode;
		}
	}
	doubly_linked_list(doubly_linked_list&& other) {
		this->size_ = other.size_;
		this->head_ = other.head_;
		this->tail_ = other.tail_;
		other.size_ = 0;
		other.head_ = nullptr;
		other.tail_ = nullptr;
	}
	doubly_linked_list& operator=(doubly_linked_list&& other) {
		this->clear();
		this->size_ = other.size_;
		this->head_ = other.head_;
		this->tail_ = other.tail_;
		other.size_ = 0;
		other.head_ = nullptr;
		other.tail_ = nullptr;
	}
	~doubly_linked_list() {
		this->clear();
	}

	doubly_linked_list() {};
private:
	Node* head_ = nullptr;
	Node* tail_ = nullptr;
	size_t size_ = 0;
};