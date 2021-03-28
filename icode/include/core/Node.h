/*
 * LinkedList.h
 *
 *  Created on: 20 févr. 2020
 *      Author: azeddine
 */

#ifndef INCLUDE_ICODE_CORE_LINKEDLIST_H_
#define INCLUDE_ICODE_CORE_LINKEDLIST_H_

class BasicLink {
public:
	friend class BasicList;
	BasicLink *next;
	BasicLink *prev;
	template<class T>
	T& Next() {
		return static_cast<T&>(*this->next);
	}
	template<class T>
	T& Prev() {
		return static_cast<T&>(*this->prev);
	}
};

class BasicList {
public:
	BasicLink *first;
	BasicLink *last;
	int size;
public:
	BasicList();
	~BasicList();
	void LinkFirst(BasicLink *e);
	void LinkLast(BasicLink *e);
	void LinkBefore(BasicLink *e, BasicLink *next);
	void LinkAfter(BasicLink *e, BasicLink *prev);
	BasicLink* UnlinkFirst();
	BasicLink* UnlinkLast();
	void Unlink(BasicLink *x);
	void Replace(BasicLink *x, BasicLink *e);
	BasicLink* Get(int index);
	BasicLink* Set(int index, BasicLink *e);
	int GetSize() {
		return this->size;
	}
	void Add(BasicLink *e) {
		LinkLast(e);
	}
	void Add(int index, BasicLink *e);
	BasicLink* Remove(int index);
};
template<class T>
class LinkedList: protected BasicList {
public:
	class LinkedListData: public BasicLink {
	public:
		T data;
		LinkedListData() {

		}
		LinkedListData(T &data) :
				T(data) {
		}
		LinkedListData* GetNext() {
			return (LinkedListData*) this->next;
		}
		LinkedListData* GetPrev() {
			return (LinkedListData*) this->prev;
		}
	};
	T& Get(int index) {
		LinkedListData *data = (LinkedListData*) BasicList::Get(index);
		return data->data;
	}
	void Set(int index, T &data) {
		LinkedListData *d = (LinkedListData*) BasicList::Get(index);
		d->data = data;
	}
	T& operator[](int index) {
		return Get(index);
	}
	void Add(T &data) {
		LinkedListData *d = new LinkedListData(data);
		BasicList::Add(d);
	}
	void Add(int index, T &data) {
		LinkedListData *d = new LinkedListData(data);
		BasicList::Add(index, d);
	}
	int GetSize() {
		return BasicList::GetSize();
	}
	LinkedListData* Remove(int index) {
		return (LinkedListData*) BasicList::Remove(index);
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
		T *array = new T[this->size];
		ToArray(array, this->size);
		return array;
	}
};

class BasicNode: public BasicLink, public BasicList {
public:
	BasicNode();
	~BasicNode();
};

#endif /* INCLUDE_ICODE_CORE_LINKEDLIST_H_ */
