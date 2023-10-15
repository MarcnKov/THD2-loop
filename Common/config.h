#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

/// @brief A boolean assignment 
typedef struct
{
	bool assigned;
	bool value;
} bool_value_t;

/// @brief A float assignment
typedef struct
{
	bool assigned;
	float value;
} float_value_t;

/// @brief A double assignment
typedef struct
{
	bool assigned;
	double value;
} double_value_t;

/// @brief A int assignment
typedef struct
{
	bool assigned;
	int value;
} int_value_t;

/// @brief A short assignment
typedef struct
{
	bool assigned;
	short value;
} short_value_t;

/// @brief An unsigned int assignment
typedef struct
{
	bool assigned;
	unsigned int value;
} uint_value_t;

/// @brief An unsigned long assignment
typedef struct
{
	bool assigned;
	unsigned long int value;
} ulong_value_t;

/// @brief A unsigned short assignment
typedef struct
{
	bool assigned;
	short value;
} ushort_value_t;

#endif//CONFIG_H
