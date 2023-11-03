CXX := 
CXXFLAGS :=
LDFLAGS :=

SRC_DIR := 
INC_DIR := 

CXXFLAGS += -I$(INC_DIR)

TARGET := main
HEADERS := $(wildcard $(INC_DIR)/*.hpp)
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

clean:
	@rm -f $(OBJS) $(TARGET)