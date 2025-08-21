Target=server
SrcDir=src
IncDir=inc
ObjDir=obj

# 创建带目录的.o文件路径列表
Object=$(patsubst $(SrcDir)/%.cpp,$(ObjDir)/%.o,$(wildcard $(SrcDir)/*.cpp))

CXXFLAGS=-levent -lmysqlclient -ljsoncpp -std=c++11
CPPFLAGS=-I$(IncDir)

$(Target): $(ObjDir) $(Object)
	g++ $(Object) -g -o $(Target) $(CXXFLAGS)

# 创建obj目录
$(ObjDir):
	mkdir -p $(ObjDir)

# 修改编译规则以支持obj目录
$(ObjDir)/%.o: $(SrcDir)/%.cpp
	g++ -c $< -o $@ $(CPPFLAGS)

clean:
	rm -rf $(ObjDir) $(Target)

.PHONY: clean $(ObjDir)