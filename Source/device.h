/* ==============================================
 * device.h
 * You MUST COMPILE in C++
 * Hyeonsoo Kim
 * ==============================================
 */

#ifndef _DEV_H
#define _DEV_H

#include "./include_list.h"
#include "./fpga_dot_font.h"
#define NUM_OF_DEVICE 5

/* =============================================
 * Device Number
 *  0 : LED_DEVICE
 *  1 : SEG_DEVICE
 *  2 : PUSH_SWITCH_DEVICE
 *  3 : LCD_DEVICE
 *  4 : DOT_DEVICE
 *  5 : BLUETOOTH
 * =============================================
 */

#define LED_DEVICE "/dev/fpga_led"
#define SEG_DEVICE "/dev/fpga_fnd"
#define PUSH_SW_DEVICE "/dev/fpga_push_switch"
#define LCD_DEVICE "/dev/fpga_text_lcd"
#define DOT_DEVICE "/dev/fpga_dot"
#define BLUETOOTH "/dev/rfcomm0"

#define MAX_BUTTON 9

/* ============================= Device ============================ */
const char *device_list[6] = {LED_DEVICE, SEG_DEVICE, PUSH_SW_DEVICE, LCD_DEVICE, DOT_DEVICE, BLUETOOTH};
int dev[6];
//------------------------------------------------------------------------
// Push switch status buffer
// If switch is on, Value of switch buffer is non-zero.

unsigned char push_sw_buff[MAX_BUTTON] = {0,0,0,0,0,0,0,0,0};
int buff_size = sizeof(push_sw_buff);

//------------------------------------------------------------------------
int device_init(void) {
    // Device Initialization
    unsigned int i;
    for (i = 0U ; i< NUM_OF_DEVICE ; i++) {
        dev[i] = open(device_list[i], O_RDWR);
        if ( dev[i] <  0 ) {
            printf("Device %s : Cannot Connect", device_list[i]);
            return 0; // Device open Error!
            }
        }
    return 1; // Device is initialized normally.
    }

int device_close(void) {
    // Device Initialization
    unsigned int i;
    for (i = 0U ; i< NUM_OF_DEVICE ; i++) {
        close(dev[i]);
        }
    return 1; // Device is initialized normally.
    }
// -----------------------------------------------------------
// Functions for Push-Switch in FPGA

int sw_update(void) {
    if (dev[2] < 0 ) {
        printf("Cannot Initialize Switch\n");
        return 0;
        }
    else
        read(dev[2], &push_sw_buff, buff_size);
    return 1;
    }

unsigned char sw_status_partially( unsigned int sw_num ) {
    // This is Function reading status of push switches in FPGA
    // 1 <= (sw_num) <= 9
    unsigned char result = 0;
    if ( (unsigned int) sw_num >= MAX_BUTTON )
        return result;
    else if ( push_sw_buff[sw_num-1] > 0 )
        result = 1;
    else
        return result;
    return result;
    // "result == 0" means switch is off or 'sw_num' is UNACCPTABLE number
    // "result == 1" means switch (sw_num) is pushed.
    }

unsigned short sw_status() {
	unsigned short result = 0;
	// ==================================================================================
	// result is bit-wise status 
	//              ---------------------------------------------
	//    result =  0 ... 0 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | ( binary-bit-pattern )
	//              ---------------------------------------------
	//    Ex. result == 0x011 == 0000 0001 0001 : push switch 5,1 is pushed.
	//    Ex. result == 0x3FF == 0011 1111 1111 : Illegal Bit -> error. 
	// ===================================================================================   
	unsigned short i;
	for (i = 0; i <9 ; i++) {
		if (push_sw_buff[i] > 0) result |= (1 << i);
		}
	return result;
	}	

int sw_clear_partially( int sw_num ) {
    // This is Function write 0 into sw_buffer partially
    // sw_clear(x+1) equivalent to "push_switch_buff[x] = 0"

    // Error : Illegal Switch number or Switch is not opened ... return -1 as error code.
    if ( (unsigned int) sw_num >= MAX_BUTTON || dev[2] < 0 )
        return 0;

    // NO Error : clean buffer.
    push_sw_buff[sw_num-1] = 0;
    return 1;
    }


int sw_clear( unsigned short *s ) {
	// Clear status for each switchs
	// Read Bit-Pattern repersents status of switchs and clear status of switches have status of "1"
	unsigned short _status = (*s);
	(*s) = 0;
	printf("status recoed init\n");
	short i;
	for (i = 15; i>=0 ; i--) {
		if ( ( _status & ( 1 << (unsigned short) i ) ) ) {
			if (i >=9)
				return 0; // Illegal Bit. return FALSE( == 0 )
			else if ( !sw_clear_partially( i+1 ) ) 
				return 0; // Error to clear. return FALSE( == 0) ;
			}
	}
	return 1;
    }

int sw_clear ( unsigned short _status ) {
	return sw_clear(&_status);
	}

// 7-Segment in FPGA

int seg(const int num) {
    unsigned char seg[4];
    seg[0]=num /1000;
    seg[1]=(num %1000)/100;
    seg[2]=(num %100)/10;
    seg[3]=num %10;
    return write(dev[1],&seg,4);
    }

int seg_clear() {
	return seg(0);
	}

// LED 
// -----------------------------------------------------------------------------
// Data is constructed by Bits
// LCD Placements
//          -------------------------
//          | (1) | (2) | (3) | (4) |
//          -------------------------
//          | (5) | (6) | (7) | (8) |
//          -------------------------
//
//         --- 1 ( LED is ON  )
//  (X) = -|
//         --- 0 ( LED is OFF )
//  Ex. data = 1111 0000 : LED (1) ~ (4) ON and LED (5) ~ (8) OFF
//  Ex. data = 1010 1010 : LED (1), (3), (5), (7) ON, Others OFF
// -----------------------------------------------------------------------------
const unsigned char led_data[9] = {0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF,0x00};

int led_set( const unsigned char status ) {
	return write(dev[0], &status, 1);
	}

int is_on(const int x){
	unsigned char led_status = 0x00;
	if ( read(dev[0], (char*)&led_status, 1) < 0 ) return -1;
	if ( led_status  & ( 1 << x ) ) return 1;
	else return 0;
	}

int led_all_off(void) {
	return led_set(0x00);
	}  


// Text LCD;
int display_lcd(const char* str){
	char string[33];
	size_t length;

	if (!str) length = 0;
	else length = strlen(str);
	printf("%d characters displayed!\n",length);
	
	memset(string, 0x20, 32);

	if(str) strncpy(string, str, length);

   	string[32] = 0;

	if ( write(dev[3], string, 32) <0 ) return -1;
	return 1;

	}

int text_clear() {
	return display_lcd((const char*) 0);
}


//Dot Matrix
int show_dot_matrix( const unsigned char *C , const size_t sz_of_c) {
	if (!C || dev[4] < 0) return 0;
	return write(dev[4], C, sz_of_c);
	}

int show_dot_mem_num( const int _n ) {
	return show_dot_matrix( fpga_number[_n], sizeof(fpga_number[1]) );
	} 

int set_dot_blank() {
	return show_dot_matrix( fpga_set_blank, sizeof(fpga_set_blank));
	}

//===============================================================================

int all_clear() {
	 sw_clear(0x1FF);
	 text_clear();
	 set_dot_blank();
	 seg(0);
	 return 0;
	}
#endif
