/*
 * AVLNode.cpp
 *
 *  Created on: 18 août 2021
 *      Author: azeddine
 */
#include "../icode.h"

AVLNode::AVLNode() {
	memset(this, 0, sizeof(AVLNode));
}

AVLNode::~AVLNode() {
}
void AVLNode::Recalculate() {
	int left_height, right_height;
	if (this->left == 0) {
		left_height = 1;
	} else {
		left_height = this->left->height + 1;
	}
	if (this->right == 0) {
		right_height = 1;
	} else {
		right_height = this->right->height + 1;
	}
	this->height = left_height > right_height ? left_height : right_height;
}

AVLNode* AVLNode::LLRotation() {
	AVLNode *root = this->right;
	this->right = root->left;
	root->left = this;
	//update parent
	if (this->right != 0) {
		this->right->parent = this;
	}
	root->parent = this->parent;
	this->parent = root;
	this->Recalculate();
	root->Recalculate();
	return root;
}

AVLNode* AVLNode::RRRotation() {
	AVLNode *root = this->left;
	this->left = root->right;
	root->right = this;
	//update parent
	if (this->left != 0) {
		this->left->parent = this;
	}
	root->parent = this->parent;
	this->parent = root;
	this->Recalculate();
	root->Recalculate();
	return root;
}

AVLNode* AVLNode::RLRotation() {
	AVLNode *t, *root;
	t = this->right;
	root = t->left;
	t->left = root->right;
	root->right = t;
	t->parent = root;
	this->right = root->left;
	root->left = this;
	//update parent
	if (t->left != 0) {
		t->left->parent = t;
	}
	if (this->right != 0) {
		this->right->parent = this;
	}
	root->parent = this->parent;
	this->parent = root;
	t->Recalculate();
	this->Recalculate();
	root->Recalculate();
	return root;
}

AVLNode* AVLNode::LRRotation() {
	AVLNode *t, *root;
	t = this->left;
	root = t->right;
	t->right = root->left;
	root->left = t;
	t->parent = root;
	this->left = root->right;
	root->right = this;
	if (t->right != 0) {
		t->right->parent = t;
	}
	if (this->left != 0) {
		this->left->parent = this;
	}
	root->parent = this->parent;
	this->parent = root;
	t->Recalculate();
	this->Recalculate();
	root->Recalculate();
	return root;
}

int AVLNode::GetBalance() {
	int balance = 0;
	if (this->right) {
		balance += this->right->height;
	}
	if (this->left) {
		balance -= this->left->height;
	}
	return balance;
}

AVLNode* AVLNode::Balance() {
	int bal = this->GetBalance();
	int bal2;
	if (bal <= -2) {
		bal2 = this->left->GetBalance();
		if (bal2 == +1) {
			return this->RLRotation();
		} else {
			return this->RRRotation();
		}
	} else if (bal >= +2) {
		bal2 = this->right->GetBalance();
		if (bal2 == -1) {
			return this->LRRotation();
		} else {
			return this->LLRotation();
		}
	}
	return this;
}
void AVLList::Recalculate() {
	int left_height, right_height;
	int size = 1;
	if (this->left == 0) {
		left_height = 1;
	} else {
		left_height = this->left->height + 1;
		size += this->left->size;
	}
	if (this->right == 0) {
		right_height = 1;
	} else {
		right_height = this->right->height + 1;
		size += this->right->size;
	}
	this->height = left_height > right_height ? left_height : right_height;
	this->size = size;
}

AVLNode* AVLNode::GetFirst() {
	AVLNode *p = 0;
	AVLNode *node = this;
	while (node) {
		p = node;
		node = node->left;
	}
	return p;
}

AVLNode* AVLNode::GetLast() {
	AVLNode *p = 0;
	AVLNode *node = this;

	while (node) {
		p = node;
		node = node->right;
	}
	return p;
}

AVLNode* AVLNode::GetNext() {
	AVLNode *n = this, *p;
	if (n->right) {
		n = n->right;
		while (n->left) {
			n = n->left;
		}
	} else {
		p = n->parent;
		while (p && n == p->right) {
			n = p;
			p = p->parent;
		}
		n = p;
	}
	return n;
}

AVLNode* AVLNode::GetPrev() {
	AVLNode *n = this, *p;
	if (n->left) {
		n = n->left;
		while (n->right) {
			n = n->right;
		}
	} else {
		p = n->parent;
		while (p && n == p->left) {
			n = p;
			p = p->parent;
		}
		n = p;
	}
	return n;
}

