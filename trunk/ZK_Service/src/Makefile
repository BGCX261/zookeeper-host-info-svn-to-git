# set CPUS for Linux or FreeBSD
PLATFORM := $(shell uname)
CPUS := $(strip $(if $(shell echo $(PLATFORM)|grep Linux),\
	$(shell cat /proc/cpuinfo|grep -c processor),\
	$(shell sysctl -a | egrep -i 'hw.ncpu' | cut -d: -f2)))

CENTOS_VERSION=centos$(shell echo `cat /etc/issue | grep CentOS| cut -d' ' -f3`)


PWD=$(shell echo `pwd`)
MYLIBS=../libs/$(PLATFORM)/

ifeq ($(PLATFORM), Linux)
RPATH=-Wl,-rpath=$(MYLIBS)/gflags/ \
	-Wl,-rpath=$(MYLIBS)/glog/ \
	-Wl,-rpath=$(MYLIBS)/zookeeper/
else
	RPATH=-Wl,-R
endif

CXX := g++
DEBUGFLAGS= -DNDEBUG -g3
CXXFLAGS := $(DEBUGFLAGS) -Wall
LDFLAGS :=-L$(MYLIBS)/gflags/ \
	-L$(MYLIBS)/glog/ \
	-L$(MYLIBS)/zookeeper

INCLUDES :=-I$(PWD)/../include/

LIBS := -lglog \
	-lgflags \
	-ldaemon \
	-lzookeeper_mt
	
SRC := $(wildcard *.cpp \
	${PWD}/../include/zk_framework/*.cpp)
OBJ := $(patsubst %.cpp, %.o, $(SRC))
DEP := $(patsubst %.o, %.d, $(OBJ))


all: ZK_Service
	$(MAKE) -j$(CPUS) $<
	(mv $< ../bin/)



ZK_Service:$(OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS) $(RPATH)

%.o:%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)


.PHONY: clean
clean:
	rm -rf $(OBJ)

