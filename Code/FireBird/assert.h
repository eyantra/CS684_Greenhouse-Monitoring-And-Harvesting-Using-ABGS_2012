/* File: assert.h
 * Description: Assertion headers
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

 #ifndef ASSERT_H
 #define ASSERT_H

 #ifdef NDEBUG
 #define ASSERT(x)	((void)0)
 #else
 #define ASSERT(x)	assert((x), __FILE__,__LINE__,#x)
 #endif
 
 extern void assert(UINT, char *, UINT, char *);
 
 #endif /* ASSERT_H */
 