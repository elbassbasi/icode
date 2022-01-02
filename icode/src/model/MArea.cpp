/*
 * MArea.cpp
 *
 *  Created on: 15 avr. 2020
 *      Author: azeddine
 */

#include "MArea.h"
#include "MWindow.h"

MPart0::MPart0() {
	this->flags = 0;
	this->parentPart = 0;
	this->name = 0;
	this->weight = 0;
}

MPart0::~MPart0() {
}
MPartStack::MPartStack() {
	this->flags = MPART_TYPE_STACK;
	this->weight = 0;
	this->id = -1;
}

MPartStack::~MPartStack() {
}
ObjectRef* MPartStack::GetRef(int *tmp) {
	tmp[0] = -1;
	return new (tmp) ObjectRef();
}
void MPartStack::GetPartBounds(WRect &r) {
	if (parentPart->GetType() == MPART_TYPE_AREA) {
		((MArea*) parentPart)->GetPartBounds(r);
	} else {
		((MPartSashContainer*) parentPart)->GetChildBounds(r, this);
	}
}
void MPartStack::SetWeight(int weight) {
	if (weight < 1 || weight > 100)
		return;
	MPart0 *parent = this->parentPart;
	if (this->parentPart->GetType() == MPART_TYPE_SASHCONTAINER) {
		MPartSashContainer *c = (MPartSashContainer*) this->parentPart;
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
void MPartStack::AddView(StringId *viewId) {
	IObject *obj = ICodePlugin::Get()->GetViewManager()->CreateView(viewId);
	if (obj == 0)
		return;
	IViewPart *view = obj->QueryInterfaceT<IViewPart>();
	if (view == 0) {
		obj->Release();
		return;
	}
	WTabItem item;
	this->AppendItem(item, view->GetTitle());
	if (!item.IsOk()) {
		view->Release();
	} else {
		WControl *control = view->GetPartControl(this);
		if (control != 0) {
			item.SetControl(control);
		} else {

		}
	}
}
bool MPartStack::OnItemClose(WTabEvent &e) {
	WControl *c = e.item->GetControl();
	if (c != 0) {
		e.item->SetControl(0);
		c->Dispose();
	}
	e.item->Remove();
	return W_TRUE;
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
	if (this->first != 0) {
		delete this->first;
	}
	if (this->last != 0) {
		delete this->last;
	}
}
void MPartSashContainer::GetPartBounds(WRect &r) {
	if (parentPart->GetType() == MPART_TYPE_AREA) {
		((MArea*) parentPart)->GetPartBounds(r);
	} else {
		((MPartSashContainer*) parentPart)->GetChildBounds(r, this);
	}
}

void MPartSashContainer::GetChildBounds(WRect &r, MPart0 *child) {
	this->GetPartBounds(r);
	int first_weight = this->first->weight, last_weight = this->last->weight;
	size_t total = first_weight + last_weight;
	if (total == 0)
		total = 1;
	if (this->first == child) {
		if (IsVertical(this->flags)) {
			r.width = (first_weight * (r.width - 3)) / total;
		} else {
			r.height = (first_weight * (r.height - 3)) / total;
		}
	} else {
		if (IsVertical(this->flags)) {
			int width = r.width;
			r.width = (last_weight * (width - 3) + total - 1) / total;
			//r.x += r.width + 3;
			r.x += width - r.width;
		} else {
			int height = r.height;
			r.height = (last_weight * (height - 3) + total - 1) / total;
			//r.y += r.height + 3;
			r.y += height - r.height;
		}
	}
}
void MPartSashContainer::SetWeight(wuint64 first, wuint64 last) {
	wuint64 total = first + last;
	this->first->weight = ((first << 16) + total - 1) / total;
	this->last->weight = ((last << 16) + total - 1) / total;
}
void MPartSashContainer::UpdateBounds(WRect &r) {
	WRect rect;
	wuint64 left_weight = this->first->weight;
	wuint64 right_weight = this->last->weight;
	wuint64 total = left_weight + right_weight;
	if (total == 0)
		total = 1;
	rect.x = r.x;
	rect.y = r.y;
	if (IsVertical(this->flags)) {
		rect.width = (left_weight * (r.width - 3)) / total;
		rect.height = r.height;
	} else {
		rect.width = r.width;
		rect.height = (left_weight * (r.height - 3)) / total;
	}
	this->first->UpdateBounds(rect);
	if (IsVertical(this->flags)) {
		rect.x += rect.width;
		this->SetBounds(rect.x, rect.y, 3, rect.height);
		rect.x += 3;
		rect.width = (right_weight * (r.width - 3)) / total;
	} else {
		rect.y += rect.height;
		this->SetBounds(rect.x, rect.y, rect.width, 3);
		rect.y += 3;
		rect.height = (right_weight * (r.height - 3)) / total;
	}
	this->last->UpdateBounds(rect);
}
bool MPartSashContainer::OnSelection(WSashEvent &e) {
	WRect sashBounds, area;
	this->GetBounds(sashBounds);
	this->GetPartBounds(area);
	wint64 shift, width, width1, width2, total;
	if (IsVertical(this->flags)) {
		shift = e.bounds.x - sashBounds.x;
		width = area.width;
	} else {
		shift = e.bounds.y - sashBounds.y;
		width = area.height;
	}
	if (shift != 0) {
		wuint64 left_weight = this->first->weight;
		wuint64 right_weight = this->last->weight;
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
		this->first->weight = ((width1 << 16) + width - 1) / width;
		this->last->weight = ((width2 << 16) + width - 1) / width;
		this->UpdateBounds(area);
	}
	return true;
}
MArea::MArea(MWindow *parent) {
	this->parentPart = (MPart0*) parent;
	this->flags = MPART_TYPE_AREA;
	this->root = 0;
	this->sharedPart = 0;
}

MArea::~MArea() {
	if (this->root != 0) {
		delete this->root;
	}
}

MPartStack* MArea::CreateSharedPart() {
	this->sharedPart = new MPartStack();
	this->sharedPart->flags |= MPART_TYPE_SHARED;
	this->sharedPart->parentPart = this;
	this->sharedPart->weight = ((200 << 16) + 999) / 1000;
	this->sharedPart->Create(GetWindow(), W_BORDER | W_CLOSE);
	this->sharedPart->SetImageList(ICodePlugin::Get()->GetImageList16());
	this->root = this->sharedPart;
	return this->sharedPart;
}
MPartStack* MArea::FindPartId(MPart0 *part, int refIndex) {
	if (part->GetType() == MPART_TYPE_SASHCONTAINER) {
		MPartSashContainer *container = static_cast<MPartSashContainer*>(part);
		MPartStack *stack = FindPartId(container->first, refIndex);
		if (stack != 0)
			return stack;
		return FindPartId(container->last, refIndex);
	}
	if (part->GetType() == MPART_TYPE_STACK) {
		MPartStack *stack = static_cast<MPartStack*>(part);
		if (stack->id == refIndex)
			return stack;
		else
			return 0;
	}
	return 0;
}
void MArea::LoadFolders(PerspectiveFolder *folders, int count) {
	MPartStack *shared = CreateSharedPart();
	for (int i = 0; i < count; i++) {
		int refIndex = folders[i].refIndex;
		MPartStack *part = FindPartId(this->root, refIndex);
		if (part == 0)
			part = GetSharedPart();
		int relationShip = folders[i].relationship;
		MPartStack *c = Div(part, folders[i].ratio, relationShip);
		c->id = i;
		PerspectivePart *parts;
		int viewCount = folders[i].parts.GetCount(parts);
		for (int j = 0; j < viewCount; j++) {
			if (parts[j].type == 0) {
				c->AddView(parts[j].id);
			}
		}
	}
}
MPartStack* MArea::Div(MPartStack *parent, float ratio, int flags) {
	MPartSashContainer *node = new MPartSashContainer();
	MPartStack *tree = new MPartStack();
	MPartSashContainer *p = (MPartSashContainer*) parent->parentPart;
	if ((flags & MPART_MASK) == MPART_LEFT) {
		node->flags = (node->flags & (~MPART_MASK)) | MPART_LEFT;
		node->first = (MPartSashContainer*) tree;
		node->last = (MPartSashContainer*) parent;
	} else if ((flags & MPART_MASK) == MPART_RIGHT) {
		node->flags = (node->flags & (~MPART_MASK)) | MPART_RIGHT;
		node->first = (MPartSashContainer*) parent;
		node->last = (MPartSashContainer*) tree;
	} else if ((flags & MPART_MASK) == MPART_TOP) {
		node->flags = (node->flags & (~MPART_MASK)) | MPART_TOP;
		node->first = (MPartSashContainer*) tree;
		node->last = (MPartSashContainer*) parent;
	} else if ((flags & MPART_MASK) == MPART_BOTTOM) {
		node->flags = (node->flags & (~MPART_MASK)) | MPART_BOTTOM;
		node->first = (MPartSashContainer*) parent;
		node->last = (MPartSashContainer*) tree;
	}
	node->weight = parent->weight;
	if (p->GetType() == MPART_TYPE_SASHCONTAINER) {
		if (p->first == parent)
			p->first = (MPartSashContainer*) node;
		else
			p->last = (MPartSashContainer*) node;
	} else {
		this->root = node;
	}
	tree->parentPart = node;
	parent->parentPart = node;
	node->parentPart = p;
	int style;
	if (IsVertical(flags)) {
		style = W_VERTICAL;
	} else {
		style = W_HORIZONTAL;
	}
	node->Create(GetWindow(), style);
	node->SetData(node);
	//node->SetSelectionAction(GetWindow(), W_ACTION(MWindow::UpdateSash));
	parent->weight = ((200 << 16) + 999) / 1000;
	tree->weight = ((200 << 16) + 999) / 1000;
	tree->Create(GetWindow(), W_BORDER | W_CLOSE);
	tree->SetImageList(ICodePlugin::GetImageList16());
	return tree;
}

void MArea::GetPartBounds(WRect &r) {
	MWindow *window = GetWindow();
	window->GetAreaRect(r);
}

void MArea::UpdateBounds() {
	if (this->root != 0) {
		WRect r;
		GetPartBounds(r);
		UpdateBounds(r);
	}
}
void MArea::UpdateBounds(WRect &r) {
	if (this->root != 0) {
		this->root->UpdateBounds(r);
	}
}
