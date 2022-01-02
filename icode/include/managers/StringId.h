/*
 * StringId.h
 *
 *  Created on: 7 oct. 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_RUNTIME_STRINGID_H_
#define ICODE_INCLUDE_RUNTIME_STRINGID_H_
#include "../core/core.h"
class StringId: public AVLNode {
public:
	static int CompareKey(const AVLNode *node, const void *key, void *userData);
	static int CompareNode(const AVLNode *node1, const AVLNode *node2,
			void *userData);
	static StringId* CreateStringId(const char *name, int length);
	wushort ref;
	wushort length;
	char name[0];
};

#endif /* ICODE_INCLUDE_RUNTIME_STRINGID_H_ */
