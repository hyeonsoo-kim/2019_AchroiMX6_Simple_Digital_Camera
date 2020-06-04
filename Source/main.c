#include "include_list.h"
#include "device.h"

// ----------------------------------------------------------------
/* ==================== Process and Status =========================== */
// Process and Status
enum _States { PHOTO = 0, READ, TRANSPORT , END};
enum _States state= PHOTO ;
enum _States ps;

pid_t pid = -2;

// Signal Handler :: Terminate Child Process before Terminated Parent Process.

void terminate( int sig ) {
	all_clear();
    if (pid != 0 )
        kill(pid, 15);
    switch (sig) {
        case SIGSEGV:
            printf("ERROR! Segmentation Fault!! Check Code for memory stack and heap\n");
            break;
		case SIGTERM:
			display_lcd("Bye.. Bye...");
			printf("TERMINATED\n");
			break;
        default     :
            printf("STOPPED!!\n");
        }
    device_close();
    exit(0);
    }

// ---------------------------------------------------------------

/* ==================== Environment Variable =========================== */
// Iteration Variable
int i,j;
// ---------------------------------------------------------------

/* Time Variable
time_t cur_time;
struct tm *lt;
*/

char string[32] = {0x20,};

char *capture_command[4];

// -------------------------------------------------------------------------

/* ===================== Process Functions =============================== */

void photo_mode(void);
void read_mode(void);
void trans_mode(void);

int file_check(int mem_num);
int check_exist_files_in_mem(int);
int file_send(int, int);
int image_show(int mem_num, int seq);


//-----------------------------------------------------------------------------



int main() {
    if ( !device_init() ) {
        device_close();
        exit(-1);
        }

    // Signal Handling ------------------------------
    (void)signal(SIGINT, terminate);
    (void)signal(SIGSEGV, terminate);
    // -----------------------------------------------

    printf("Debug Shell :: press <ctrl+c> to exit\n");

    while (1) {
        switch (state) {
            case PHOTO: {
                photo_mode();
                continue;
                }
            case READ: {
                read_mode();
                continue;
                }
            case TRANSPORT: {
                trans_mode();
                continue;
                }
			case END: {
				display_lcd("Power off?     yes(1), no(2)");
				if (sw_status() == 0x0001) terminate(SIGTERM);
				else if (sw_status() == 0x0002) state=ps;
				continue;
			}
            default :
                continue;
            }
        }
    device_close();
    return 0;
    }

