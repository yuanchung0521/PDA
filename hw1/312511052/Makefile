# Makefile for My C++ Project

# 编译器
CXX = g++

# 编译选项
CXXFLAGS = -std=c++11 -Wall

# 目标文件
TARGET = main

# 源文件
SOURCES = main.cpp

# 目标文件依赖的头文件
HEADERS = classtype.h

# 编译可执行文件
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# 清理中间文件和可执行文件
clean:
	rm -f $(TARGET)