AVLNode* AVLNode::InsertNode(AVLNode *node, CompareNode compare,
		void *userData) {
	AVLNode *node_original = node;
	AVLNode *p = 0, *cur, *root = this;
	int cmp;

	cur = this;
	while (cur) {
		cmp = compare(cur, node, userData);
		p = cur;
		if (cmp > 0) {
			cur = cur->left;
		} else if (cmp < 0) {
			cur = cur->right;
		} else {
			return cur;
		}
	}
	node->parent = p;
	if (p) {
		if (compare(p, node, userData) > 0) {
			p->left = node;

		} else {
			p->right = node;
		}
	} else {
		root = node;
	}
	while (node) {
		p = node->parent;
		if (p) {
			if (p->right == node) {
				node = node->Balance();
				p->right = node;
			} else {
				node = node->Balance();
				p->left = node;
			}
		} else if (node == root) {
			root = root->Balance();
			break;
		}
		node = p;
	}
	return root;
}

AVLNode* AVLNode::RemoveNode(AVLNode *node) {
	if (node->left == 0 && node->right == 0) {
		AVLNode *p = node->parent;
		if (p->left == node)
			p->left = 0;
		else
			p->right = 0;
	} else if (node->left == 0) {
		AVLNode *p = node->parent;
		if (p->left == node)
			p->left = node->right;
		else
			p->right = node->right;
	} else if (node->right == 0) {
		AVLNode *p = node->parent;
		if (p->left == node)
			p->left = node->left;
		else
			p->right = node->left;
	} else {
		AVLNode *succ = node->right;
		AVLNode *psucc = succ;
		while (1) {
			if (psucc == 0)
				break;
			succ = psucc;
			psucc = psucc->left;
		}
		psucc = succ->parent;
		psucc->left = succ->right;
		succ->left = node->left;
		succ->right = node->right;
		AVLNode *p = node->parent;
		if (p->left == node) {
			p->left = succ;
		} else {
			p->right = succ;
		}
		succ->parent = node->parent;
	}
	node->parent->Recalculate();
	return node->parent->Balance();
}

int AVLNode::CompareString(const char *s1, const char *s2) {
	while (1) {
		if (*s1 > *s2)
			return 1;
		if (*s1 < *s2)
			return -1;
		if (*s1 == 0)
			break;
		s1++;
		s2++;
	}
	return 0;
}
int ComparePtrKey(const AVLNode *node, const void *key, void *userData){
	const AVLPtrKey<int> *node1 = static_cast<const AVLPtrKey<int>*>(node);
	if((intptr_t)node1->key > (intptr_t)key) return 1;
	if((intptr_t)node1->key < (intptr_t)key) return -1;
	return 0;
}
int ComparePtrNode(const AVLNode *node1, const AVLNode *node2,
		void *userData){
	const AVLPtrKey<int> *_node2 = static_cast<const AVLPtrKey<int>*>(node2);
	return ComparePtrKey(node1, _node2->key, userData);
}
AVLNode* AVLNode::FindNode(const void *key, CompareKey compare,
		void *userData) {
	AVLNode *p = this;
	int cmp;

	while (p) {
		cmp = compare(p, key, userData);
		if (cmp > 0) {
			p = p->left;
		} else if (cmp < 0) {
			p = p->right;
		} else {
			return p;
		}
	}
	return 0;
}

AVLNode* AVLNode::FindNodeGreater(const void *key, CompareKey compare,
		void *userData) {
	AVLNode *p = this;
	AVLNode *pp = 0;
	int cmp;

	while (p) {
		cmp = compare(p, key, userData);
		pp = p;
		if (cmp > 0) {
			p = p->left;
		} else if (cmp < 0) {
			p = p->right;
		} else {
			return p;
		}
	}
	if (!pp) {
		return pp;
	}

	cmp = compare(pp, key, userData);
	if (cmp > 0) {
		return pp;
	} else {
		return pp->GetNext();
	}
}

AVLNode* AVLNode::FindNodeSmaller(const void *key, CompareKey compare,
		void *userData) {
	AVLNode *p = this;
	AVLNode *pp = 0;
	int cmp;

	while (p) {
		cmp = compare(p, key, userData);
		pp = p;
		if (cmp > 0) {
			p = p->left;
		} else if (cmp < 0) {
			p = p->right;
		} else {
			return p;
		}
	}
	if (!pp) {
		return pp;
	}
	cmp = compare(pp, key, userData);
	if (cmp < 0) {
		return pp;
	} else {
		return pp->GetPrev();
	}
}

