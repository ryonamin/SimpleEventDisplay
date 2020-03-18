
ROOTLIBS = $(shell root-config --evelibs)
CXXFLAGS = $(shell root-config --cflags) -g

INCPATH = -I${LCIO}/src/cpp/include -I${LCIO}/include
LIBS = -L${LCIO}/lib -llcio
CXXFLAGS += -std=c++11

all	: MCParticle.o Track.o Jet.o Event.o Dict.o
ifeq (${LCIO},)
	$(error Error due to missing LCIO path)
endif
	g++ ${CXXFLAGS} main.cc MCParticle.o Track.o Jet.o Event.o Dict.o ${ROOTLIBS} ${INCPATH} ${LIBS} -o run

MCParticle.o	: MCParticle.cc MCParticle.h 
	g++ ${CXXFLAGS} ${ROOTLIBS} ${INCPATH} ${LIBS} -c MCParticle.cc

Track.o	: Track.cc Track.h 
	g++ ${CXXFLAGS} ${ROOTLIBS} ${INCPATH} ${LIBS} -c Track.cc

Jet.o	: Jet.cc Jet.h 
	g++ ${CXXFLAGS} ${ROOTLIBS} ${INCPATH} ${LIBS} -c Jet.cc


Event.o	: Event.cc Event.h 
	g++ ${CXXFLAGS} ${ROOTLIBS} ${INCPATH} ${LIBS} -c Event.cc

Dict.o : Event.cc Event.h
	rootcint -f Dict.cxx -c Event.h ${INCPATH} LinkDef.h 
	g++ ${ROOTLIBS} ${CXXFLAGS} ${INCPATH} -c Dict.cxx

clean :
	rm Dict.cxx Dict.h *~ *.o *.pcm run
