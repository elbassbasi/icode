/*
 * MArea.cpp
 *
 *  Created on: 15 avr. 2020
 *      Author: azeddine
 */

#include "MArea.h"
#include "MPerspective.h"

MPart0::MPart0() {
	this->flags = 0;
	this->parent = 0;
	this->name = 0;
}

MPart0::~MPart0() {
}
MPartStack::MPartStack() {
	this->flags = MPART_TYPE_STACK;
	this->weight = 0;
}

MPartStack::~MPartStack() {
}
void MPartStack::GetPartBounds(WRect &r) {
	if (parent->GetType() == MPART_TYPE_AREA) {
		((MArea*) parent)->GetPartBounds(r);
	} else {
		((MPartSashContainer*) parent)->GetChildBounds(r, this);
	}
}
void MPartStack::SetWeight(int weight) {
	if (weight < 1 || weight > 100)
		return;
	MPart0 *parent = this->parent;
	if (this->parent->GetType() == MPART_TYPE_SASHCONTAINER) {
		MPartSashContainer *c = (MPartSashContainer*) this->parent;
		int first, last;
		if (c->first == this) {
			first = weight;
			last = 100 - weight;
		} else {
			first = 100 - weight;
			last = weight;
		}
		c->SetWeight(first, last);
	}
}

void MPartStack::AddView(const char *viewId) {
	IViewPart* view = ICodePlugin::Get()->CreateView(viewId);
	if(view == 0) return;
	WTabItem item;
	this->AppendItem(item, view->GetTitle());
	if(!item.IsOk()){
		view->DecRef();
	}else{
		WControl* control = view->GetPartControl(this);
		if(control != 0){
			item.SetControl(control);
		}else{

		}
	}
}
void MPartStack::UpdateBounds(WRect &r) {
	this->SetBounds(r);
}
MPartSashContainer::MPartSashContainer() {
	this->flags = MPART_TYPE_SASHCONTAINER;
	this->last = 0;
	this->first = 0;
	this->weight = 0;
}

MPartSashContainer::~MPartSashContainer() {
}
void MPartSashContainer::GetPartBounds(WRect &r) {
	if (parent->GetType() == MPART_TYPE_AREA) {
		((MArea*) parent)->GetPartBounds(r);
	} else {
		((MPartSashContainer*) parent)->GetChildBounds(r, this);
	}
}

void MPartSashContainer::GetChildBounds(WRect &r, MPart0 *child) {
	this->GetPartBounds(r);
	int first_weight = GetFirstWeight(), last_weight = GetLastWeight();
	size_t total = first_weight + last_weight;
	if (total == 0)
		total = 1;
	if (this->first == child) {
		if (this->flags & MPART_VERTICAL) {
			r.width = (first_weight * (r.width - 3)) / total;
		} else {
			r.height = (first_weight * (r.height - 3)) / total;
		}

	} else {
		if (this->flags & MPART_VERTICAL) {
			r.width = (last_weight * (r.width - 3)) / total;
			r.x += r.width + 3;
		} else {
			r.height = (last_weight * (r.height - 3)) / total;
			r.y += r.height + 3;
		}
	}
}

int MPartSashContainer::GetFirstWeight() {
	if (this->first->GetType() == MPART_TYPE_SASHCONTAINER) {
		return ((MPartSashContainer*) this->first)->weight;
	} else {
		return ((MPartStack*) this->first)->weight;
	}
}

int MPartSashContainer::GetLastWeight() {
	if (this->last->GetType() == MPART_TYPE_SASHCONTAINER) {
		return ((MPartSashContainer*) this->last)->weight;
	} else {
		return ((MPartStack*) this->last)->weight;
	}
}

void MPartSashContainer::SetFirstWeight(int weight) {
	if (this->first->GetType() == MPART_TYPE_SASHCONTAINER) {
		((MPartSashContainer*) this->first)->weight = weight;
	} else {
		((MPartStack*) this->first)->weight = weight;
	}
}

void MPartSashContainer::SetLastWeight(int weight) {
	if (this->first->GetType() == MPART_TYPE_SASHCONTAINER) {
		((MPartSashContainer*) this->last)->weight = weight;
	} else {
		((MPartStack*) this->last)->weight = weight;
	}
}
void MPartSashContainer::SetWeight(int first, int last) {
	int total = first + last;
	SetFirstWeight((((long long int) first << 16) + total - 1) / total);
	SetLastWeight((((long long int) last << 16) + total - 1) / total);
}
void MPartSashContainer::UpdateBounds(WRect &r) {
	WRect rect;
	int left_weight = GetFirstWeight(), right_weight = GetLastWeight();
	size_t total = left_weight + right_weight;
	if (total == 0)
		total = 1;
	rect.x = r.x;
	rect.y = r.y;
	if (this->flags & MPART_VERTICAL) {
		rect.width = (left_weight * (r.width - 3)) / total;
		rect.height = r.height;
	} else {
		rect.width = r.width;
		rect.height = (left_weight * (r.height - 3)) / total;
	}
	if (this->first->GetType() == MPART_TYPE_SASHCONTAINER) {
		((MPartSashContainer*) this->first)->UpdateBounds(rect);
	} else {
		((MPartStack*) this->first)->UpdateBounds(rect);
	}
	if (this->flags & MPART_VERTICAL) {
		rect.x += rect.width;
		this->sash.SetBounds(rect.x, rect.y, 3, rect.height);
		rect.x += 3;
		rect.width = (right_weight * (r.width - 3)) / total;
	} else {
		rect.y += rect.height;
		this->sash.SetBounds(rect.x, rect.y, rect.width, 3);
		rect.y += 3;
		rect.height = (right_weight * (r.height - 3)) / total;
	}
	if (this->last->GetType() == MPART_TYPE_SASHCONTAINER) {
		((MPartSashContainer*) this->last)->UpdateBounds(rect);
	} else {
		((MPartStack*) this->last)->UpdateBounds(rect);
	}
}
MArea::MArea(MPerspective *parent) {
	this->parent = (MPart0*) parent;
	this->flags = MPART_TYPE_AREA;
	this->root = 0;
	this->sharedPart = 0;
}