AVLList* AVLList::GetAt(size_t index) {
	AVLList *n = this;
	while (1) {
		int leftSize;
		if (n->left == 0)
			leftSize = 0;
		else {
			leftSize = n->left->size;
		}
		if (index < leftSize) {
			n = (AVLList*)n->left;
		} else if (index > leftSize) {
			index -= leftSize + 1;
			n = (AVLList*)n->right;
		} else {
			return n;
		}
	}
}

AVLList* AVLList::InsertAt(AVLList *node, size_t index) {
	if (this == 0)
		return node;
	int leftSize;
	if (this->left == 0) {
		leftSize = 0;
	} else {
		leftSize = this->left->size;
	}
	if (index <= leftSize) {
		this->left = reinterpret_cast<AVLList*>(this->left)->InsertAt(node,
				index);
		this->left->parent = this;
	} else {
		this->right = reinterpret_cast<AVLList*>(this->right)->InsertAt(node,
				index - leftSize - 1);
		this->right->parent = this;
	}
	this->Recalculate();
	return (AVLList*)this->Balance();
}

AVLList* AVLList::RemoveAt(size_t index, AVLList **toDelete) {
	int leftSize;
	if (this->left == 0) {
		leftSize = 0;
	} else {
		leftSize = this->left->size;
	}
	if (index < leftSize)
		this->left = reinterpret_cast<AVLList*>(this->left)->RemoveAt(index,
				toDelete);
	else if (index > leftSize)
		this->right = reinterpret_cast<AVLList*>(this->right)->RemoveAt(
				index - leftSize - 1, toDelete);
	else if (this->left == 0 && this->right == 0) {
		*toDelete = this;
		return 0;
	} else if (this->left != 0 && this->right == 0) {
		AVLList *result = (AVLList*)this->left;
		this->left = 0;
		*toDelete = this;
		return result;
	} else if (this->left == 0 && this->right != 0) {
		AVLList *result = (AVLList*)this->right;
		this->right = 0;
		*toDelete = this;
		return result;
	} else {
		this->right = reinterpret_cast<AVLList*>(this->right)->RemoveAt(0,
				toDelete); // Remove successor node
	}
	this->Recalculate();
	return (AVLList*)Balance();
}

size_t AVLList::GetIndex() {
	int index;
	if (this->left) {
		index = this->left->size;
	} else {
		index = 0;
	}
	AVLNode *c = this;
	AVLNode *p = this->parent;
	while (p) {
		if (c == p->right) {
			index += p->size - c->size;
		}
		c = p;
		p = p->parent;
	}
	return index;
}

