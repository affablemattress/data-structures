#pragma once
#include <utility>
#include <stdexcept>

template<typename KeyType, typename DataType>
class binary_search_tree_node;

template<typename KeyType, typename DataType>
class binary_search_tree;

template<typename KeyType, typename DataType>
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
			return *this;
		}

		if (ptr_->right_) {
			ptr_ = ptr_->right_;
			while (ptr_->left_) {
				ptr_ = ptr_->left_;
			}
			return *this;
		}
		while (true) {
			if (!ptr_->parent_) {
				ptr_ = nullptr;
				return *this;
			}
			else if (ptr_->key < ptr_->parent_->key) {
				ptr_ = ptr_->parent_;
				return *this;
			}
			else {
				ptr_ = ptr_->parent_;
			}
		}
	}
	Iterator operator++(int) {
		Iterator temp = Iterator(*this);
		++(*this);
		if (temp.ptr_->right_) {
			temp.ptr_ = temp.ptr_->right_;
			while (temp.ptr_->left_) {
				temp.ptr_ = temp.ptr_->left_;
			}
			return temp;
		}
		while (true) {
			if (!temp.ptr_->parent_) {
				temp.ptr_ = nullptr;
				return temp;
			}
			else if (temp.ptr_->key < temp.ptr_->parent_->key) {
				temp.ptr_ = temp.ptr_->parent_;
				return temp;
			}
			else {
				temp.ptr_ = temp.ptr_->parent_;
			}
		}
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

template<typename KeyType, typename DataType>
class binary_search_tree_node {
	using Node = typename binary_search_tree_node;
public:
	KeyType key;
	DataType data;

	binary_search_tree_node(const KeyType& key_, DataType&& data_)
		: key(key_)
		, data(std::forward<DataType>(data_)) {}
	template <typename... ArgTypes>
	binary_search_tree_node(const KeyType& key_, ArgTypes&&... args)
		: key(key_)
		, data(std::forward<ArgTypes>(args)...) {
	}

	friend binary_search_tree_iterator<KeyType, DataType>;
	friend binary_search_tree<KeyType, DataType>;
private:
	Node* left_ = nullptr;
	Node* right_ = nullptr;
	Node* parent_ = nullptr;
};

template<typename KeyType, typename DataType>
class binary_search_tree {
	using Iterator = typename binary_search_tree_iterator<KeyType, DataType>;
	using Node = typename binary_search_tree_node<KeyType, DataType>;
public:
	Node* search(const KeyType& key) {
		return search_subtree(key, root_);
	}

	void insert(const KeyType& key, DataType& data) {
		Node* newNode = new Node(key, data);
		if (root_) {
			Node* correctLeaf = find_correct_leaf_for_key_in_subtree(key, this->root_);
			newNode->parent_ = correctLeaf;
			if (key < correctLeaf->key) {
				correctLeaf->left_ = newNode;
			}
			else {
				correctLeaf->right_ = newNode;
			}
		}
		else {
			root_ = newNode;
		}
	}
	void insert(const KeyType& key, DataType&& data) {
		Node* newNode = new Node(key, std::move(data));
		if (root_) {
			Node* correctLeaf = find_correct_leaf_for_key_in_subtree(key, this->root_);
			newNode->parent_ = correctLeaf;
			if (key < correctLeaf->key) {
				correctLeaf->left_ = newNode;
			}
			else {
				correctLeaf->right_ = newNode;
			}
		}
		else {
			root_ = newNode;
		}
	}
	template <typename... ArgTypes>
	void emplace(const KeyType key, ArgTypes... args) {
		Node* newNode = new Node(key, std::forward<ArgTypes>(args)...);
		if (root_) {
			Node* correctLeaf = find_correct_leaf_for_key_in_subtree(key, this->root_);
			newNode->parent_ = correctLeaf;
			if (key < correctLeaf->key) {
				correctLeaf->left_ = newNode;
			}
			else {
				correctLeaf->right_ = newNode;
			}
		}
		else {
			root_ = newNode;
		}
	}

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
	void clear() {
		if (this->root_)
			clear_subtree(this->root_);
			this->root_ = nullptr;
	}

	Iterator begin() {
		if (!this->root_) {
			return Iterator(nullptr);
		}

		Node* smallestNode = this->root_;
		while (true) {
			if (smallestNode->left_)
				smallestNode = smallestNode->left_;
			else
				return Iterator(smallestNode);
		}
	}             
	Iterator end() {
		return Iterator(nullptr);
	}

	binary_search_tree(binary_search_tree& other) 
		: root_(nullptr) {    //TO DO: Implement copy constructor?
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
	static Node* search_subtree(const KeyType& key, Node* node) {
		if (!node) {
			return nullptr;
		}
		else {
			if (node->key == key) {
				return node;
			}
			else if (key < node->key) {
				return search_subtree(key, node->left_);
			}
			else {
				return search_subtree(key, node->right_);
			}
		}
	}

	static Node* find_correct_leaf_for_key_in_subtree(const KeyType& key, Node* node) {
		if (node->key == key) {
			throw std::invalid_argument("Key is already in tree.");
		}
		else if (key < node->key) {
			if (!node->left_) {
				return node;
			}
			else {
				return find_correct_leaf_for_key_in_subtree(key, node->left_);
			}
		} 
		else {
			if (!node->right_) {
				return node;
			}
			else {
				return find_correct_leaf_for_key_in_subtree(key, node->right_);
			}
		}
	}

	static void clear_subtree(Node* node) {
		if (node->left_)
			clear_subtree(node->left_);
		if (node->right_)
			clear_subtree(node->right_);
		delete node;
	}
	static void clone_subtree(Node* destinationParent, Node*& destination, Node*& source) {
		destination = new Node(*source);
		destination->parent_ = destinationParent;
		if (source->left_)
			clone_subtree(destination, destination->left_, source->left_);
		if (source->right_)
			clone_subtree(destination, destination->right_, source->right_);
	}

	void insert(Node* node) {
		KeyType& key = node->key;

		if (root_) {
			Node* correctLeaf = find_correct_leaf_for_key_in_subtree(key, this->root_);
			node->parent_ = correctLeaf;
			if (key < correctLeaf->key) {
				correctLeaf->left_ = node;
			}
			else {
				correctLeaf->right_ = node;
			}
		}
		else {
			root_ = node;
			node->parent_ = nullptr;
		}
	}

	Node* root_ = nullptr;
};