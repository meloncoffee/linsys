# 빌드 시간 정보
BUILD_TIME := $(shell date "+%Y-%m-%d\ %H:%M:%S")

# 모듈명 및 버전 정보
LINSYS_GUARD := linsys_guard
LINSYS_GUARD_VERSION := 1.0.0
LINSYS_MANAGER := linsys_manager
LINSYS_MANAGER_VERSION := 1.0.0

# 디렉터리 정의
SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := obj
INC_DIR := include

# 빌드 시 define 설정
LINSYS_GUARD_DEFINES := -DMODULE_NAME=\"$(LINSYS_GUARD)\" \
						-DVERSION=\"$(LINSYS_GUARD_VERSION)\" \
						-DBUILD_TIME=\"$(BUILD_TIME)\"
LINSYS_MANAGER_DEFINES := -DMODULE_NAME=\"$(LINSYS_MANAGER)\" \
						  -DVERSION=\"$(LINSYS_MANAGER_VERSION)\" \
						  -DBUILD_TIME=\"$(BUILD_TIME)\"

# 공통 소스 파일 수집 및 오브젝트 파일 목록 생성
COMMON_SRCS := $(shell find $(SRC_DIR)/common -type f -name "*.c")
COMMON_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(COMMON_SRCS))

# linsys_guard 소스 파일 수집 및 오브젝트 파일 목록 생성
LINSYS_GUARD_SRCS := $(shell find $(SRC_DIR)/$(LINSYS_GUARD) -type f -name "*.c")
LINSYS_GUARD_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(LINSYS_GUARD_SRCS))
LINSYS_GUARD_OBJS += $(COMMON_OBJS)

# linsys_manager 소스 파일 수집 및 오브젝트 파일 목록 생성
LINSYS_MANAGER_SRCS := $(shell find $(SRC_DIR)/$(LINSYS_MANAGER) -type f -name "*.c")
LINSYS_MANAGER_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(LINSYS_MANAGER_SRCS))
LINSYS_MANAGER_OBJS += $(COMMON_OBJS)