/*int avl_p_node_compare(const void *node1, const void *node2,
 const intptr_t diff) {
 avl_p_node *n1 = (avl_p_node*) node1;
 avl_p_node *n2 = (avl_p_node*) node2;
 if (n2->key > n1->key)
 return 1;
 if (n2->key < n1->key)
 return -1;
 return 0;
 }
 int avl_p_key_compare(const void *node, const void *key, const intptr_t diff) {
 avl_p_node *n = (avl_p_node*) node;
 if (key > n->key)
 return 1;
 if (key < n->key)
 return -1;
 return 0;
 }
 void avl_list_recalculate(void *node, const intptr_t diff) {
 avl_node *_node = (avl_node*) ((intptr_t) node + diff);
 int left_height, right_height;
 int size = 1;
 void *n = _node->left;
 if (n == 0) {
 left_height = 1;
 } else {
 avl_node *_n = (avl_node*) ((intptr_t) n + diff);
 left_height = _n->height + 1;
 size += _n->size;
 }
 n = _node->right;
 if (n == 0) {
 right_height = 1;
 } else {
 avl_node *_n = (avl_node*) ((intptr_t) n + diff);
 right_height = _n->height + 1;
 size += _n->size;
 }
 _node->height = left_height > right_height ? left_height : right_height;
 _node->size = size;
 }
 void avl_node_recalculate(void *node, const intptr_t diff) {
 avl_node *_node = (avl_node*) ((intptr_t) node + diff);
 int left_height, right_height;
 int size = 1;
 void *n = _node->left;
 if (n == 0) {
 left_height = 1;
 } else {
 avl_node *_n = (avl_node*) ((intptr_t) n + diff);
 left_height = _n->height + 1;
 }
 n = _node->right;
 if (n == 0) {
 right_height = 1;
 } else {
 avl_node *_n = (avl_node*) ((intptr_t) n + diff);
 right_height = _n->height + 1;
 }
 _node->height = left_height > right_height ? left_height : right_height;
 }

 void* avl_node_llrotation(void *node, const intptr_t diff,
 _avl_node_recalculate recalculate) {
 avl_node *_node = (avl_node*) ((intptr_t) node + diff);
 void *root = _node->right;
 avl_node *_root = (avl_node*) ((intptr_t) root + diff);
 _node->right = _root->left;
 _root->left = node;
 //update parent
 if (_node->right != 0) {
 avl_node *_n = (avl_node*) ((intptr_t) _node->right + diff);
 _n->parent = node;
 }
 _root->parent = _node->parent;
 _node->parent = root;
 recalculate(node, diff);
 recalculate(root, diff);
 return root;
 }

 void* avl_node_rrrotation(void *node, const intptr_t diff,
 _avl_node_recalculate recalculate) {
 avl_node *_node = (avl_node*) ((intptr_t) node + diff);
 void *root = _node->left;
 avl_node *_root = (avl_node*) ((intptr_t) root + diff);
 _node->left = _root->right;
 _root->right = node;
 //update parent
 if (_node->left != 0) {
 avl_node *_n = (avl_node*) ((intptr_t) _node->left + diff);
 _n->parent = node;
 }
 _root->parent = _node->parent;
 _node->parent = root;
 recalculate(node, diff);
 recalculate(root, diff);
 return root;
 }
 void* avl_node_rlrotation(void *node, const intptr_t diff,
 _avl_node_recalculate recalculate) {
 void *t, *root;
 avl_node *_node, *_t, *_root;
 _node = (avl_node*) ((intptr_t) node + diff);
 t = _node->right;
 _t = (avl_node*) ((intptr_t) t + diff);
 root = _t->left;
 _root = (avl_node*) ((intptr_t) root + diff);
 _t->left = _root->right;
 _root->right = t;
 _t->parent = root;
 _node->right = _root->left;
 _root->left = node;
 //update parent
 if (_t->left != 0) {
 avl_node *_n = (avl_node*) ((intptr_t) _t->left + diff);
 _n->parent = t;
 }
 if (_node->right != 0) {
 avl_node *_n = (avl_node*) ((intptr_t) _node->right + diff);
 _n->parent = node;
 }
 _root->parent = _node->parent;
 _node->parent = root;
 recalculate(t, diff);
 recalculate(node, diff);
 recalculate(root, diff);
 return root;
 }

 void* avl_node_lrrotation(void *node, const intptr_t diff,
 _avl_node_recalculate recalculate) {
 void *t, *root;
 avl_node *_node, *_t, *_root;
 _node = (avl_node*) ((intptr_t) node + diff);
 t = _node->left;
 _t = (avl_node*) ((intptr_t) t + diff);
 root = _t->right;
 _root = (avl_node*) ((intptr_t) root + diff);
 _t->right = _root->left;
 _root->left = t;
 _t->parent = root;
 _node->left = _root->right;
 _root->right = node;
 if (_t->right != 0) {
 avl_node *_n = (avl_node*) ((intptr_t) _t->right + diff);
 _n->parent = t;
 }
 if (_node->left != 0) {
 avl_node *_n = (avl_node*) ((intptr_t) _node->left + diff);
 _n->parent = node;
 }
 _root->parent = _node->parent;
 _node->parent = root;
 recalculate(t, diff);
 recalculate(node, diff);
 recalculate(root, diff);
 return root;
 }
 int avl_node_get_balance(avl_node *node, const intptr_t diff) {
 int balance = 0;
 if (node->right) {
 avl_node *_n = (avl_node*) ((intptr_t) node->right + diff);
 balance += _n->height;
 }
 if (node->left) {
 avl_node *_n = (avl_node*) ((intptr_t) node->left + diff);
 balance -= _n->height;
 }
 return balance;
 }
 void* avl_node_balance(void *node, const intptr_t diff,
 _avl_node_recalculate recalculate) {
 avl_node *_node = (avl_node*) ((intptr_t) node + diff);
 int bal = avl_node_get_balance(_node, diff);
 int bal2;
 if (bal <= -2) {
 avl_node *_n = (avl_node*) ((intptr_t) _node->left + diff);
 bal2 = avl_node_get_balance(_n, diff);
 if (bal2 == +1) {
 return avl_node_rlrotation(node, diff, recalculate);
 } else {
 return avl_node_rrrotation(node, diff, recalculate);
 }
 } else if (bal >= +2) {
 avl_node *_n = (avl_node*) ((intptr_t) _node->right + diff);
 bal2 = avl_node_get_balance(_n, diff);
 if (bal2 == -1) {
 return avl_node_lrrotation(node, diff, recalculate);
 } else {
 return avl_node_llrotation(node, diff, recalculate);
 }
 }
 return node;
 }
 void* avl_node_first(void *root, const intptr_t diff) {
 void *p = 0;
 void *node = root;

 while (node) {
 p = node;
 avl_node *_node = (avl_node*) ((intptr_t) node + diff);
 node = _node->left;
 }
 return p;
 }
 void* avl_node_last(void *root, const intptr_t diff) {
 void *p = 0;
 void *node = root;

 while (node) {
 p = node;
 avl_node *_node = (avl_node*) ((intptr_t) node + diff);
 node = _node->right;
 }
 return p;

 }
 void* avl_node_next(void *node, const intptr_t diff) {
 void *n = node, *p;
 avl_node *_n = (avl_node*) ((intptr_t) n + diff);
 if (_n->right) {
 n = _n->right;
 _n = (avl_node*) ((intptr_t) n + diff);
 while (_n->left) {
 n = _n->left;
 _n = (avl_node*) ((intptr_t) n + diff);
 }
 } else {
 p = _n->parent;
 avl_node *_p = (avl_node*) ((intptr_t) p + diff);
 while (p && n == _p->right) {
 n = p;
 p = _p->parent;
 _p = (avl_node*) ((intptr_t) p + diff);
 }
 n = p;
 }
 return n;
 }

 void* avl_node_prev(void *node, const intptr_t diff) {
 void *n = node, *p;
 avl_node *_n = (avl_node*) ((intptr_t) n + diff);
 if (_n->left) {
 n = _n->left;
 _n = (avl_node*) ((intptr_t) n + diff);
 while (_n->right) {
 n = _n->right;
 _n = (avl_node*) ((intptr_t) n + diff);
 }
 } else {
 p = _n->parent;
 avl_node *_p = (avl_node*) ((intptr_t) p + diff);
 while (p && n == _p->left) {
 n = p;
 p = _p->parent;
 _p = (avl_node*) ((intptr_t) p + diff);
 }
 n = p;
 }
 return n;
 }
 void* avl_node_insert(void *root, const intptr_t diff,
 _avl_node_compare compare, void *node,
 _avl_node_recalculate recalculate) {
 void *node_original = node;
 void *p = 0, *cur;
 int cmp;

 cur = root;
 while (cur) {
 cmp = compare(cur, node, diff);
 p = cur;
 if (cmp > 0) {
 avl_node *_cur = (avl_node*) ((intptr_t) cur + diff);
 cur = _cur->left;
 } else if (cmp < 0) {
 avl_node *_cur = (avl_node*) ((intptr_t) cur + diff);
 cur = _cur->right;
 } else {
 return cur;
 }
 }
 avl_node *_node = (avl_node*) ((intptr_t) node + diff);
 avl_node *_p = (avl_node*) ((intptr_t) p + diff);
 _node->parent = p;
 if (p) {
 if (compare(p, node, diff) > 0) {
 _p->left = node;

 } else {
 _p->right = node;
 }
 } else {
 root = node;
 }
 while (node) {
 if (p) {
 _p = (avl_node*) ((intptr_t) p + diff);
 if (_p->right == node) {
 node = avl_node_balance(node, diff, recalculate);
 _p->right = node;
 } else {
 node = avl_node_balance(node, diff, recalculate);
 _p->left = node;
 }
 } else if (node == root) {
 root = avl_node_balance(root, diff, recalculate);
 break;
 }
 node = p;
 _node = _p;
 p = _node->parent;
 }
 return root;
 }
 void* avl_node_remove(void *root, const intptr_t diff, void *node,
 _avl_node_recalculate recalculate) {
 avl_node *_node = (avl_node*) ((intptr_t) node + diff);
 if (_node->left == 0 && _node->right == 0) {
 avl_node *_p = (avl_node*) ((intptr_t) _node->parent + diff);
 if (_p->left == node)
 _p->left = 0;
 else
 _p->right = 0;
 } else if (_node->left == 0) {
 avl_node *_p = (avl_node*) ((intptr_t) _node->parent + diff);
 if (_p->left == node)
 _p->left = _node->right;
 else
 _p->right = _node->right;
 } else if (_node->right == 0) {
 avl_node *_p = (avl_node*) ((intptr_t) _node->parent + diff);
 if (_p->left == node)
 _p->left = _node->left;
 else
 _p->right = _node->left;
 } else {
 void *succ = _node->right;
 void *psucc = succ;
 while (1) {
 if (psucc == 0)
 break;
 succ = psucc;
 avl_node *_succ = (avl_node*) ((intptr_t) psucc + diff);
 psucc = _succ->left;
 }
 avl_node *_succ = (avl_node*) ((intptr_t) succ + diff);
 avl_node *_psucc = (avl_node*) ((intptr_t) _succ->parent + diff);
 _psucc->left = _succ->right;
 _succ->left = _node->left;
 _succ->right = _node->right;
 avl_node *_p = (avl_node*) ((intptr_t) _node->parent + diff);
 if (_p->left == node) {
 _p->left = succ;
 } else {
 _p->right = succ;
 }
 _succ->parent = _node->parent;
 }
 recalculate(_node->parent, diff);
 void *ret = avl_node_balance(_node->parent, diff, recalculate);
 return ret;
 }
 void* avl_node_find(void *root, const intptr_t diff, _avl_key_compare compare,
 const void *key) {
 void *p = root;
 int cmp;

 while (p) {
 cmp = compare(p, key, diff);
 if (cmp > 0) {
 avl_node *_p = (avl_node*) ((intptr_t) p + diff);
 p = _p->left;
 } else if (cmp < 0) {
 avl_node *_p = (avl_node*) ((intptr_t) p + diff);
 p = _p->right;
 } else {
 return p;
 }
 }
 return 0;
 }
 void* avl_node_find_greater(void *root, const intptr_t diff,
 _avl_key_compare compare, const void *key) {
 void *p = root;
 void *pp = 0;
 int cmp;

 while (p) {
 cmp = compare(p, key, diff);
 pp = p;
 if (cmp > 0) {
 avl_node *_p = (avl_node*) ((intptr_t) p + diff);
 p = _p->left;
 } else if (cmp < 0) {
 avl_node *_p = (avl_node*) ((intptr_t) p + diff);
 p = _p->right;
 } else {
 return p;
 }
 }
 if (!pp) {
 return pp;
 }

 cmp = compare(pp, key, diff);
 if (cmp > 0) {
 return pp;
 } else {
 return avl_node_next(pp, diff);
 }
 }
 void* avl_node_find_smaller(void *root, const intptr_t diff,
 _avl_key_compare compare, const void *key) {
 void *p = root;
 void *pp = 0;
 int cmp;

 while (p) {
 cmp = compare(p, key, diff);
 pp = p;
 if (cmp > 0) {
 avl_node *_p = (avl_node*) ((intptr_t) p + diff);
 p = _p->left;
 } else if (cmp < 0) {
 avl_node *_p = (avl_node*) ((intptr_t) p + diff);
 p = _p->right;
 } else {
 return p;
 }
 }
 if (!pp) {
 return pp;
 }
 cmp = compare(pp, key, diff);
 if (cmp < 0) {
 return pp;
 } else {
 return avl_node_prev(pp, diff);
 }
 }*/