void photo_mode(void) {
    int mem_num=1;
    capture_command[0] = "./mxc_v4l2_still.out -f RGB565";
    if (!display_lcd("Mode : Photo")) printf("LCD Error!\n");
    sleep(1);

    while(1) {
        //initialize commands to capture
        capture_command[1] = (char*)malloc(sizeof(char)*128);
        strcpy(capture_command[1], "ffmpeg -vcodec rawvideo -pix_fmt rgb565 -s 640x480 -i still.yuv -f image2 -vcodec png ");
        capture_command[2] = (char*)malloc(sizeof(char)*25);

        //================================================
        if (pid == -2) {
            pid = fork();
            }
        // ================================= Child =====================
        switch(pid) {
            case -1: {
                printf("fork failed");
                break;
                }
            case  0: {
                execl("./mxc_v4l2_overlay.out", "mxc_v4l2_overlay.out", "-iw", "640", "-ih", "480", "-ow", "1024", "-oh", "600", "-fr", "30", (char*)0);
                printf("exec failed");
				exit(-1);
                break;
                }
            // ============================= Parents =======================
            default: {
                display_lcd("choose memory address for photo");

                while(1) {
                    if( !sw_update() ) terminate(-1);
                    if(push_sw_buff[0] > 0) {
                        mem_num = 1;
                        push_sw_buff[0] = 0;
                        if(file_check(mem_num)) {
                            write(dev[4],fpga_number[mem_num],sizeof(fpga_number[mem_num]));
                            //show_dot_mem_num(mem_num);
                            break;
                            }
                        else
                            continue;
                        }
                    else if(push_sw_buff[1] > 0) {
                        mem_num = 2;
                        push_sw_buff[1] = 0;
                        if(file_check(mem_num)) {
                            show_dot_mem_num(mem_num);
                            break;
                            }
                        else
                            continue;
                        }
                    else if(push_sw_buff[2] > 0) {
                        mem_num = 3;
                        push_sw_buff[2] = 0;
                        if(file_check(mem_num)) {
                            show_dot_mem_num(mem_num);
                            break;
                            }
                        else
                            continue;
                        }
                    else if(push_sw_buff[3] > 0) {
                        mem_num = 4;
                        push_sw_buff[3] = 0;
                        if(file_check(mem_num)) {
                            show_dot_mem_num(mem_num);
                            break;
                            }
                        else
                            continue;
                        }
                    else if(push_sw_buff[4] > 0) {
                        mem_num = 5;
                        push_sw_buff[4] = 0;
                        if(file_check(mem_num)) {
                            show_dot_mem_num(mem_num);
                            break;
                            }
                        else
                            continue;
                        }
                    else if(push_sw_buff[5] > 0) {
                        mem_num = 6;
                        push_sw_buff[5] = 0;
                        if(file_check(mem_num)) {
                            show_dot_mem_num(mem_num);
                            break;
                            }
                        else
                            continue;
                        }
                    else if (push_sw_buff[7] > 0) {
                        state = READ;
                        if (pid!=0)
                            kill(pid,9);
                        all_clear();
                        return;
                        }
                    else if (push_sw_buff[8] > 0) {
                        state = TRANSPORT;
                        if (pid!=0)
                            kill(pid,9);
                        all_clear();
                        return;
                        }
                    }
                display_lcd("Click button to take picture");
                sleep(1);
                while(1) {
                    read(dev[2], &push_sw_buff, buff_size);
                    if( push_sw_buff[0] > 0 ) {
                        kill(pid, 19);
                        sprintf(capture_command[2], "photo_%d_1.png", mem_num);
                        strcat(capture_command[1], capture_command[2]);
                        capture_command[3] = (char*)malloc(25);
                        strcpy(capture_command[3],"rm -f still.yuv");
                        for (int i = 0 ; i <=2 ; i++) {
                            if ( i !=2 )
                                system( capture_command[i]);
                            // if ( i >=1 ) free(capture_command[i]);
                            }
                        write(dev[0],&led_data[0],1);
                        seg(1);
                        kill(pid, 18);
                        push_sw_buff[0] = 0;
                        break;
                        }
                    else if (push_sw_buff[1] > 0) {
                        kill(pid, 19);
                        for(int i=0; i<8; i++) {
                            capture_command[2][0] = 0;
                            if ( i >=1 ) capture_command[1][strlen(capture_command[1]) - 13] = 0;
                            sprintf(capture_command[2], "photo_%d_%d.png", mem_num,i+1);
                            strcat(capture_command[1], capture_command[2]);
                            strcpy(capture_command[2],"rm -f still.yuv");
                            for (j = 0 ; j <3 ; j++) {
                                if ( system( capture_command[j]) >= 0 ) seg(i+1) ;
                                write(dev[0],&led_data[i],1);
                                }
                            }
                        kill(pid, 18);
                        push_sw_buff[1] = 0;
                        break;
                        }
                    else if( push_sw_buff[2] > 0 ) {
                        kill(pid, 19);
                        sprintf(capture_command[2], "photo_%d_1.png", mem_num);
                        strcat(capture_command[1], capture_command[2]);
                        capture_command[3] = (char*)malloc(25);
                        strcpy(capture_command[3],"rm -f still.yuv");
						display_lcd("3");
						sleep(1);
						display_lcd("2");
						sleep(1);
						display_lcd("1");
						sleep(1);
						display_lcd(" ");
                        for (int i = 0 ; i <=2 ; i++) {
                            if ( i !=2 )
                                system( capture_command[i]);
                            // if ( i >=1 ) free(capture_command[i]);
                            }
                        write(dev[0],&led_data[0],1);
                        seg(1);
                        kill(pid, 18);
                        push_sw_buff[0] = 0;
                        break;
                        }
                    else if (push_sw_buff[7] > 0 ) {
                        state = READ;
                        if (pid!=0)
                            kill(pid,9);
                        all_clear();
                        return;
                        }
                    else if (push_sw_buff[8] > 0 ) {
                        state = TRANSPORT;
                        if (pid!=0)
                            kill(pid,9);
                        all_clear();
                        return;
                        }
                    }
                for (i = 1; i<3 ; i++)
                    free(capture_command[i]);
                continue;
                }

            }
        }
    }

