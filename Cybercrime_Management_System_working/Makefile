CC = g++
CFLAGS = -std=c++17 -Wall -Wextra -I"C:/Program Files/MySQL/MySQL Server 8.0/include"
LDFLAGS = -L"C:/Program Files/MySQL/MySQL Server 8.0/lib" -lmysql
SRC_DIR = src
INCLUDE_DIR = include
TARGET = cybercrime_system

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	del /Q $(SRC_DIR)\*.o $(TARGET).exe 2>nul || rm -f $(SRC_DIR)/*.o $(TARGET)

.PHONY: all clean