/* list */
/*void* avl_list_get(void *node, const intptr_t diff, int index) {
 void *n = node;
 while (1) {
 avl_node *_n = (avl_node*) ((intptr_t) n + diff);
 int leftSize;
 if (_n->left == 0)
 leftSize = 0;
 else {
 avl_node *_left = (avl_node*) ((intptr_t) _n->left + diff);
 leftSize = _left->size;
 }
 if (index < leftSize) {
 n = _n->left;
 } else if (index > leftSize) {
 index -= leftSize + 1;
 n = _n->right;
 } else {
 return n;
 }
 }
 }
 void* avl_list_insert(void *root, const intptr_t diff, int index, void *node,
 _avl_node_recalculate recalculate) {
 if (root == 0)
 return node;
 int leftSize;
 avl_node *_root = (avl_node*) ((intptr_t) root + diff);
 if (_root->left == 0) {
 leftSize = 0;
 } else {
 avl_node *_left = (avl_node*) ((intptr_t) _root->left + diff);
 leftSize = _left->size;
 }
 if (index <= leftSize) {
 _root->left = avl_list_insert(_root->left, diff, index, node,
 recalculate);
 avl_node *_left = (avl_node*) ((intptr_t) _root->left + diff);
 _left->parent = root;
 } else {
 _root->right = avl_list_insert(_root->right, diff, index - leftSize - 1,
 node, recalculate);
 avl_node *_right = (avl_node*) ((intptr_t) _root->right + diff);
 _right->parent = root;
 }
 recalculate(root, diff);
 void *ret = avl_node_balance(root, diff, recalculate);
 return ret;*/
