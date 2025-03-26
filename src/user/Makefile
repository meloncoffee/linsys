# 컴파일러 및 플래그
CC := gcc
CFLAGS := -O2 -Wall -Wextra
LDFLAGS := -lpthread

# 디렉토리 및 파일 설정
OBJ_DIR := obj
INC_DIR := ../../include

# 하위 디렉터리까지 .c 파일을 찾음
SRCS := $(shell find . -name '*.c')
# 소스 파일 경로를 오브젝트 파일 경로로 변환
OBJS := $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

DEFINES := -DLINSYS_MANAGER
DEBUG ?= 0
ifeq ($(DEBUG), 1)
    DEFINES += -DDEBUG
    CFLAGS += -g
endif

all: $(USER_MODULE_NAME)

# 실행 파일 빌드
$(USER_MODULE_NAME): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# 오브젝트 빌드 규칙
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@

# 정리
clean:
	@rm -rf $(USER_MODULE_NAME) $(OBJ_DIR)

.PHONY: all clean
