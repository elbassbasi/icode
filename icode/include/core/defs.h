/*
 * defs.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_CORE_DEFS_H_
#define ICODE_INCLUDE_CORE_DEFS_H_
#include "../../../swtc/swtp.h"
#ifdef ICODEBUILD
#define ICODE_PUBLIC DLL_EXPORT
#else
#define ICODE_PUBLIC DLL_IMPORT
#endif
#endif /* ICODE_INCLUDE_CORE_DEFS_H_ */
