/*
 * AVLNode.h
 *
 *  Created on: 18 août 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_CORE_AVLNODE_H_
#define ICODE_INCLUDE_CORE_AVLNODE_H_
#include "defs.h"
/*struct avl_node {
 void *parent;
 void *left;
 void *right;
 unsigned height :8;
 unsigned size :24;
 };
 struct avl_p_node {
 avl_node node;
 void *key;
 };
 typedef void (*_avl_node_recalculate)(void *node, const intptr_t diff);
 typedef int (*_avl_node_compare)(const void *node1, const void *node2,
 const intptr_t diff);
 typedef int (*_avl_key_compare)(const void *node, const void *key,
 const intptr_t diff);
 int avl_p_node_compare(const void *node1, const void *node2,
 const intptr_t diff);
 int avl_p_key_compare(const void *node, const void *key, const intptr_t diff);
 void avl_node_recalculate(void *node, const intptr_t diff);
 void* avl_node_llrotation(void *node, const intptr_t diff,
 _avl_node_recalculate recalculate);
 void* avl_node_rrrotation(void *node, const intptr_t diff,
 _avl_node_recalculate recalculate);
 void* avl_node_rlrotation(void *node, const intptr_t diff,
 _avl_node_recalculate recalculate);
 void* avl_node_lrrotation(void *node, const intptr_t diff,
 _avl_node_recalculate recalculate);
 int avl_node_get_balance(avl_node *node, const intptr_t diff);
 void* avl_node_balance(void *node, const intptr_t diff,
 _avl_node_recalculate recalculate);
 void* avl_node_first(void *root, const intptr_t diff);
 void* avl_node_last(void *root, const intptr_t diff);
 void* avl_node_next(void *node, const intptr_t diff);
 void* avl_node_prev(void *node, const intptr_t diff);
 void* avl_node_insert(void *root, const intptr_t diff,
 _avl_node_compare compare, void *node,
 _avl_node_recalculate recalculate);
 void* avl_node_remove(void *root, const intptr_t diff, void *node);
 void* avl_node_find(void *root, const intptr_t diff, _avl_key_compare compare,
 const void *key);
 void* avl_node_find_greater(void *root, const intptr_t diff,
 _avl_key_compare compare, const void *key);
 void* avl_node_find_smaller(void *root, const intptr_t diff,
 _avl_key_compare compare, const void *key);*/
/* list */
/*void avl_list_recalculate(void *node, const intptr_t diff);
 void* avl_list_get(void *node, const intptr_t diff, int index);
 void* avl_list_insert(void *root, const intptr_t diff, int index, void *node,
 _avl_node_recalculate recalculate);
 void* avl_list_remove(void *root, const intptr_t diff, int index,
 _avl_node_recalculate recalculate, void **toDelete);
 int avl_list_index(void *node, const intptr_t diff);*/
