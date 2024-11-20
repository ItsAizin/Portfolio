CXX := g++
CXXFLAGS := -std=gnu++14 -MMD -MP -O3 #-Wall -Wextra

SOURCES = $(wildcard *.cc)
OBJECTS = $(SOURCES:.cc=.o)
TARGET = run.out

.PHONY: clean

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ -c

clean:
	$(RM) *.o $(TARGET) *.d

-include $(OBJECTS:.o=.d)
