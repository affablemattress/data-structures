#pragma once
#include <concepts>
#include <utility>
#include <cstdint>



// <<<-------------------------------------------------->>>
// <<<----------- Class forward declarations ----------->>>
// <<<-------------------------------------------------->>>
template<typename KeyType, typename DataType>
	requires (std::totally_ordered<KeyType>&& std::copyable<KeyType>
			  && std::copyable<DataType>)
class avl_tree_iterator;
template<typename KeyType, typename DataType>
	requires (std::totally_ordered<KeyType>&& std::copyable<KeyType>
			  && std::copyable<DataType>)
class avl_tree_node;
template<typename KeyType, typename DataType>
	requires (std::totally_ordered<KeyType>&& std::copyable<KeyType>
			  && std::copyable<DataType>)
class avl_tree;



// An in-order traversal two way iterator.
// end() iterator is nullptr.
template<typename KeyType, typename DataType>
	requires (std::totally_ordered<KeyType>&& std::copyable<KeyType>
			  && std::copyable<DataType>)
class avl_tree_iterator {
	using Iterator = typename avl_tree_iterator;
	using Node = typename avl_tree_node<KeyType, DataType>;
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

	Iterator& operator--() {
		if (!ptr_) {
			return (*this);
		}

		if (ptr_->left_) {
			ptr_ = ptr_->left_;
			while (ptr_->right_) {
				ptr_ = ptr_->right_;
			}
			return *this;
		}

		while (ptr_->parent_) {
			if (ptr_->key > ptr_->parent_->key) {
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
	Iterator operator--(int) {
		Iterator temp = *this;
		++(*this);
		if (!temp.ptr_) {
			return (temp);
		}

		if (temp.ptr_->left_) {
			temp.ptr_ = temp.ptr_->left_;
			while (temp.ptr_->right_) {
				temp.ptr_ = temp.ptr_->right_;
			}
			return temp;
		}

		while (temp.ptr_->parent_) {
			if (temp.ptr_->key > temp.ptr_->parent_->key) {
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

	Node* operator->() {
		return this->ptr_;
	}
	Node& operator*() {
		return *(this->ptr_);
	}

	avl_tree_iterator(Node* node)
		: ptr_(node) {}

	friend avl_tree<KeyType, DataType>;
private:
	Node* ptr_;
};

// Holds a key-data pair and a balance integer.
template<typename KeyType, typename DataType>
	requires (std::totally_ordered<KeyType>&& std::copyable<KeyType>
			  && std::copyable<DataType>)
class avl_tree_node {
	using Node = typename avl_tree_node;
public:
	DataType data;
	KeyType key;

	avl_tree_node(const KeyType& key_, DataType&& data_)
		: key(key_)
		, data(DataType(std::forward<DataType>(data_))) {}
	template <typename... ArgTypes>
	avl_tree_node(const KeyType& key_, ArgTypes&&... args)
		: key(key_)
		, data(DataType(std::forward<ArgTypes>(args)...)) {}

	friend avl_tree_iterator<KeyType, DataType>;
	friend avl_tree<KeyType, DataType>;
private: 
	int_fast8_t balanceFactor_ = 0;
	Node* parent_ = nullptr;
	Node* left_ = nullptr;
	Node* right_ = nullptr;
};

// An AVL tree implementation.
// The key is a seperate member from the data, this means the DataType 
// doesn't have to have comparison operators implemented.
// KeyType must be copyable and totally_ordered.
// DataType must be copyable.
template<typename KeyType, typename DataType>
	requires (std::totally_ordered<KeyType>&& std::copyable<KeyType>
			  && std::copyable<DataType>)
class avl_tree {
	using Iterator = typename avl_tree_iterator<KeyType, DataType>;
	using Node = typename avl_tree_node<KeyType, DataType>;
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
	// @return false if key is already in tree.
	bool insert(const KeyType& key_, const DataType& data_) {
		if (this->root_) {
			Node* parent = find_parent_for_key_in_subtree(key_, this->root_);
			if (parent) {
				this->insert_node_at(parent, new Node(key_, std::move(data_)));
			}
			else {
				return false;
			}
		}
		else {
			this->root_ = new Node(key_, std::move(data_));
		}
		return true;
	}
	// Creates a newNode on the tree. Does a move operation on the data.
	// @return false if key is already in tree.
	bool insert(const KeyType& key_, DataType&& data_) {
		if (this->root_) {
			Node* parent = find_parent_for_key_in_subtree(key_, this->root_);
			if (parent) {
				this->insert_node_at(parent, new Node(key_, std::forward<DataType>(data_)));
			}
			else {
				return false;
			}
		}
		else {
			this->root_ = new Node(key_, std::forward<DataType>(data_));
		}
		return true;
	}
	// Creates a newNode on the tree. Constructs the DataType object in place (avoids copy/move operations).
	// @param[...args] args are passed to the DataType constructor.
	// @return false if key is already in tree.
	template <typename... ArgTypes>
	bool emplace(const KeyType key_, ArgTypes... args) {
		if (this->root_) {
			Node* parent = find_parent_for_key_in_subtree(key_, this->root_);
			if (parent) {
				this->insert_node_at(parent, new Node(key_, std::forward<ArgTypes>(args)...));
			}
			else {
				return false;
			}
		}
		else {
			this->root_ = new Node(key_, std::forward<ArgTypes>(args)...);
		}
		return true;
	}

	// Removes an element from the tree and calls the destructor on its data. 
	// If the removed element has 2 children, copies the max() in left subtree to the element's place then deletes the original copy.
	// Then does the rebalancing.
	bool remove(const KeyType& key_) {
		Node* node = this->search(key_);

		if (node) {
			if (node != this->root_) {
				if (!(node->left_ || node->right_)) {
					Node*& parentsCorrectPointer = (node->key < node->parent_->key) ? node->parent_->left_ : node->parent_->right_;
					parentsCorrectPointer = nullptr;
					balance_parents_after_remove(node->parent_, key_);
					delete node;
				}
				else  if (!node->left_ != !node->right_) {
					Node*& parentsCorrectPointer = (node->key < node->parent_->key) ? node->parent_->left_ : node->parent_->right_;
					if (node->left_) {
						parentsCorrectPointer = node->left_;
						node->left_->parent_ = node->parent_;
						balance_parents_after_remove(node->parent_, key_);
						delete node;
					}
					else {
						parentsCorrectPointer = node->right_;
						node->right_->parent_ = node->parent_;
						balance_parents_after_remove(node->parent_, key_);
						delete node;
					}
				}
				else if (node->left_ && node->right_) {
					Node* replacementNode = find_max_in_subtree(node->left_);
					Node*& parentsCorrectPointer = (replacementNode->key < replacementNode->parent_->key) ? replacementNode->parent_->left_ : replacementNode->parent_->right_;
					node->key = std::move(replacementNode->key);
					node->data = std::move(replacementNode->data);
					if (replacementNode->left_) {
						replacementNode->left_->parent_ = replacementNode->parent_;
						parentsCorrectPointer = replacementNode->left_;
					}
					else {
						parentsCorrectPointer = nullptr;
					}
					balance_parents_after_remove(replacementNode->parent_, node->key);
					delete replacementNode;
				}
			}
			else {
				if (!(node->left_ || node->right_)) {
					this->root_ = nullptr;
					delete node;
				}
				else  if (!node->left_ != !node->right_) {
					if (node->left_) {
						this->root_ = node->left_;
						root_->parent_ = nullptr;
						delete node;
					}
					else {
						this->root_ = node->right_;
						root_->parent_ = nullptr;
						delete node;
					}
				}
				else if (node->left_ && node->right_) {
					Node* replacementNode = find_max_in_subtree(node->left_);
					Node*& parentsCorrectPointer = (replacementNode->key < replacementNode->parent_->key) ? replacementNode->parent_->left_ : replacementNode->parent_->right_;
					node->key = std::move(replacementNode->key);
					node->data = std::move(replacementNode->data);
					if (replacementNode->left_) {
						replacementNode->left_->parent_ = replacementNode->parent_;
						parentsCorrectPointer = replacementNode->left_;
					}
					else {
						parentsCorrectPointer = nullptr;
					}
					balance_parents_after_remove(replacementNode->parent_, node->key);
					delete replacementNode;
				}
			}
			return true;
		}
		else {
			return false;
		}
	}

	// Removes all elements from the tree.
	void clear() {
		if (this->root_) {
			std::vector<Node*> allNodes;
			for (Node& node : *this) {
				allNodes.push_back(&node);
			}
			for (Node* node : allNodes) {
				delete node;
			}
			this->root_ = nullptr;
		}
	}
	
	Node* min() {
		if (root_) {
			return find_min_in_subtree(root_);
		}
		else {
			return nullptr;
		}
	}
	Node* max() {
		if (root_) {
			return find_max_in_subtree(root_);

		}
		else {
			return nullptr;
		}
	}

	// @return An in-order traversal iterator pointing at the smallest element of the tree.
	Iterator begin() {
		if (!this->root_) {
			return this->end();
		}
		return Iterator(this->min());
	}
	// @return An in-order traversal iterator pointing at nullptr.
	Iterator end() {
		return Iterator(nullptr);
	}

	avl_tree(const avl_tree& other) {
		this->root_ = nullptr;
		if (other.root_)
			clone_subtree(nullptr, this->root_, other.root_);
	}
	avl_tree& operator=(const avl_tree& other) {
		this->clear();
		if (other.root_)
			clone_subtree(nullptr, this->root_, other.root_);
		return *this;
	}
	avl_tree(avl_tree&& other) noexcept {
		this->root_ = other.root_;
		other.root_ = nullptr;
	}
	avl_tree& operator=(avl_tree&& other) noexcept {
		this->clear();
		this->root_ = other.root_;
		other.root_ = nullptr;
	}
	~avl_tree() {
		this->clear();
	}

	avl_tree() 
		: root_(nullptr) {}
private:
	bool rotate_left(Node* root) {
		Node* const pivot = root->right_;
		bool isHeightReduced = (pivot->balanceFactor_ == -1);

		//rearrange nodes.
		Node* pivotLeft = pivot->left_;

		if (root->parent_) {
			(root->key < root->parent_->key) ? root->parent_->left_ = pivot : root->parent_->right_ = pivot;
		}
		else {
			this->root_ = pivot;
		}
		pivot->parent_ = root->parent_;
		
		       
		pivot->left_ = root;
		root->parent_ = pivot;

		root->right_ = pivotLeft;
		if (pivotLeft) {
			pivotLeft->parent_ = root;
		}

		//set balance factors.
		if (pivot->balanceFactor_ == -1) {
			root->balanceFactor_ = 0;
			pivot->balanceFactor_ = 0;
		} 
		else if (pivot->balanceFactor_ == 0) {
			root->balanceFactor_ = -1;
			pivot->balanceFactor_ = 1;
		}

		return isHeightReduced;
	}
	bool rotate_right_left(Node* root) {
		Node* pivot = root->right_;
		Node* secondary = pivot->left_;

		//resassign nodes
		Node* secondaryLeft = secondary->left_;
		Node* secondaryRight = secondary->right_;

		if (root->parent_) {
			(root->key < root->parent_->key) ? root->parent_->left_ = secondary : root->parent_->right_ = secondary;
		}
		else {
			this->root_ = secondary;
		}
		secondary->parent_ = root->parent_;

		secondary->left_ = root;
		root->parent_ = secondary;

		secondary->right_ = pivot;
		pivot->parent_ = secondary;

		root->right_ = secondaryLeft;
		if (secondaryLeft) {
			secondaryLeft->parent_ = root;
		}

		pivot->left_ = secondaryRight;
		if (secondaryRight) {
			secondaryRight->parent_ = pivot;
		}

		//set balance factors.
		if (secondary->balanceFactor_ == -1) {
			root->balanceFactor_ = 1;
			pivot->balanceFactor_ = 0;
			secondary->balanceFactor_ = 0;
		}
		else if (secondary->balanceFactor_ == 0) {
			root->balanceFactor_ = 0;
			pivot->balanceFactor_ = 0;
			secondary->balanceFactor_ = 0;
		}
		else {
			root->balanceFactor_ = 0;
			pivot->balanceFactor_ = -1;
			secondary->balanceFactor_ = 0;
		}

		return true;
	}
	bool rotate_right(Node* root) {
		Node* const pivot = root->left_;
		bool isHeightReduced = (pivot->balanceFactor_ == 1);

		//rearrange nodes.
		Node* pivotRight = pivot->right_;

		if (root->parent_) {
			(root->key < root->parent_->key) ? root->parent_->left_ = pivot : root->parent_->right_ = pivot;
		}
		else {
			this->root_ = pivot;
		}
		pivot->parent_ = root->parent_;

		pivot->right_ = root;
		root->parent_ = pivot;

		root->left_ = pivotRight;
		if (pivotRight) {
			pivotRight->parent_ = root;
		}

		//set balance factors.

		if (pivot->balanceFactor_ == 0) {
			root->balanceFactor_ = 1;
			pivot->balanceFactor_ = -1;
		} 
		else if (pivot->balanceFactor_ == 1) {
			root->balanceFactor_ = 0;
			pivot->balanceFactor_ = 0;
		}

		return isHeightReduced;
	}
	bool rotate_left_right(Node* root) {
		Node* pivot = root->left_;
		Node* secondary = pivot->right_;

		//resassign nodes
		Node* secondaryLeft = secondary->left_;
		Node* secondaryRight = secondary->right_;

		if (root->parent_) {
			(root->key < root->parent_->key) ? root->parent_->left_ = secondary : root->parent_->right_ = secondary;
		}
		else {
			this->root_ = secondary;
		}
		secondary->parent_ = root->parent_;

		secondary->right_ = root;
		root->parent_ = secondary;

		secondary->left_ = pivot;
		pivot->parent_ = secondary;

		pivot->right_ = secondaryLeft;
		if (secondaryLeft) {
			secondaryLeft->parent_ = pivot;
		}

		root->left_ = secondaryRight;
		if (secondaryRight) {
			secondaryRight->parent_ = root;
		}

		//set balance factors.
		if (secondary->balanceFactor_ == -1) {
			root->balanceFactor_ = 0;
			pivot->balanceFactor_ = 1;
			secondary->balanceFactor_ = 0;
		}
		else if (secondary->balanceFactor_ == 0) {
			root->balanceFactor_ = 0;
			pivot->balanceFactor_ = 0;
			secondary->balanceFactor_ = 0;
		}
		else {
			root->balanceFactor_ = -1;
			pivot->balanceFactor_ = 0;
			secondary->balanceFactor_ = 0;
		}

		return true;
	}
	// Decides which rotation to do depending on the balance factors of the root and its children.
	bool decide_and_do_rotation(Node* root) {
		if (root->balanceFactor_ == -2) {
			if (root->right_->balanceFactor_ == 1) {
				return this->rotate_right_left(root);
			}
			else {
				return this->rotate_left(root);
			}
		}
		else {
			if (root->left_->balanceFactor_ == -1) {
				return this->rotate_left_right(root);
			}
			else {
				return this->rotate_right(root);
			}
		}
	}

	// Tail recursive method that balances all parents of the inserted node.
	void balance_parents_after_insert(Node* parent, const KeyType& insertedKey) {
		const int bfChange = (insertedKey < parent->key) ? 1 : -1; // If insert was to the left root bfChange = +1 else -1
		parent->balanceFactor_ += bfChange;
		if (parent->balanceFactor_ == 0) {
			return;
		}
		else if ((parent->balanceFactor_ == 2) || (parent->balanceFactor_ == -2)) {
			this->decide_and_do_rotation(parent);
		}
		else {
			if (parent->parent_) {
				this->balance_parents_after_insert(parent->parent_, insertedKey);
			}
		}
	}
	// Tail recursive method that balances all parents of the removed node.
	void balance_parents_after_remove(Node* parent, const KeyType& removedKey) {
		const int bfChange = (removedKey <= parent->key) ? -1 : 1; // If pivot is left of root BF change = -1 else +1
		parent->balanceFactor_ += bfChange;
		if ((parent->balanceFactor_ == -1) || (parent->balanceFactor_ == 1)) {
			return;
		}
		else if ((parent->balanceFactor_ == 2) || (parent->balanceFactor_ == -2)) {
			Node* grandParent = parent->parent_;
			if (this->decide_and_do_rotation(parent)) {
				if (grandParent) {
					this->balance_parents_after_remove(grandParent, removedKey);
				}
			}
		}
		else {
			if (parent->parent_) {
				this->balance_parents_after_remove(parent->parent_, removedKey);
			}
		}
	}

	// Inserts the node into the tree at the specified parent and does the rebalancing.
	void insert_node_at(Node* parent, Node* node) {
		node->parent_ = parent;
		if (node->key < parent->key) {
			parent->left_ = node;
		}
		else {
			parent->right_ = node;
		}
		this->balance_parents_after_insert(parent, node->key);
	}

	// @return nullptr if key is already in the tree.
	static Node* find_parent_for_key_in_subtree(const KeyType& key, Node* root) {
		if (root->key == key) {
			return nullptr;
		}
		else if (key < root->key) {
			if (!root->left_) {
				return root;
			}
			else {
				return find_parent_for_key_in_subtree(key, root->left_);
			}
		}
		else {
			if (!root->right_) {
				return root;
			}
			else {
				return find_parent_for_key_in_subtree(key, root->right_);
			}
		}
	}

	static Node* find_min_in_subtree(Node* root) {
		Node* smallestNode = root;
		while (smallestNode->left_) {
			smallestNode = smallestNode->left_;
		}
		return smallestNode;
	}
	static Node* find_max_in_subtree(Node* root) {
		Node* largestNode = root;
		while (largestNode->right_) {
			largestNode = largestNode->right_;
		}
		return largestNode;
	}

	// Tail recursive method that searches a subtree for key.
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
		else if (key > node->key) {
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

	// Recursive method that copies a subtree to destination. 
	// May overflow the stack if used on trees too big. Too bad.
	// Used by the copy constructor and the copy assign operator.
	static void clone_subtree(Node* destinationParent, Node*& destination, Node*& source) {
		destination = new Node(*source);
		destination->parent_ = destinationParent;
		if (source->left_)
			clone_subtree(destination, destination->left_, source->left_);
		if (source->right_)
			clone_subtree(destination, destination->right_, source->right_);
	}

	Node* root_;
};