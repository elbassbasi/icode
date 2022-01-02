/*
 * LinkedList.h
 *
 *  Created on: 20 févr. 2020
 *      Author: azeddine
 */

#ifndef INCLUDE_ICODE_CORE_LINKEDLIST_H_
#define INCLUDE_ICODE_CORE_LINKEDLIST_H_
#include "defs.h"

template<class T>
class LinkedList {
public:
	class LinkedListData: public WLink<LinkedListData> {
	public:
		T data;
		LinkedListData() {

		}
		LinkedListData(T &data) :
				T(data) {
		}
	};
protected:
	LinkedListData *first;
	size_t count;
public:
	LinkedList() {
		this->first = 0;
		this->count = 0;
	}
	LinkedListData* GetL(size_t index) {
		LinkedListData *x = first;
		for (int i = 0; i < index; i++)
			x = x->next;
		return *x;
	}
	T& Get(size_t index) {
		return GetL(index)->data;
	}
	void Set(int index, T &data) {
		LinkedListData *d = GetL(index);
		d->data = data;
	}
	T& operator[](int index) {
		return Get(index);
	}
	void Add(T &data) {
		LinkedListData *d = new LinkedListData(data);
		d->LinkLast(d, first);
		this->count++;
	}
	void Add(int index, T &data) {
		LinkedListData *d = new LinkedListData(data);
		LinkedListData *l = GetL(index);
		d->LinkBefore(d, l, first);
		this->count++;
	}
	int GetSize() {
		return count;
	}
	LinkedListData* Remove(int index) {
		LinkedListData *d = GetL(index);
		d->Unlink(d, first);
		return d;
	}
	void Delete(int index) {
		LinkedListData *d = Remove(index);
		if (d != 0)
			delete d;
	}
	void ToArray(T *array, int count) {
		LinkedListData *d = (LinkedListData*) this->first;
		for (int i = 0; i < count; i++) {
			array[i] = d->data;
			d = d->GetNext();
		}
	}
	T* ToArray() {
		T *array = new T[this->count];
		ToArray(array, this->count);
		return array;
	}
};

#endif /* INCLUDE_ICODE_CORE_LINKEDLIST_H_ */