void read_mode(void) {
    int mem_num=1, pic = 1;
    int read_seq =1;
	unsigned short sw = 0;
    int str_size = 10;
    char mem_select = 0;
    char display_memselect[26] = "Enter Memory No (1-6) :  ";
    all_clear();
    char read = 1;
    printf("READ_MODE");
    display_lcd("Mode : Read");
    sleep(1);

    while(1) {
    	if ( display_memselect[24] > 0x30 ) {
			display_memselect[24] = 0x20;
            display_lcd(display_memselect);
		}
		else 
            display_lcd(display_memselect);
        while(!mem_select) {
            if (sw != 0) { sw_clear(&sw);  }
            sw_update();
            sw = sw_status();

            switch(sw) {
                case 0x0001 : {
                    mem_num = 1;
                    display_memselect[24]= mem_num + 0x30;
                    if(check_exist_files_in_mem(mem_num)) {
                        display_lcd(display_memselect);
                        show_dot_mem_num(mem_num);
                        mem_select = 1;
                        break;
                        }
                    else continue;
                    }
                case 0x0002 : {
                    mem_num = 2;
                    display_memselect[24] = mem_num + 0x30;
                    if(check_exist_files_in_mem(mem_num)) {
                        display_lcd(display_memselect);
                        show_dot_mem_num(mem_num);
                        mem_select = 1;
                        break;
                        }
                    else continue;
                    }
                case 0x0004 : {
                    mem_num = 3;
                    display_memselect[24]= mem_num + 0x30;
                    if(check_exist_files_in_mem(mem_num)) {
                        display_lcd(display_memselect);
                        show_dot_mem_num(mem_num);
                        mem_select = 1;
                        break;
                        }
                    else continue;
                    }
                case 0x0008 : {
                    mem_num = 4;
                    display_memselect[24]= mem_num + 0x30;
                    if(check_exist_files_in_mem(mem_num)) {
                        display_lcd(display_memselect);
                        show_dot_mem_num(mem_num);
                        mem_select = 1;
                        break;
                        }
                    else continue;
                    }
                case 0x0010 : {
                    mem_num = 5;
                    display_memselect[24]= mem_num + 0x30;
                    if(check_exist_files_in_mem(mem_num)) {
                        display_lcd(display_memselect);
                        show_dot_mem_num(mem_num);
                        mem_select = 1;
                        break;
                        }
                    else continue;
                    }
                case 0x0020 : {
                    mem_num = 6;
                    display_memselect[24]= mem_num + 0x30;
                    if(check_exist_files_in_mem(mem_num)) {
                        display_lcd(display_memselect);
                        show_dot_mem_num(mem_num);
                        mem_select = 1;
                        break;
                        }
                    else continue;
                    }
                default : {
                    if(sw == 0x0040) {
                        state = PHOTO;
                        pid = -2;
                        sw_clear(&sw);
                        return;
                        }
                    else if(sw == 0x0100) {
                        state = TRANSPORT;
                        sw_clear(&sw);
                        return;
                        }
                    else { read = 0; break; }
                    }
                }
            }

		if (!read) { read_seq = 1; image_show(mem_num, read_seq); read = 1;
		}
        sleep(1);
        while(1) {
            if (sw != 0) sw_clear(&sw);
            sw_update();
            sw = sw_status();
            switch(sw) {
                case 0x0001 : {
                    display_lcd("Previous File...");
					if(read_seq == 1) read_seq = 8;
					else read_seq--;
					led_set(1 << (read_seq-1));					
					image_show(mem_num, read_seq);
					break;
                    }
                case 0x0002 : {
                    display_lcd("Memory Reselection...");
                    sleep(1);
					all_clear();
                    mem_select = 0;
                    read = 1;
                    break;
                    }
                case 0x0004 : {
                    display_lcd("Next File...");
					if(read_seq == 8) read_seq = 1;
					else read_seq++;
					led_set(1 << (read_seq-1));					
					image_show(mem_num, read_seq);
					break;
                    }
                default : {
                    if(sw == 0x0040) {
                        state = PHOTO;
                        pid = -2;
                        sw_clear(&sw);
                        return;
                        }
                    else if(sw == 0x0100) {
                        state = TRANSPORT;
                        sw_clear(&sw);
                        return;
                        }
                    else { read = 0; break; }
                    }
                }
			if(read) break;
            }
        }
    }

