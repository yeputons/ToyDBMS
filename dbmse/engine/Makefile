.PHONY: all clean

# Based on https://github.com/yeputons/project-templates

TARGETS=demo
SRCS_demo=$(wildcard $(SRCDIR)/*.cpp)
OBJDIR=.obj
SRCDIR=src
INCDIR=inc

all: $(TARGETS)
demo: $(SRCS_demo:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

CXX=g++
CXXFLAGS=-pedantic -std=c++11 -g -O0 -I$(INCDIR)
LDFLAGS=
LDLIBS=

$(TARGETS):
	$(CXX) -o $@ $(LDFLAGS) $^ $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf $(TARGETS) $(OBJDIR)

include $(wildcard $(OBJDIR)/*.d)
