EXECUTABLE=myapp

INCDIR=inc
SRCDIR=src
OBJDIR=obj
BINDIR=bin
OBJS = $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(wildcard $(SRCDIR)/*.cpp))

CXX=g++
CXXFLAGS=-Wall -I $(INCDIR) -c -std=c++17
LIBS=$(shell pkg-config --static --libs glfw3 gl) 
#LIBS=-lGL -lGLU -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor
LDFLAGS=$(LIBS)

.PHONY: all
all: $(BINDIR)/$(EXECUTABLE)

.PHONY: clean
clean:
	rm -vrf $(BINDIR) $(OBJDIR)

.PHONY: run
run: $(BINDIR)/$(EXECUTABLE)
	./$(BINDIR)/$(EXECUTABLE)

$(BINDIR)/$(EXECUTABLE): $(OBJS)
	mkdir -p $(BINDIR)
	$(CXX) -o $@ $^ $(LDFLAGS) 

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp 
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $< -o $@