void trans_mode(void) {
    int mem_num=1;
    int ip_num=0;
    unsigned short sw = 0;
    int str_size = 10;
    char mem_select = 0;
    char display_memselect[27];
    char display_mem_res[3] = { 0x20, 0, 0};
    strcpy(display_memselect, "Enter Memory No (1-6) :");
    all_clear();

    printf("TRANS_MODE");
    display_lcd("Mode : Transport");
    sleep(1);

    while(1) {
        while(!mem_select) {
            display_lcd(display_memselect);
            if (sw != 0) sw_clear(&sw);
            sw_update();
            sw = sw_status();

            switch(sw) {
                case 0x0001 : {
                    mem_num = 1;
                    display_mem_res[1] = mem_num + 0x30;
                    if(check_exist_files_in_mem(mem_num)) {
                        strcat(display_memselect,display_mem_res);
                        display_lcd(display_memselect);
                        show_dot_mem_num(mem_num);
                        mem_select = 1;
                        break;
                        }
                    else continue;
                    }
                case 0x0002 : {
                    mem_num = 2;
                    display_mem_res[1] = mem_num + 0x30;
                    if(check_exist_files_in_mem(mem_num)) {
                        strcat(display_memselect,display_mem_res);
                        display_lcd(display_memselect);
                        show_dot_mem_num(mem_num);
                        mem_select = 1;
                        break;
                        }
                    else continue;
                    }
                case 0x0004 : {
                    mem_num = 3;
                    display_mem_res[1] = mem_num + 0x30;
                    if(check_exist_files_in_mem(mem_num)) {
                        strcat(display_memselect,display_mem_res);
                        display_lcd(display_memselect);
                        show_dot_mem_num(mem_num);
                        mem_select = 1;
                        break;
                        }
                    else continue;
                    }
                case 0x0008 : {
                    mem_num = 4;
                    display_mem_res[1] = mem_num + 0x30;
                    if(check_exist_files_in_mem(mem_num)) {
                        strcat(display_memselect,display_mem_res);
                        display_lcd(display_memselect);
                        show_dot_mem_num(mem_num);
                        mem_select = 1;
                        break;
                        }
                    else continue;
                    }
                case 0x0010 : {
                    mem_num = 5;
                    display_mem_res[1] = mem_num + 0x30;
                    if(check_exist_files_in_mem(mem_num)) {
                        strcat(display_memselect,display_mem_res);
                        display_lcd(display_memselect);
                        show_dot_mem_num(mem_num);
                        mem_select = 1;
                        break;
                        }
                    else continue;
                    }
                case 0x0020 : {
                    mem_num = 6;
                    display_mem_res[1] = mem_num + 0x30;
                    if(check_exist_files_in_mem(mem_num)) {
                        strcat(display_memselect,display_mem_res);
                        display_lcd(display_memselect);
                        show_dot_mem_num(mem_num);
                        mem_select = 1;
                        break;
                        }
                    else continue;
                    }
                default : {
                    if(sw == 0x0040) {
                        state = PHOTO;
                        pid = -2;
                        sw_clear(&sw);
                        return;
                        }
                    else if(sw == 0x0080) {
                        state = READ;
                        sw_clear(&sw);
                        return;
                        }
                    else { sleep(1); break; }
                    }
                }
            }

        display_lcd("Select IP address");
        sleep(1);
        while(1) {
            if (sw != 0) sw_clear(&sw);
            sw_update();
            sw = sw_status();

            switch(sw) {
                case 0x0001 : {
                    ip_num = 0;
                    //if ( !file_send(ip_num, mem_num) ) return;
                    }
                case 0x0002 : {
                    ip_num = 1;
                    //if ( !file_send(ip_num, mem_num) ) return;
                    }
                case 0x0004 : {
                    ip_num = 2;
                    //if ( !file_send(ip_num, mem_num) ) return;
                    }
                case 0x0008 : {
                    ip_num = 3;
                    //if ( !file_send(ip_num, mem_num) ) return;
                    }
                case 0x0010 : {
                    ip_num = 4;
                    //if ( !file_send(ip_num, mem_num) ) return;
                    }
                case 0x0020 : {
                    ip_num = 5;
                    //if ( !file_send(ip_num, mem_num) ) return;
                    }
                default : {
                    if(sw == 0x0040) {
                        state = PHOTO;
                        pid = -2;
                        sw_clear(&sw);
                        return;
                        }
                    else if(sw == 0x0080) {
                        state = READ;
						pid = -2;
                        sw_clear(&sw);
                        return;
                        }
                    else if ( sw != 0x0100 && !file_send(ip_num, mem_num) ) {
                        state = PHOTO;
                        pid = -2;
                        sw_clear(&sw);
                        return;
                        }
                    }
                }
            }
        }
    }

