CC := g++
TARGET := main
CPPFLAGS :=-std=c++14 -MMD -MP
LDFLAGS :=

SRCS := $(wildcard ./src/*.cpp)
OBJS := $(addsuffix .o, $(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)

.PHONY : all
all: $(TARGET)

.PHONY : debug
debug: CPPFLAGS += -DDEBUG -g
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) $(OBJS) -o $@

.PHONY : clean
clean:
	rm $(TARGET) $(OBJS) $(DEPS)

-include $(DEPS)