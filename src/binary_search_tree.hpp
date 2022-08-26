#pragma once
#include <concepts>
#include <utility>
#include <stdexcept>

// <<<-------------------------------------------------->>>
// <<<----------- Class forward declarations ----------->>>
// <<<-------------------------------------------------->>>
template<typename KeyType, typename DataType>
	requires (std::totally_ordered<KeyType>&& std::copyable<KeyType>
			  && std::copyable<DataType>)
class binary_search_tree_iterator;
template<typename KeyType, typename DataType>
	requires (std::totally_ordered<KeyType> && std::copyable<KeyType>
			  && std::copyable<DataType>)
class binary_search_tree_node;
template<typename KeyType, typename DataType>
	requires (std::totally_ordered<KeyType> && std::copyable<KeyType>
			  && std::copyable<DataType>)
class binary_search_tree;


// An in-order traversal forward iterator.
// end() iterator is nullptr.
template<typename KeyType, typename DataType>
	requires (std::totally_ordered<KeyType> && std::copyable<KeyType>
			  && std::copyable<DataType>)
class binary_search_tree_iterator {
	using Iterator = typename binary_search_tree_iterator;
	using Node = typename binary_search_tree_node<KeyType, DataType>;
public:
	bool operator==(const Iterator& other) const {
		return (this->ptr_ == other.ptr_);
	}
	bool operator!=(const Iterator& other) const {
		return (this->ptr_ != other.ptr_);
	}

	Iterator& operator++() {
		if (!ptr_) {
			return (*this);
		}

		if (ptr_->right_) {
			ptr_ = ptr_->right_;
			while (ptr_->left_) {
				ptr_ = ptr_->left_;
			}
			return *this;
		}

		while (ptr_->parent_) {
			if (ptr_->key < ptr_->parent_->key) {
				ptr_ = ptr_->parent_;
				return *this;
			}
			else {
				ptr_ = ptr_->parent_;
			}
		}

		ptr_ = nullptr;
		return *this;
	}
	Iterator operator++(int) {
		Iterator temp = *this;
		++(*this);
		if (!temp.ptr_) {
			return (temp);
		}

		if (temp.ptr_->right_) {
			temp.ptr_ = temp.ptr_->right_;
			while (temp.ptr_->left_) {
				temp.ptr_ = temp.ptr_->left_;
			}
			return temp;
		}

		while (temp.ptr_->parent_) {
			if (temp.ptr_->key < temp.ptr_->parent_->key) {
				temp.ptr_ = temp.ptr_->parent_;
				return temp;
			}
			else {
				temp.ptr_ = temp.ptr_->parent_;
			}
		}

		temp.ptr_ = nullptr;
		return temp;
	}

	DataType* operator->() {
		return this->ptr_->data;
	}
	DataType& operator*() {
		return this->ptr_->data;
	}

	binary_search_tree_iterator(Node* node)
		: ptr_(node) {}
private:
	Node* ptr_;
};

// Holds a key-data pair.
template<typename KeyType, typename DataType>
	requires (std::totally_ordered<KeyType> && std::copyable<KeyType>
			  && std::copyable<DataType>)
class binary_search_tree_node {
	using Node = typename binary_search_tree_node;
public:
	DataType data;
	const KeyType key;

	binary_search_tree_node(const KeyType& key_, DataType&& data_)
		: key(key_)
		, data(std::forward<DataType>(data_)) {}
	template <typename... ArgTypes>
	binary_search_tree_node(const KeyType& key_, ArgTypes&&... args)
		: key(key_)
		, data(DataType(std::forward<ArgTypes>(args)...)) {}

	friend binary_search_tree_iterator<KeyType, DataType>;
	friend binary_search_tree<KeyType, DataType>;
private:
	Node* left_ = nullptr;
	Node* right_ = nullptr;
	Node* parent_ = nullptr;
};

// A binary search tree implementation.
// The key is a seperate member from the data, this means the DataType 
// doesn't have to have comparison operators implemented.
// KeyType must be copyable and totally_ordered.
// DataType must be copyable.
template<typename KeyType, typename DataType>
	requires (std::totally_ordered<KeyType> && std::copyable<KeyType>
			  && std::copyable<DataType>)
class binary_search_tree {
	using Iterator = typename binary_search_tree_iterator<KeyType, DataType>;
	using Node = typename binary_search_tree_node<KeyType, DataType>;
public:
	// @return nullptr if key is not present in the tree.
	Node* search(const KeyType& key) {
		if (root_) {
			return search_subtree(key, root_);
		}
		else {
			return nullptr;
		}
	}

