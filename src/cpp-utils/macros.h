#pragma once
#ifndef MESSMER_CPPUTILS_MACROS_H_
#define MESSMER_CPPUTILS_MACROS_H_

//TODO If possible, make classes final and destructors non-virtual or delete destructors
//TODO Use DISALLOW_COPY_AND_ASSIGN where possible

/**
 * Disallow the copy and assignment constructors of a class
 */
#define DISALLOW_COPY_AND_ASSIGN(Class)        \
  Class(const Class &rhs) = delete;            \
  Class &operator=(const Class &rhs) = delete;

/**
 * Declare a function parameter as intentionally unused to get rid of the compiler warning
 */
#define UNUSED(expr) (void)(expr)

#endif
