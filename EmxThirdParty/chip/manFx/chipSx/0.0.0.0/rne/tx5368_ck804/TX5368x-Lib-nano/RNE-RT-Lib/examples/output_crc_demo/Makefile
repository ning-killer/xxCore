################################################################################
# makefile
################################################################################

ARCH ?= a53
ROOT_PATH ?= ../..
EXPORT_DIR := $(ROOT_PATH)

ifeq ($(strip $(ARCH)),a53)
	CC_PREFIX ?= aarch64-none-elf-
	CPU_FLAGS ?= -march=armv8-a -mcpu=cortex-a53
	DEF_FLAGS ?= -DRESULT_HWC
	INC_FLAGS ?=
	BUILD_PATH ?= build_a53
	PLATFORM := a53
	X_LIB_L_FLAGS := -la53_bsp,-lgcc,-lc,-lm
	LINK_SCRIPT := ../../libs/64bit/gcc.ld
	LINK_FLAGS := -Wl,--start-group --specs=nosys.specs -T$(LINK_SCRIPT) -Wl,--end-group -Wl,-gc-sections -Wl,-zmax-page-size=1024 --specs=rdimon.specs
	LIB_SUB_DIR ?= 64bit
endif

INC_FLAGS += -I$(EXPORT_DIR)/include/

LIBS := -L$(EXPORT_DIR)/libs/$(LIB_SUB_DIR) -Wl,--start-group,-lrne_rt_g2,-lrne_pal_$(PLATFORM),$(X_LIB_L_FLAGS),--end-group
CC_FLAGS := $(CPU_FLAGS)
CC_FLAGS += $(DEF_FLAGS)
CC_FLAGS += $(OPT_LEVEL)

RM =rm -rf

ifeq ($(MAKECMDGOALS),release)
	BUILD_DIR = $(BUILD_PATH)/Release
	OPT_LEVEL ?= -Os
else ifeq ($(MAKECMDGOALS),debug)
	BUILD_DIR = $(BUILD_PATH)/Debug
	OPT_LEVEL ?= -O0
else
	BUILD_DIR = $(BUILD_PATH)/Release
	OPT_LEVEL ?= -Os
endif

ifeq ($(OPT_LEVEL),-O0)
    CC_FLAGS += -g
endif

CC_FLAGS += $(OPT_LEVEL)
CC_FLAGS += -Werror
CC_FLAGS += -fPIC -ffunction-sections -fdata-sections
CC_FLAGS += $(INC_FLAGS)
LINK_FLAGS += $(OPT_LEVEL)


SRC_DIR := $(shell find src -type d)
SRC_DIR += $(shell find src -type l)

C_SRCS = $(wildcard $(foreach n,$(SRC_DIR),$(n)/*.c))
C_OBJS = $(addprefix $(BUILD_DIR)/, ${C_SRCS:.c=.o})

CC_SRCS = $(wildcard $(foreach n,$(SRC_DIR),$(n)/*.cpp))
CC_OBJS = $(addprefix $(BUILD_DIR)/, ${CC_SRCS:.cpp=.o})

OBJS = $(CC_OBJS) $(C_OBJS)

SRC_DEPS = $(patsubst %.o, %.d, $(OBJS))

TARGET_NAME = simple_forward_demo.elf
TARGET = $(BUILD_DIR)/$(TARGET_NAME)
ELFSIZE = $(TARGET).size

ifneq ($(CC_SRCS),)
	LINKER  := $(CC_PREFIX)g++
else ifeq ($(CC_PREFIX),)
	LINKER  := $(CC_PREFIX)g++
else
	LINKER  := $(CC_PREFIX)gcc
endif

# All Target
release: all
debug: all
all: pre_build $(TARGET) $(ELFSIZE)

# Tool invocations
$(TARGET): $(OBJS) $(LINK_SCRIPT)
	@echo 'Building target: $@'
	$(LINKER) $(CPU_FLAGS) $(LINK_FLAGS) -o $@ $(OBJS) $(LIBS)
	@echo ' '

%.size: %
	@echo 'Building target: $@'
	$(CC_PREFIX)size $< |tee $@
	@echo ' '

$(BUILD_DIR)/%.o: %.c
	@echo 'Building file: $<'
	$(CC_PREFIX)gcc -Wall -std=c99 -c -fmessage-length=0 $(CC_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo ' '

$(BUILD_DIR)/%.o: %.cpp
	@echo 'Building file: $<'
	$(CC_PREFIX)g++ -Wall -std=c++11 -c -fmessage-length=0 $(CC_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo ' '


-include $(SRC_DEPS)

# Other Targets
clean:
	@-$(RM) $(SRC_DEPS) $(OBJS) $(ELFSIZE) $(TARGET)
	@-$(RM) build*

run: all
	$(TARGET)

pre_build: mkdir_build


mkdir_build:
	@if [ ! -d $(BUILD_DIR) ]; then\
		mkdir -p $(addprefix $(BUILD_DIR)/, $(SRC_DIR));\
	fi

.PHONY: clean all pre_build mkdir_build

