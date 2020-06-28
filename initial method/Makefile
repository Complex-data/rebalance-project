CC=g++
CPPFLAGS= -O3 -march=native -std=c++11 -funroll-all-loops
#CC=/opt/ohpc/pub/compiler/llvm/5.0.1/bin/clang+
#CXX=clang++ -std=c++11
#CXX=clang++ -std=c++11 -stdlib=libc++
#CPPFLAGS= -O3 -march=native -std=c++11

INCLUDE= 

#CPPFLAGS+= -DNBEBUG
EXEC=create
SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)

all: $(EXEC)

create: $(OBJ)
	@$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp %.hpp
	@$(CXX) -o $@ -c $< $(CPPFLAGS) $(INCLUDE)


clean:
	@rm -rf *.o

