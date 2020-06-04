#ifndef _FCTL_
#define _FCTL_
#include "include_list.h"

unsigned int mem_file_num[6] = {0,};

int inc_filenum( unsigned int idx ) {
	if (idx > 6) return 0;
	mem_file_num[idx-1] ++ ;
	return 1;
	}

int dec_filenum( unsigned int idx ) {
	if (idx > 6) return 0;
	mem_file_num[idx-1] --;
	return 1;
	}

unsigned int get_filenum( unsigned int idx ) {
	if (idx > 6) return 0;
	return mem_file_num[idx-1];
	}


#endif

