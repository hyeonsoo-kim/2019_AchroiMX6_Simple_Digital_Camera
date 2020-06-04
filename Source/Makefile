CC = arm-none-linux-gnueabi-g++
#CC = gcc
STDFLAG = -std=c++11
CFLAG = -D TEST -W -Wall -I /work/achroimx6q/achroimx_kernel/include -lpthread  -I /usr/local/include
LFALG = -lpthread
OPT = -Og
TARGET = term_proj

all : $(TARGET)

$(TARGET) : include_list.h image_open.h device.h
	$(CC) $(STDFLAG) $(CFLAG) $(LFALG) $(OPT) -o $(TARGET) main.c v4l_wrapper.c 
	rm -f *.o

#v4l_wrapper.o : v4l_wrapper.c v4l_wrapper.h
#	$(CC) $(CFLAG) -c -o v4l_wrapper.o v4l_wrapper.c v4l_wrapper.h

clean:
	rm -f *.o
