# Tên file thực thi
TARGET = test_temp_control

# Trình biên dịch
CC = gcc

# Đường dẫn 
PATH_APP_CTRL = src/dev/M0_App/AppOS/App_6_Temperature_Control
PATH_OS       = src/dev/M0_App/OS
PATH_TEST     = unit_test/pid_test

# Include
INC_DIRS = -I$(PATH_APP_CTRL) -I$(PATH_OS) -I$(PATH_TEST)

# Cờ biên dịch (Thêm -DSIMULATION_MODE để chạy file sw_timer mock trên PC)
CFLAGS = $(INC_DIRS) -Wall -g -DSIMULATION_MODE

# File nguồn
SRCS = $(PATH_APP_CTRL)/pid.c \
       $(PATH_APP_CTRL)/profile_state.c \
       $(PATH_OS)/sw_timer.c \
       $(PATH_TEST)/test_main.c

# Khai báo các target giả (không phải là file thực tế) để tránh lỗi xung đột tên
.PHONY: all clean run

# QUY TẮC MẶC ĐỊNH (Target đầu tiên)
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) -lm

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)