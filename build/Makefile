ROOT_DIR=c:/CPPS/hamster
INCLUDE_DIR=$(ROOT_DIR)/inc
SRC_DIR=$(ROOT_DIR)/src
OUTPUT_DIR=$(ROOT_DIR)/out

BUILD_TARGET=demo
LIBS=-lpthread
CC=c++
CFLAGS= -g -std=c++11 -I $(INCLUDE_DIR)

vpath %.o $(OUTPUT_DIR)
vpath %.cpp $(SRC_DIR)

# g++ -g -std=c++11 -I dir -o demo DemoMain.cpp o1.o o2.o
$(BUILD_TARGET):libs
	$(CC) $(CFLAGS) -o $@ 

libs: