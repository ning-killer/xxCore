
# define the exlude dirs here if need
exclude_dirs= include

# get all of the sub dirs
dirs:=$(shell find . -maxdepth 1 -type d)
dirs:=$(basename $(patsubst ./%,%,$(dirs)))
dirs:=$(filter-out $(exclude_dirs),$(dirs))
SUBDIRS := $(dirs)


# target source
CSRCSS  := $(wildcard src/*.c)
CPPSRCS:= $(wildcard src/*.cpp)
COBJS  := $(CSRCSS:%.c=%.o)
CPPOBJS := $(CPPSRCS:%.cpp=%.o)


COBJ  := $(COBJS:src/%=%)
CPPOBJ  := $(CPPOBJS:src/%=%)


.PHONY : all

#all: $(COBJS) $(CPPOBJS) subdirs
all: $(COBJS) $(CPPOBJS)

subdirs:$(SUBDIRS)
	@for dir in $(SUBDIRS);\
		do $(MAKE) -C $$dir all||exit 1;\
	done

$(COBJS):%.o:%.c 
	@echo CC $(<F)
	@$(CC) $(CFLAGS) -c $< -o $(OBJS_DIR)/$(notdir $@)
	
$(CPPOBJS):%.o:%.cpp 
	@echo CPP $(<F)
	@$(GPP) $(CPPFLAGS) -c $< -o $(OBJS_DIR)/$(notdir $@)

