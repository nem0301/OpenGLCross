OBJ_DIR = ./obj
SRC_DIR = ./src
HDR_DIR = ./hdr
APP_NAME = app.exe

WIN = x86_64-w64-mingw32-
CXX = $(WIN)g++
LD  = $(WIN)g++

INCLUDES = -I./hdr
CFLAGS = -g -std=c++11 -Wall -DGLEW_STATIC
LIBS = -L./lib -lglfw3 -lgdi32 -lglew32 -lopengl32 -lws2_32 -static-libstdc++ -static-libgcc 

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:%.o=$(OBJ_DIR)/%.d)

all : $(APP_NAME)

$(APP_NAME) : $(OBJS)
	$(LD) -o $@ $^ $(LIBS) 

$(OBJS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(HDR_DIR)/*.h
	$(CXX) $(CLFAGS) $(INCLUDES)  -MMD -o $@ -c $<


.PHONY : clean
clean :
	@rm $(APP_NAME) $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d
