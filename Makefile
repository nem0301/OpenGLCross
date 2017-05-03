OBJ_DIR = ./obj
SRC_DIR = ./src
HDR_DIR = ./hdr
TGT_DIR = ./tgt
APP_NAME = app.exe
TARGET = $(TGT_DIR)/$(APP_NAME)

WIN = x86_64-w64-mingw32-
WIN_CXX = $(WIN)g++
WIN_LD  = $(WIN)g++

OSX_CXX = g++
OSX_LD  = g++

INCLUDES = -I./hdr
WIN_CFLAGS = -g -std=c++11 -Wall -DGLEW_STATIC
OSX_CFLAGS = -g -std=c++11 -Wall -DGLEW_STATIC
WIN_LIBS = -L./lib -lglfw3 -lgdi32 -lglew32 -lopengl32 -lws2_32 -static-libstdc++ -static-libgcc 
OSX_LIBS = -framework OpenGL -lglfw -lglew

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:%.o=%.d)

#LIBS = $(OSX_LIBS)
#CFLAGS = $(OSX_CFLAGS)
#CXX = $(OSX_CXX)
#LD = $(OSX_LD)

LIBS = $(WIN_LIBS)
CFLAGS = $(WIN_CFLAGS)
CXX = $(WIN_CXX)
LD = $(WIN_LD)


all : $(TARGET) glslvalid
	@cp -r ./shader ./tgt
	@cp -r ./res ./tgt
	@cd tgt && rm -f app.tgz && tar zcf app.tgz * 
	@./put.sh

$(TARGET) : $(OBJS)
	$(LD) -o $@ $^ $(LIBS) 

$(OBJS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp 
	$(CXX) $(CFLAGS) $(INCLUDES)  -MMD -o $@ -c $<

glslvalid:
	@glslangValidator shader/*


.PHONY : clean
clean :
	@rm -rf $(TGT_DIR)/* $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d 

-include $(DEPS)