class AVLNode {
public:
	AVLNode *parent;
	AVLNode *left;
	AVLNode *right;
	unsigned height :8;
	unsigned size :24;
public:
	static int CompareString(const char *s1, const char *s2);
	typedef int (*CompareNode)(const AVLNode *node1, const AVLNode *node2,
			void *userData);
	typedef int (*CompareKey)(const AVLNode *node, const void *key,
			void *userData);
	AVLNode();
	virtual ~AVLNode();
protected:
	virtual void Recalculate();
	AVLNode* LLRotation();
	AVLNode* RRRotation();
	AVLNode* RLRotation();
	AVLNode* LRRotation();
	int GetBalance();
	AVLNode* Balance();
public:
	AVLNode* GetFirst();
	AVLNode* GetLast();
	AVLNode* GetNext();
	AVLNode* GetPrev();
	AVLNode* InsertNode(AVLNode *node, CompareNode compare, void *userData);
	AVLNode* RemoveNode(AVLNode *node);
	void DeleteAll();
	AVLNode* FindNode(const void *key, CompareKey compare, void *userData);
	AVLNode* FindNodeGreater(const void *key, CompareKey compare,
			void *userData);
	AVLNode* FindNodeSmaller(const void *key, CompareKey compare,
			void *userData);
	template<class T>
	T* Cast() {
		return static_cast<T*>(this);
	}
};
#define FIELDTOPOINTER(FIELD,TYPE,FIELDNAME) ((TYPE*) (FIELD - ((TYPE*)0)->FIELDNAME))
class AVLList: public AVLNode {
protected:
	void Recalculate();
public:
	AVLList* GetAt(size_t index);
	AVLList* InsertAt(AVLList *node, size_t index);
	AVLList* RemoveAt(size_t index, AVLList **toDelete);
	size_t GetIndex();
};
int ComparePtrKey(const AVLNode *node, const void *key, void *userData);
int ComparePtrNode(const AVLNode *node1, const AVLNode *node2, void *userData);
template<class K>
class AVLPtrKey: public AVLNode {
public:
	AVLPtrKey() {
		this->key = 0;
	}
	K *key;
	AVLNode* Insert(AVLNode *node) {
		return InsertNode(node, ComparePtrNode, 0);
	}
	AVLNode* Find(const K *key) {
		return FindNode(key, ComparePtrKey, 0);
	}
	AVLNode* FindGreater(const K *key) {
		return FindNodeGreater(key, ComparePtrKey, 0);
	}
	AVLNode* FindSmaller(const K *key) {
		return FindNodeSmaller(key, ComparePtrKey, 0);
	}
};
/*
 template<class T>
 class _AVLNode0 {
 public:
 T *parent;
 T *left;
 T *right;
 unsigned height :8;
 unsigned size :24;
 _AVLNode0() {
 Init();
 }
 T* GetObject(const intptr_t diff) {
 return (T*) ((intptr_t) this - diff);
 }
 protected:
 void Init() {
 this->parent = 0;
 this->left = 0;
 this->right = 0;
 this->height = 0;
 this->size = 0;
 }
 static intptr_t Diff(_AVLNode0<T> *o2, T *o1) {
 return (intptr_t) o2 - (intptr_t) o1;
 }
 T* _Insert(T *root, _avl_node_compare compare, T *node,
 _avl_node_recalculate recalculate) {
 return (T*) avl_node_insert(root, Diff(this, root), compare, node,
 (_avl_node_recalculate) recalculate);
 }
 T* _Remove(T *root, T *node) {
 return (T*) avl_node_remove(root, Diff(this, root), node);
 }
 T* _Find(T *root, _avl_key_compare compare, const void *key) {
 return (T*) avl_node_find(root, Diff(this, root), compare,
 (const void*) key);
 }
 T* _FindGreater(T *root, _avl_key_compare compare, void *key) {
 return (T*) avl_node_find_greater(root, Diff(this, root), compare, key);
 }
 T* _FindSmaller(T *root, _avl_key_compare compare, const void *key) {
 return (T*) avl_node_find_smaller(root, Diff(this, root), compare, key);
 }
 T* _GetAt(T *node, int index) {
 return (T*) avl_list_get(node, Diff(this, node), index);
 }
 T* _InsertAt(T *root, int index, T *node,
 _avl_node_recalculate recalculate) {
 return (T*) avl_list_insert(root, Diff(this, root), index, node,
 recalculate);
 }
 T* _RemoveAt(T *root, int index, T **toDelete,
 _avl_node_recalculate recalculate) {
 return (T*) avl_list_remove(root, Diff(this, root), index, recalculate,
 (void**) toDelete);
 }
 int _Index(T *node) {
 return avl_list_index(node, Diff(this, node));
 }
 public:
 T* First(T *root) {
 return (T*) avl_node_first(root, Diff(this, root));
 }
 T* Last(T *root) {
 return (T*) avl_node_last(root, Diff(this, root));
 }
 T* Next(T *node) {
 return (T*) avl_node_next(node, Diff(this, node));
 }
 T* Prev(T *node) {
 return (T*) avl_node_prev(node, Diff(this, node));
 }
 };
 template<class T>
 class _AVLNode {
 public:
 T *parent;
 T *left;
 T *right;
 unsigned height :8;
 unsigned size :24;
 _AVLNode() {
 Init();
 }
 protected:
 void Init() {
 this->parent = 0;
 this->left = 0;
 this->right = 0;
 this->height = 0;
 this->size = 0;
 }
 T* _Insert(_avl_node_compare compare, T *node,
 _avl_node_recalculate recalculate) {
 return (T*) avl_node_insert(this, 0, compare, node,
 (_avl_node_recalculate) recalculate);
 }
 T* _Remove(T *node) {
 return (T*) avl_node_remove(this, 0, node);
 }
 T* _Find(_avl_key_compare compare, const void *key) {
 return (T*) avl_node_find(this, 0, compare, (const void*) key);
 }
 T* _FindGreater(_avl_key_compare compare, void *key) {
 return (T*) avl_node_find_greater(this, 0, compare, key);
 }
 T* _FindSmaller(_avl_key_compare compare, const void *key) {
 return (T*) avl_node_find_smaller(this, 0, compare, key);
 }
 T* _GetAt(int index) {
 return (T*) avl_list_get(this, 0, index);
 }
 T* _InsertAt(int index, T *node, _avl_node_recalculate recalculate) {
 return (T*) avl_list_insert(this, 0, index, node, recalculate);
 }
 T* _RemoveAt(int index, T **toDelete, _avl_node_recalculate recalculate) {
 return (T*) avl_list_remove(this, 0, index, recalculate,
 (void**) toDelete);
 }
 int _Index() {
 return avl_list_index(this, 0);
 }
 public:
 T* First() {
 return (T*) avl_node_first(this, 0);
 }
 T* Last() {
 return (T*) avl_node_last(this, 0);
 }
 T* Next() {
 return (T*) avl_node_next(this, 0);
 }
 T* Prev() {
 return (T*) avl_node_prev(this, 0);
 }
 };
 template<class T, class K>
 class AVLNode0: public _AVLNode0<T> {
 public:
 typedef int (*CompareNode)(const T *node1, const T *node2,
 const intptr_t diff);
 typedef int (*CompareKey)(const T *node, const K *key, const intptr_t diff);
 typedef void (*Recalculate)(T *node, const intptr_t diff);
 T* GetObject(const intptr_t diff) {
 return (T*) ((intptr_t) this - diff);
 }
 protected:
 CompareNode PointerNodeCompare() {
 return (CompareNode) avl_p_node_compare;
 }
 CompareKey PointerCompare() {
 return (CompareKey) avl_p_key_compare;
 }
 Recalculate DefaultRecalculate() {
 return (Recalculate) avl_node_recalculate;
 }
 public:
 T* Insert(T *root, T *node, CompareNode compare) {
 return this->_Insert(root, (_avl_node_compare) compare, node,
 avl_node_recalculate);
 }
 T* Remove(T *root, T *node) {
 return this->_Remove(root, node);
 }
 T* Find(T *root, const K *key, CompareKey compare) {
 return this->_Find(root, (_avl_key_compare) compare, key);
 }
 T* FindGreater(T *root, const K *key, CompareKey compare) {
 return this->_FindGreater(root, (_avl_key_compare) compare, key);
 }
 T* FindSmaller(T *root, const K *key, CompareKey compare) {
 return this->_FindSmaller(root, (_avl_key_compare) compare, key);
 }
 };
 template<class T, class K>
 class __AVLNode: public _AVLNode<T> {
 public:
 typedef int (*CompareNode)(const T *node1, const T *node2,
 const intptr_t diff);
 typedef int (*CompareKey)(const T *node, const K *key, const intptr_t diff);
 typedef void (*Recalculate)(T *node, const intptr_t diff);
 protected:
 CompareNode PointerNodeCompare() {
 return (CompareNode) avl_p_node_compare;
 }
 CompareKey PointerCompare() {
 return (CompareKey) avl_p_key_compare;
 }
 Recalculate DefaultRecalculate() {
 return (Recalculate) avl_node_recalculate;
 }
 public:
 T* Insert(T *node, CompareNode compare) {
 return this->_Insert((_avl_node_compare) compare, node,
 avl_node_recalculate);
 }
 T* Remove(T *node) {
 return this->_Remove(node);
 }
 T* Find(const K *key, CompareKey compare) {
 return this->_Find((_avl_key_compare) compare, (const void*) key);
 }
 T* FindGreater(const K *key, CompareKey compare) {
 return this->_FindGreater((_avl_key_compare) compare, key);
 }
 T* FindSmaller(const K *key, CompareKey compare) {
 return this->_FindSmaller((_avl_key_compare) compare, key);
 }
 };
 template<class T>
 class AVLList0: public _AVLNode0<T> {
 public:
 T* GetAt(T *root, int index) {
 return this->_GetAt(root, index);
 }
 T* InsertAt(T *root, int index, T *node) {
 return this->_InsertAt(root, index, node, avl_list_recalculate);
 }
 T* RemoveAt(T *root, int index, T **toDelete) {
 return this->_RemoveAt(root, index, toDelete, avl_list_recalculate);
 }
 int Index(T *node) {
 return this->_Index(node);
 }
 };
 template<class T>
 class __AVLList: public _AVLNode<T> {
 public:
 T* GetAt(int index) {
 return this->_GetAt(index);
 }
 T* InsertAt(int index, T *node) {
 return this->_InsertAt(index, node, avl_list_recalculate);
 }
 T* RemoveAt(int index, T **toDelete) {
 return this->_RemoveAt(index, toDelete, avl_list_recalculate);
 }
 int Index() {
 return this->_Index();
 }
 };*/
#endif /* ICODE_INCLUDE_CORE_AVLNODE_H_ */
