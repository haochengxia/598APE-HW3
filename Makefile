ENABLE_BETTER_OPT ?= 0
ENABLE_OPENMP ?= 1
ENBALE_ALLOCAETED_NEXT ?= 0
ENBALE_BARNES_HUT ?= 1

ifeq ($(ENABLE_BETTER_OPT), 1)
    EXTRA_FLAGS += -march=native -ffast-math
endif

ifeq ($(ENABLE_OPENMP), 1)
	EXTRA_FLAGS += -fopenmp -DENABLE_OPENMP
endif

ifeq ($(ENBALE_ALLOCAETED_NEXT), 1)
	EXTRA_FLAGS += -DENBALE_ALLOCAETED_NEXT
endif

ifeq ($(ENBALE_BARNES_HUT), 1)
	EXTRA_FLAGS += -DENBALE_BARNES_HUT
endif

FUNC := g++
copt := -c 
OBJ_DIR := ./bin/
FLAGS := -O3 -lm -g -Werror $(EXTRA_FLAGS)

CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix $(OBJ_DIR),$(notdir $(CPP_FILES:.cpp=.obj)))

all:
	$(FUNC) ./main.cpp -o ./main.exe $(FLAGS)

clean:
	rm -f ./*.exe