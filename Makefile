# Makefile for compiling an ev3 project with the following folder structure:
#
#	this_folder/
# 		Makefile (this file)
#		ev3dev-c/
#		libraries/
#			libbluetooth.a
#			libev3dev-c.a
#		include/
# 			bt_client.h
#			messages.h
#			movement.h
#			sensors.h
#	 	source/
#			bt_client.c
#			messages.c
#			movement.c
#			sensors.c
#			main.c
#
# The main executable will be located in the same directory as you ran 
# "Make" from. To add new .c files, simply add them to the OBJS variable.

CC 		= arm-linux-gnueabi-gcc
CFLAGS 		= -O2 -g -std=gnu99 -W -Wall -Wno-comment
INCLUDES 	= -I./ev3dev-c/source/ev3 -I./include/
LDFLAGS 	= -L./libraries -lrt -lm -lev3dev-c -lpthread -lbluetooth
BUILD_DIR 	= ./build
SOURCE_DIR 	= ./source

OBJS = \
	$(BUILD_DIR)/movement.o \
	$(BUILD_DIR)/sensors.o \
	$(BUILD_DIR)/bt_client.o \
	$(BUILD_DIR)/messages.o \
	$(BUILD_DIR)/object_recogn.o \
	$(BUILD_DIR)/search.o \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/SharedData.o \
	$(BUILD_DIR)/server_send_thread.o \
	$(BUILD_DIR)/sensor_thread.o \
	$(BUILD_DIR)/gyro_thread.o 

all: main

main: ${OBJS}
	$(CC) $(INCLUDES) $(CFLAGS) $(OBJS) $(LDFLAGS) -o main

test1: $(BUILD_TEST)/test1.o
test2: $(BUILD_TEST)/test2.o
test3: $(BUILD_TEST)/test3.o
test4: $(BUILD_TEST)/test4.o
test5: $(BUILD_TEST)/test5.o
test6: $(BUILD_TEST)/test6.o

$(OBJS): $(BUILD_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) -c $(SOURCE_DIR)/$*.c $(INCLUDES) -o $(BUILD_DIR)/$*.o

$(OBJS_TEST):  \
        $(BUILD_DIR)/movement.o \
        $(BUILD_DIR)/sensors.o \
		$(BUILD_DIR)/launcher.c \
        $(BUILD_DIR)/bt_client.o \
        $(BUILD_DIR)/messages.o \
        $(BUILD_DIR)/object_recogn.o \
        $(BUILD_DIR)/search.o 

$(BUILD_TEST)/%.o: $(OBJS_TEST) $(BUILD_DIR)/%.o
	$(CC) $(INCLUDES) $(CFLAGS) $(OBJS_TEST) $(BUILD_DIR)/$*.o $(LDFLAGS) -o $*


launch: 
	$(CC) $(INCLUDES) $(CFLAGS) -c $(SOURCE_DIR)/launcher.c -o $(BUILD_DIR)/launcher.o
	$(CC) $(BUILD_DIR)/launcher.o -Wall -lm -lev3dev-c $(CFLAGS) $(LDFLAGS) -o launcher

clean:
	rm -f $(BUILD_DIR)/*.o
	rm ./main

export LD_LIBRARY_PATH=./ev3dev-c/lib/


