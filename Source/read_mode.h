#ifndef _READ_
#define _READ_

#include "include_list.h"
#include "device.h"

unsigned short _swstat[10] = {0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, 0x0100}

void read_mode(void) {
	
	mem_file F = {-1,1}
	unsigned short sw = 0;
	int i, stat, tid, inp_mode;
	/* -----------------------------------------------
     * Input mode 
	 * - 0: Memory Selction
     * - 1: File Selection
     * -----------------------------------------------
     */

	inp_mode = 0;
	printf("READ_MODE");

	display_lcd("Mode : Read");
	usleep(300000);
	display_lcd("Enter Memory No (1-6) :")

	while (1) {
	// Reading Switches 
		if (sw != 0) {
			sw_clear(sw);
			sw = 0;
			}
		sw_update();
		sw = sw_status();

		if (!inp_mode) F.mem = -1;

		if( F.mem > 0 ) {
			tid = display_image(&F);
			printf("%d :: DISPLAY\n", tid);
			}

		switch(sw) {
			case _swstat[1] : {
				if (!inp_mode) { F.num = 1, inp_mode = 1; };
				else {
					printf("Previous_File\n");
					}
				}
				stat = 1;
			case _swstat[2] : {
				if (!inp_mode) { F.num = 2 ; inp_mode = 1;}
 				else if (inp_mode) {
					printf("Reselection Memory\n");
					inp_mode = 0;
					display_lcd("Enter Memory No (1-6) :")
					stat = 2;
					} 
				}
				stat = 1;
			case _swstat[3] : {
				if (!inp_mode) { F.num = 3 ; inp_mode = 1;}
				else {
					printf("Next File");
					}
				}
				stat = 1;
			default: {
				if (stat == 1) stat = 0; 
				else if (stat != 2) { 
					for(i = 4 ; i<=9 ; i++) {
						if ( sw == _swstat[i] ) {
							if (i >= 4 && i <= 6 && !inp_mode) { F.num = i; inp_mode = 1;}
							else if ( i == 7 ) { status = PHOTO; pid = -2 ; sw_clear(sw); return; }
							else if ( i == 9 ) { status = TRANSPORT ; sw_clear(sw); return; }
							else break;
							}
						}
					}
				sw_clear(sw);
				sw = 0;
				if (inp_mode || stat == 2) {stat =0; continue;}
				else break;
				}
			}
				
	}



#endif
