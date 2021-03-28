/*
 * LinkedList.cpp
 *
 *  Created on: 20 févr. 2020
 *      Author: azeddine
 */

#include "../icode.h"

BasicList::BasicList() {
	this->first = 0;
	this->last = 0;
	this->size = 0;
}

BasicList::~BasicList() {
}

void BasicList::LinkFirst(BasicLink *e) {
	if (e == 0)
		return;
	BasicLink *f = first;
	e->next = f;
	e->prev = 0;
	first = e;
	if (f == 0)
		last = e;
	else
		f->prev = e;
	size++;
}

void BasicList::LinkLast(BasicLink *e) {
	if (e == 0)
		return;
	BasicLink *l = last;
	e->next = 0;
	e->prev = l;
	last = e;
	if (l == 0)
		first = e;
	else
		l->next = e;
	size++;
}

void BasicList::LinkBefore(BasicLink *e, BasicLink *succ) {
	if (succ == 0) {
		LinkLast(e);
		return;
	}
	BasicLink *pred = succ->prev;
	e->next = succ;
	e->prev = pred;
	succ->prev = e;
	if (pred == 0)
		first = e;
	else
		pred->next = e;
	size++;
}
void BasicList::LinkAfter(BasicLink *e, BasicLink *succ) {
	if (succ == 0) {
		LinkFirst(e);
		return;
	}
	BasicLink *next = succ->next;
	e->next = next;
	e->prev = succ;
	succ->next = e;
	if (next == 0)
		last = e;
	else
		next->prev = e;
	size++;
}
BasicLink* BasicList::UnlinkFirst() {
	if (this->first == 0)
		return 0;
	BasicLink *first = this->first;
	BasicLink *next = first->next;
	//first->next = 0;
	this->first = next;
	if (next == 0)
		last = 0;
	else
		next->prev = 0;
	size--;
	return first;
}

BasicLink* BasicList::UnlinkLast() {
	if (this->last == 0)
		return 0;
	BasicLink *l = this->last;
	BasicLink *prev = l->prev;
	//l->prev = 0;
	last = prev;
	if (prev == 0)
		first = 0;
	else
		prev->next = 0;
	size--;
	return l;
}

void BasicList::Unlink(BasicLink *x) {
	if (x == 0)
		return;
	BasicLink *next = x->next;
	BasicLink *prev = x->prev;

	if (prev == 0) {
		first = next;
	} else {
		prev->next = next;
		x->prev = 0;
	}

	if (next == 0) {
		last = prev;
	} else {
		next->prev = prev;
		x->next = 0;
	}
	size--;
}

BasicLink* BasicList::Get(int index) {
	if ((size_t) index >= (size_t) this->size)
		return 0;
	if (index < (size >> 1)) {
		BasicLink *x = first;
		for (int i = 0; i < index; i++)
			x = x->next;
		return x;
	} else {
		BasicLink *x = last;
		for (int i = size - 1; i > index; i--)
			x = x->prev;
		return x;
	}
}
void BasicList::Replace(BasicLink *x, BasicLink *e) {
	if (x == 0 || e == 0)
		return;
	e->next = x->next;
	e->prev = x->prev;
	if (e->next != 0) {
		e->next->prev = e;
	} else {
		this->last = e;
	}
	if (e->prev != 0) {
		e->prev->next = e;
	} else {
		this->first = e;
	}
}
BasicLink* BasicList::Set(int index, BasicLink *e) {
	BasicLink *l = Get(index);
	if (l == 0)
		return l;
	Replace(l, e);
	return l;
}
void BasicList::Add(int index, BasicLink *e) {
	if (e == 0)
		return;
	if (index == this->size)
		LinkLast(e);
	else {
		BasicLink *l = Get(index);
		LinkBefore(e, l);
	}
}
BasicLink* BasicList::Remove(int index) {
	BasicLink *l = Get(index);
	if (l == 0)
		return l;
	Unlink(l);
	return l;
}
BasicNode::BasicNode() {
}

BasicNode::~BasicNode() {
}

