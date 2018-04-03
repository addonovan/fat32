#ifndef __FILESYSTEM_H__

#define __FILESYSTEM_H__

#include <stdio.h>
#include <bootsector.h>
#include <stdbool.h>

typedef struct
{
	FILE* 			_file;

	bootsector_t 	_boot;

} filesystem_t;

bool filesystem_init( filesystem_t*, const char* file_name  );

#endif