MArea::~MArea() {
}
MPartStack* MArea::CreateSharedPart() {
	this->sharedPart = new MPartStack();
	this->sharedPart->flags |= MPART_CONTAINER_SHARED;
	this->sharedPart->parent = this;
	this->sharedPart->weight = ((200 << 16) + 999) / 1000;
	this->sharedPart->Create(GetComposite(), W_BORDER | W_CLOSE);
	this->sharedPart->SetImageList(IManagers::Get()->GetImageList());
	this->root = this->sharedPart;
	return this->sharedPart;
}
MPartStack* MArea::Div(MPartStack *parent, int flags) {
	MPartSashContainer *node = new MPartSashContainer();
	MPartStack *tree = new MPartStack();
	MPartSashContainer *p = (MPartSashContainer*) parent->parent;
	if (flags & MPART_VERTICAL) {
		node->flags |= MPART_VERTICAL;
	}
	node->weight = parent->weight;
	if (flags & MPART_LAST) {
		node->first = (MPartSashContainer*) parent;
		node->last = (MPartSashContainer*) tree;
	} else {
		node->first = (MPartSashContainer*) tree;
		node->last = (MPartSashContainer*) parent;
	}
	if (p->GetType() == MPART_TYPE_SASHCONTAINER) {
		if (p->first == parent)
			p->first = (MPartSashContainer*) node;
		else
			p->last = (MPartSashContainer*) node;
	} else {
		this->root = node;
	}
	tree->parent = node;
	parent->parent = node;
	node->parent = p;
	int style;
	if (flags & MPART_VERTICAL) {
		style = W_VERTICAL;
	} else {
		style = W_HORIZONTAL;
	}
	node->sash.Create(GetComposite(), style);
	node->sash.SetData(node);
	parent->weight = ((200 << 16) + 999) / 1000;
	tree->weight = ((200 << 16) + 999) / 1000;
	tree->Create(GetComposite(), W_BORDER | W_CLOSE);
	tree->SetImageList(IManagers::Get()->GetImageList());
	return tree;
}

void MArea::GetPartBounds(WRect &r) {
	GetPerspective()->GetAreaBounds(r);
}

void MArea::UpdateBounds(WRect &r) {
	if (this->root != 0) {
		if (this->root->GetType() == MPART_TYPE_SASHCONTAINER) {
			((MPartSashContainer*) this->root)->UpdateBounds(r);
		} else {
			((MPartStack*) this->root)->UpdateBounds(r);
		}
	}
}
WComposite* MArea::GetComposite() {
	return GetPerspective()->GetComposite();
}
bool MArea::UpdateSash(WSashEvent *event) {
	WRect sashBounds, area;
	WSash *sash = (WSash*) event->widget;
	sash->GetBounds(sashBounds);
	MPartSashContainer *node = (MPartSashContainer*) sash->GetData();
	node->GetPartBounds(area);
	int shift, width, width1, width2, total;
	if (sash->GetStyle() & W_VERTICAL) {
		shift = event->bounds.x - sashBounds.x;
		width = area.width;
	} else {
		shift = event->bounds.y - sashBounds.y;
		width = area.height;
	}
	if (shift != 0) {
		int left_weight = node->GetFirstWeight(), right_weight =
				node->GetLastWeight();
		total = left_weight + right_weight;
		if (total == 0) {
			total = 1;
		}
		width1 = (left_weight * (width - 3)) / total;
		width2 = (right_weight * (width - 3)) / total;
		width1 += shift;
		width2 -= shift;
		if (width1 < DRAG_MINIMUM) {
			width1 = DRAG_MINIMUM;
			width2 = width - width1 - 3;
		}
		if (width2 < DRAG_MINIMUM) {
			width2 = DRAG_MINIMUM;
		}
		node->SetFirstWeight(((width1 << 16) + width - 1) / width);
		node->SetLastWeight(((width2 << 16) + width - 1) / width);
		//node->UpdateBounds(area);
		GetPerspective()->UpdateLayout();
	}
	return true;
}

void MArea::test() {
	MPartStack *p1, *p2;
	p1 = CreateSharedPart();
	testControl(p1, "Shared");
	p2 = testDiv(p1, "Project", MPART_FIRST | MPART_VERTICAL);
	p2->SetWeight(20);
	p2 = testDiv(p1, "Console", MPART_LAST | MPART_HORIZENTAL);
	p2->SetWeight(20);
	p1 = testDiv(p1, "Outline", MPART_LAST | MPART_VERTICAL);
	p1->SetWeight(20);
}
void MArea::testControl(MPartStack *container, const char *name) {
	char txt[0x50];
	WTabItem item;
	WTextEdit *c;
	for (int i = 0; i < 7; i++) {
		sprintf(txt, "%s%d", name, i);
		if (i < 3) {
			c = new WTextEdit(container,
					W_MULTI | W_HSCROLL | W_VSCROLL | W_FREE_MEMORY);
			c->SetText(txt);
			container->AppendItem(item,txt);
			item.SetControl(c);
			item.SetImage(0);
		} else {
			container->AppendItem(item,txt);
			item.SetImage(1);
		}
	}
}

MPartStack* MArea::testDiv(MPartStack *parent, const char *name, int flags) {
	MPartStack *tree = Div(parent, flags);
	tree->name = name;
	testControl(tree, name);
	return tree;
}