	// Creates a node on the tree. Does a copy operation on the data.
	bool insert(const KeyType& key, const DataType& data) {
		Node* newNode = new Node(key, data);
		if (root_) {
			Node* parent = find_parent_for_key_in_subtree(key, root_);
			if (parent) {
				this->insert_node_at(parent, newNode);
			}
			else {
				return false;
			}
		}
		else {
			root_ = newNode;
		}
		return true;
	}
	// Creates a newNode on the tree. Does a move operation on the data.
	bool insert(const KeyType& key, DataType&& data) {
		Node* newNode = new Node(key, std::move(data));
		if (root_) {
			Node* parent = find_parent_for_key_in_subtree(key, root_);
			if (parent) {
				this->insert_node_at(parent, newNode);
			}
			else {
				return false;
			}
		}
		else {
			root_ = newNode;
		}
		return true;
	}
	// Creates a newNode on the tree. Constructs the DataType object in place (avoids copy/move operations).
	// @param[...args] args are passed to the DataType constructor.
	template <typename... ArgTypes>
	bool emplace(const KeyType key, ArgTypes... args) {
		Node* newNode = new Node(key, std::forward<ArgTypes>(args)...);
		if (root_) {
			Node* parent = find_parent_for_key_in_subtree(key, root_);
			if (parent) {
				this->insert_node_at(parent, newNode);
			}
			else {
				return false;
			}
		}
		else {
			root_ = newNode;
		}
		return true;
	}

	// Removes an element from the tree and calls the destructor on its data.
	void remove(const KeyType& key) {
		Node* node = this->search(key);

		if (node) {
			if (this->root_ != node)
			{
				if (key > node->parent_->key) {
					node->parent_->right_ = nullptr;
				}
				else {
					node->parent_->left_ = nullptr;
				}
			} 
			else {
				this->root_ = nullptr;
			}

			if (node->left_) {
				this->insert(node->left_);
			}
			if (node->right_) {
				this->insert(node->right_);
			}
			delete node;
		}

	}
	// Removes all elements from the tree.
	void clear() {
		if (this->root_)
			clear_subtree(this->root_);
			this->root_ = nullptr;
	}

	// @return An in-order traversal forward iterator pointing at the smallest element of the tree.
	Iterator begin() {
		if (!this->root_) {
			return Iterator(nullptr);
		}

		Node* smallestNode = this->root_;
		while (smallestNode->left_) {
			smallestNode = smallestNode->left_;
		}
		return Iterator(smallestNode);
	}   
	// @return An in-order traversal forward iterator pointing at nullptr.
	Iterator end() {
		return Iterator(nullptr);
	}

	binary_search_tree(binary_search_tree& other) 
		: root_(nullptr) {
		if (other.root_)
			clone_subtree(nullptr, this->root_, other.root_);
	}
	binary_search_tree& operator=(binary_search_tree& other) {
		this->clear();
		if (other.root_)
			clone_subtree(nullptr, this->root_, other.root_);
		return *this;
	}
	binary_search_tree(binary_search_tree&& other) noexcept
		: root_(other.root_) {
		other.root_ = nullptr;
	}
	binary_search_tree& operator=(binary_search_tree&& other) noexcept {
		this->clear();
		this->root_ = other.root_;
		other.root_ = nullptr;
	}
	~binary_search_tree() {
		this->clear();
	}

	binary_search_tree() {}
private:
	// Recursive method that searches a subtree for a suitable parent to attach the passed key to.
	// Used by insert() and emplace() methods.
	// @return nullptr if key is already in the tree.
	static Node* find_parent_for_key_in_subtree(const KeyType& key, Node* node) {
		if (node->key == key) {
			return nullptr;
		}
		else if (key < node->key) {
			if (!node->left_) {
				return node;
			}
			else {
				return find_parent_for_key_in_subtree(key, node->left_);
			}
		} 
		else {
			if (!node->right_) {
				return node;
			}
			else {
				return find_parent_for_key_in_subtree(key, node->right_);
			}
		}
	}
	// Inserts an existing node into the tree. 
	static void insert_node_at(Node* parent, Node* node) {
		node->parent_ = parent;
		if (node->key < parent->key) {
			parent->left_ = node;
		}
		else {
			parent->right_ = node;
		}
	}

	// Recursive method that searches a subtree for key.
	// Used by search() and remove() methods.
	// @return nullptr if key is not present in the tree.
	static Node* search_subtree(const KeyType& key, Node* node) {
		if (key < node->key) {
			if (node->left_) {
				return search_subtree(key, node->left_);
			} 
			else {
				return nullptr;
			}
		}
		else if (key < node->key) {
			if (node->right_) {
				return search_subtree(key, node->right_);
			}
			else {
				return nullptr;
			}
		}
		else {
			return node;
		}
	}
	// Recursive method that clears a subtree. 
	// Used by clear() method.
	static void clear_subtree(Node* node) {
		if (node->left_)
			clear_subtree(node->left_);
		if (node->right_)
			clear_subtree(node->right_);
		delete node;
	}
	// Recursive method that copies a subtree to destination. 
	// Used by the copy constructor and the copy assign operator.
	static void clone_subtree(Node* destinationParent, Node*& destination, Node*& source) {
		destination = new Node(*source);
		destination->parent_ = destinationParent;
		if (source->left_)
			clone_subtree(destination, destination->left_, source->left_);
		if (source->right_)
			clone_subtree(destination, destination->right_, source->right_);
	}

	Node* root_ = nullptr;
};