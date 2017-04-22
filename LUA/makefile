CC = g++

INCLUDE_DIRS = 

LIB_DIRS = -L./ -L/usr/lib64/ 

LIBS = -lm -lpthread -lstdc++ -ldl

LINKER_OPTS = -Wall -g -O2

CFLAGS = -std=c++14 -g -Wall -Wunknown-pragmas -Wno-unused-value -Wno-unused-variable $(INCLUDE_DIRS)


BIN = lua


LUA_FILES = $(wildcard ./LUA/lua_lib/*.c)
LUA_CPP_FILES = $(wildcard ./LUA/*.cpp)
ROOT_FILES = $(wildcard ./*.cpp)


OBJ = $(addprefix ,$(notdir $(LUA_FILES:.c=.obj))) \
	$(addprefix ,$(notdir $(LUA_CPP_FILES:.cpp=.obj))) \
	$(addprefix ,$(notdir $(ROOT_FILES:.cpp=.obj)))






.PHONY: sestav
.PHONY: clean
.PHONY: vypis

#http://www.cs.dartmouth.edu/~campbell/cs50/buildlib.html


vypis:
	@echo $(OBJ)
#-----------------------------------------

clean:
	rm -f *.obj
	rm -f *.o


#-----------------------------------------
sestav: ${BIN}


#-----------------------------------------

$(BIN): $(OBJ)
	$(CC) $(LINKER_OPTS) $^ $(LIB_DIRS) $(LIBS) -o $@


#-----------------------------------------


	
#----------- LUA


%.obj: ./LUA/lua_lib/%.c
	gcc -g -c -o $@ $< 


%.obj: ./Lua/%.cpp
	$(CC)  $(CFLAGS) -c -o $@ $< 

#----------- 

%.obj: ./%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 

                                                                           




                      
