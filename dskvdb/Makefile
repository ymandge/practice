CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra
LIBS = -lpthread

TARGET = dskvdb
SRCS = main.cpp node.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)