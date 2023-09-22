CXX = g++
CXXFLAGS = -Wextra -Wall -Wfloat-equal -Wconversion -Wparentheses \
-pedantic -Wunused-parameter -Wunused-variable -Wreturn-type \
-Wunused-function -Wredundant-decls -Wreturn-type -Wunused-value \
-Wswitch-default -Wuninitialized -Winit-self -Werror

SRC = ENSE452-A1.cpp ENSE452-A1.h
OBJ = $(SRC:.cpp=.o)
all: as1

as1 : $(OBJ)
	$(CXX) $(CXXFLAGS) -o calculator $(SRC)