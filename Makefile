include config.mk

# 컴파일러 및 옵션
CC := gcc
CFLAGS := -Wall -Wextra -I$(INC_DIR)
LDFLAGS := -lpthread

# 디버깅 옵션 체크
ifeq ($(DEBUG), 1)
	CFLAGS += -O0 -g -DDEBUG
else
	CFLAGS += -O2
endif

all: $(BIN_DIR)/$(LINSYS_GUARD) $(BIN_DIR)/$(LINSYS_MANAGER)

# linsys_guard 빌드
$(LINSYS_GUARD): $(BIN_DIR)/$(LINSYS_GUARD)
$(BIN_DIR)/$(LINSYS_GUARD): $(LINSYS_GUARD_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

# linsys_manager 빌드
$(LINSYS_MANAGER): $(BIN_DIR)/$(LINSYS_MANAGER)
$(BIN_DIR)/$(LINSYS_MANAGER): $(LINSYS_MANAGER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

# 공통 오브젝트 파일 빌드 규칙
$(OBJ_DIR)/common/%.o: $(SRC_DIR)/common/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<

# linsys_guard 오브젝트 파일 빌드 규칙
$(OBJ_DIR)/$(LINSYS_GUARD)/%.o: $(SRC_DIR)/$(LINSYS_GUARD)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LINSYS_GUARD_DEFINES) -o $@ -c $<

# linsys_manager 오브젝트 파일 빌드 규칙
$(OBJ_DIR)/$(LINSYS_MANAGER)/%.o: $(SRC_DIR)/$(LINSYS_MANAGER)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LINSYS_MANAGER_DEFINES) -o $@ -c $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean $(LINSYS_GUARD) $(LINSYS_MANAGER)