/*if (root == 0)
 return node;
 void *p = 0, *cur;
 cur = root;
 int _index = index;
 while (1) {
 int leftSize;
 avl_list0 *_cur = (avl_list0*) ((intptr_t) cur + diff);
 if (_cur->left == 0) {
 leftSize = 0;
 } else {
 avl_list0 *_left = (avl_list0*) ((intptr_t) _cur->left + diff);
 leftSize = _left->size;
 }
 p = cur;
 if (_index <= leftSize) {
 avl_node0 *_cur = (avl_node0*) ((intptr_t) cur + diff);
 cur = _cur->left;
 if(cur == 0){
 _cur->left = node;
 break;
 }
 } else {
 avl_node0 *_cur = (avl_node0*) ((intptr_t) cur + diff);
 cur = _cur->right;
 if(cur == 0){
 _cur->right = node;
 break;
 }
 _index = _index - leftSize - 1;
 }
 }
 avl_node0 *_node = (avl_node0*) ((intptr_t) node + diff);
 avl_node0 *_p = (avl_node0*) ((intptr_t) p + diff);
 _node->parent = p;
 while (node) {
 if (p) {
 recalculate(node, diff);
 node = avl_node_balance(node, diff, recalculate);
 _p = (avl_node0*) ((intptr_t) p + diff);
 if (_p->right == node) {
 _p->right = node;
 } else {
 _p->left = node;
 }
 } else if (node == root) {
 recalculate(root, diff);
 root = avl_node_balance(root, diff, recalculate);
 break;
 }
 node = p;
 _node = _p;
 p = _node->parent;
 }
 return root;*/
