OBJ_DIR = ./obj
SRC_DIR = ./src
HDR_DIR = ./hdr
TGT_DIR = ./tgt
APP_NAME = app.exe
TARGET = $(TGT_DIR)/$(APP_NAME)

WIN = x86_64-w64-mingw32-
CXX = $(WIN)g++
LD  = $(WIN)g++

INCLUDES = -I./hdr
CFLAGS = -g -std=c++11 -Wall -DGLEW_STATIC
LIBS = -L./lib -lglfw3 -lgdi32 -lglew32 -lopengl32 -lws2_32 -static-libstdc++ -static-libgcc 

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:%.o=%.d)

all : $(TARGET)
	cp ./shader/* ./tgt
	cp -r ./res ./tgt

$(TARGET) : $(OBJS)
	$(LD) -o $@ $^ $(LIBS) 

$(OBJS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp 
	$(CXX) $(CLFAGS) $(INCLUDES)  -MMD -o $@ -c $<


.PHONY : clean
clean :
	@rm $(TGT_DIR)/* $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d 

-include $(DEPS)