int file_check(int mem_num) {
    char rm_command[18];
    char *file_name = (char*)malloc(16);
    int answer;
    FILE *fp;
    sprintf(file_name, "./photo_%d_1.png",mem_num);
    fp = fopen(file_name, "rw");
    if(!access(file_name, F_OK)) {
        sprintf(file_name, "./photo_%d_8.png",mem_num);
        fclose(fp);
        fp = fopen(file_name, "rw");
        if(!access(file_name, F_OK)) {
            seg(8);
            }
        else {
            seg(1);
            }
        display_lcd("Overwrite it?   Yes(1), No(2)");
        sleep(1);
        while(1) {
            read(dev[2], &push_sw_buff,buff_size);
            if(push_sw_buff[0]>0) {
                answer=1;
				sprintf(rm_command,"rm photo_%d_*.png",mem_num);
				system(rm_command);
                break;
                }
            else if(push_sw_buff[1]>0) {
                display_lcd("Please select   other memory");
                sw_clear(2);
                sleep(1);
                answer=0;
                break;
                }
            }
        }
    else {
        seg(0);
        answer = 1;
        }
    free(file_name);
    return answer;
    }

int check_exist_files_in_mem(int mem_num) {
    //unsigned char file_name[15];
    char *file_name = (char*)malloc(16);
    int answer;
    sprintf(file_name, "./photo_%d_1.png",mem_num);
	if ( !access(file_name, F_OK) ) {
		file_name[10]+=7;  
        //sprintf(file_name, "./photo_%d_8.png",mem_num);
        if(!access(file_name, F_OK)) 
            seg(8);
        else 
            seg(1);
        answer=1;
        }
    else {
        seg(0);
        display_lcd("No image in this memory address!");
        sleep(1);
        display_lcd("Please select   other memory");
        sleep(1);
        answer = 0;
        }
    free(file_name);
    return answer;
    }

int file_send(int ip_num, int mem_num) {
    unsigned char file_name[12];
    unsigned char ip[16];
    const char *nfs_command[2] = { "mount -t nfs ", ":/nfsroot /mnt/nfs2 -o rw,rsize=4096,nolock" };
    char command[256];
    FILE *fp;
    int i=0, connect_result = 0;
    strncpy((char*)file_name, "ip_list.txt",11);
    fp = fopen((char*)file_name, "rw");
    if(fp) {
        do {
            fscanf(fp, "%s", ip);
            i++;
            }
        while(i < ip_num);
        text_clear();
        sprintf(string, "Send to : %s",ip);
        display_lcd((const char*)string);
        sleep(1);

        strcpy(command, nfs_command[0]);
        strcat(command,(char*)ip);
        strcat(command,nfs_command[1]);
        //system("mkdir /mnt/send && chmod 777 /mnt/send");
        connect_result = system(command); // nfs connect
        printf("NFS STATUS :: %d\n", connect_result);
        if (connect_result) {
            printf("NFS Connection Error! :: CANNOT TRANSPORT!!\n");
            display_lcd("Transport ERROR!Cannot Connect");
            state = PHOTO;
            sleep(1);
            display_lcd("Please Check IPv4 addr");
            sleep(1);
            return 0;
            }

        command[0] = 0;

        sprintf(command,"cp photo_%d_*.png /mnt/nfs2",mem_num);
        system(command); // file copy
		//system("umount -af /mnt/send");
        display_lcd("Done!");
        sleep(1);
        }
    else {
        display_lcd("No ip list file!");
        sleep(1);
        return 0;
        }
    return 1;
    }

int image_show(int mem_num, int seq){
    char fbv[38];
	strcpy(fbv, "echo \'0xD\' | fbv -e -r ");
	char filename[16];
	sprintf(filename, "./photo_%d_%d.png", mem_num, seq);
	strcat(fbv, filename);
	if (!access(filename, F_OK)) {
		if (system(fbv)) display_lcd("Read Error!");
		else display_lcd("File Loading Complete!");
	} 
	else return 0;
	return 1;
}