/*}
 void* avl_list_remove(void *root, const intptr_t diff, int index,
 _avl_node_recalculate recalculate, void **toDelete) {
 int leftSize;
 avl_node *_root = (avl_node*) ((intptr_t) root + diff);
 if (_root->left == 0) {
 leftSize = 0;
 } else {
 avl_node *_left = (avl_node*) ((intptr_t) _root->left + diff);
 leftSize = _left->size;
 }
 if (index < leftSize)
 _root->left = avl_list_remove(_root->left, diff, index, recalculate,
 toDelete);
 else if (index > leftSize)
 _root->right = avl_list_remove(_root->right, diff, index - leftSize - 1,
 recalculate, toDelete);
 else if (_root->left == 0 && _root->right == 0) {
 *toDelete = root;
 return 0;
 } else if (_root->left != 0 && _root->right == 0) {
 void *result = _root->left;
 _root->left = 0;
 *toDelete = root;
 return result;
 } else if (_root->left == 0 && _root->right != 0) {
 void *result = _root->right;
 _root->right = 0;
 *toDelete = root;
 return result;
 } else {
 _root->right = avl_list_remove(_root->right, diff, 0, recalculate,
 toDelete); // Remove successor node
 }
 recalculate(root, diff);
 void *ret = avl_node_balance(root, diff, recalculate);
 return ret;
 }
 int avl_list_index(void *node, const intptr_t diff) {
 avl_node *_n = (avl_node*) ((intptr_t) node + diff);
 int index;
 if (_n->left) {
 avl_node *_left = (avl_node*) ((intptr_t) _n->left + diff);
 index = _left->size;
 } else {
 index = 0;
 }
 void *c = node;
 void *p = _n->parent;
 while (p) {
 avl_node *_p = (avl_node*) ((intptr_t) p + diff);
 if (c == _p->right) {
 avl_node *_c = (avl_node*) ((intptr_t) c + diff);
 index += _p->size - _c->size;
 }
 c = p;
 p = _p->parent;
 }
 return index;
 }*/
