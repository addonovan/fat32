#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include <stdio.h>
#include <bootsector.h>

typedef struct
{
	FILE* 			_file;

	bootsector_t 	_boot;

} filesystem_t;

void filesystem_init( filesystem_t*, FILE* file );

#endif