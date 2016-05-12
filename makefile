# Cpp Cost Model makefile
# Written for Cpp Cost Model project by Ismael Jimenez

# Set compiler choice
CC := g++

# Set compilation flags.
CXXFLAGS := -Wall -O3 -std=c++14 -fno-omit-frame-pointer

# Set required libraries
LDFLAGS := -lbenchmark -lpthread

# Source files directory
SRCDIR := src

# List all of source files 
SRC := $(SRCDIR)/benchmark.cc

# Binary name
PRODUCT := benchmark

# Default rule
all:   $(PRODUCT)

# Clean target
clean:
	rm -f $(OBJ) $(OBJ_IVY) $(PRODUCT) $(PRODUCT_IVY)

# Rule to generate a list of object names.
OBJ = $(addsuffix .o, $(basename $(SRC)))
OBJ_IVY = $(addsuffix .o, $(basename $(SRC_IVY)))

# Tell make how to automatically generate rules that build the
# appropriate object-file from each of the source files listed in SRC.
%.o : %.cc .buildmode
	$(CC) $(CXXFLAGS) -g $< -o $@
%.o : %.cpp .buildmode
	$(CC) $(CXXFLAGS) -g $< -o $@

$(PRODUCT): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)
$(PRODUCT_IVY): $(OBJ_IVY)
	$(CC) -o $@ $(OBJ_IVY) $(LDFLAGS)

