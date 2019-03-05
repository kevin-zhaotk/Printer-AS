/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _HP_GENERIC_MACROS_H_
#define _HP_GENERIC_MACROS_H_

#include <stddef.h>

#ifndef MIN
#define MIN(x, y)    ((x) < (y) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x, y)                   ((x) > (y) ? (x) : (y))
#endif

#define ARRAY_SIZE(x)               (sizeof(x) / sizeof(x[0]))

/*
 * Given a pointer ("ifc_p") to a struct containing a pointer named
 * vtbl_p that points to a struct containing a function pointer
 * named "member", IFC_CALL calls function "member".  Use of the macro must
 * be followed with the parameter list expected by the function, and
 * IFC_CALL returns the function return value.
 */
#define IFC_CALL(ifc_p, member)     (*((ifc_p)->vtbl_p->member))

/*
 * Given a "pointer" to a named "member" of a struct of type "class", IMPL
 * returns a pointer to the struct.
 */

#define IMPL(class, member, pointer) \
    ((class *) (((uint32_t) pointer) - offsetof(class, member)))
#endif /* _HP_GENERIC_MACROS_H_ */
