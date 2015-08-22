CC := g++ 
EXE := ./bin/main
DIR_SRC := ./src
DIR_OBJ := ./obj
DIR_LIB := ./lib
DIR_INC := ./include
MYSQL_INC := /usr/include/mysql
MYSQL_LIB := /usr/lib64/mysql


SRC := $(wildcard $(DIR_SRC)/*.cpp)
OBJ := $(patsubst %.cpp, $(DIR_OBJ)/%.o, $(notdir $(SRC)))

CFLAG := -I$(DIR_INC)  -L$(DIR_LIB)  -I$(MYSQL_INC) -L$(MYSQL_LIB)  -g  -Wall -lpthread -lcrypto -lnlpir  -llog4cpp -ljsoncpp -lhiredis -lmysqlclient  -std=c++11 -DOS_LINUX   -DMY_DEBUG


$(EXE): $(OBJ)
	$(CC)  -o $@ $^ $(CFLAG)  

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp
	$(CC) -c -o $@ $^ $(CFLAG)  

.PHONY: clean
clean:
	find $(DIR_OBJ) -name "*.o" -exec rm -rf {} \;


