CXXFLAGS ::= -Wall -Wextra -Wpedantic -fopenmp
LDFLAGS ::= -lboost_random -lgmp -fopenmp
CXXFLAGSREL ::= -O3
LDFLAGSREL ::=
CXXFLAGSDEBUG ::= -g
LDFLAGSDEBUG ::=

all: rsampler.r lmc.r appmc.r sampler.r pc.r
debug: rsampler.d lmc.d appmc.d sampler.d pc.d
.PHONY: all debug clean cleanall
obj:
	mkdir -p obj/src
clean:
	rm -rf obj
cleanall: clean
	rm -f *.r *.d

obj/src/pdac.cpp.o: obj src/pdac.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSREL) -c src/pdac.cpp -o obj/src/pdac.cpp.o
obj/src/pdac.cpp.od: obj src/pdac.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSDEBUG) -c src/pdac.cpp -o obj/src/pdac.cpp.od

obj/src/rsampler.cpp.o: obj src/rsampler.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSREL) -c src/rsampler.cpp -o obj/src/rsampler.cpp.o
obj/src/rsampler.cpp.od: obj src/rsampler.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSDEBUG) -c src/rsampler.cpp -o obj/src/rsampler.cpp.od
rsampler.r: obj/src/rsampler.cpp.o obj/src/pdac.cpp.o obj/src/nnf.cpp.o obj/src/var.cpp.o
	$(CXX) $(LDFLAGS) $(LDFLAGSREL) $^ -o rsampler.r
rsampler.d: obj/src/rsampler.cpp.od obj/src/pdac.cpp.od obj/src/nnf.cpp.od obj/src/var.cpp.od
	$(CXX) $(LDFLAGS) $(LDFLAGSDEBUG) $^ -o rsampler.d

obj/src/lmc.cpp.o: obj src/lmc.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSREL) -c src/lmc.cpp -o obj/src/lmc.cpp.o
obj/src/lmc.cpp.od: obj src/lmc.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSDEBUG) -c src/lmc.cpp -o obj/src/lmc.cpp.od
lmc.r: obj/src/lmc.cpp.o obj/src/pdac.cpp.o obj/src/nnf.cpp.o obj/src/var.cpp.o
	$(CXX) $(LDFLAGS) $(LDFLAGSREL) $^ -o lmc.r
lmc.d: obj/src/lmc.cpp.od obj/src/pdac.cpp.od obj/src/nnf.cpp.od obj/src/var.cpp.od
	$(CXX) $(LDFLAGS) $(LDFLAGSDEBUG) $^ -o lmc.d

obj/src/appmc.cpp.o: obj src/appmc.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSREL) -c src/appmc.cpp -o obj/src/appmc.cpp.o
obj/src/appmc.cpp.od: obj src/appmc.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSDEBUG) -c src/appmc.cpp -o obj/src/appmc.cpp.od
appmc.r: obj/src/appmc.cpp.o obj/src/pdac.cpp.o obj/src/nnf.cpp.o obj/src/var.cpp.o
	$(CXX) $(LDFLAGS) $(LDFLAGSREL) $^ -o appmc.r
appmc.d: obj/src/appmc.cpp.od obj/src/pdac.cpp.od obj/src/nnf.cpp.od obj/src/var.cpp.od
	$(CXX) $(LDFLAGS) $(LDFLAGSDEBUG) $^ -o appmc.d

obj/src/sampler.cpp.o: obj src/sampler.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSREL) -c src/sampler.cpp -o obj/src/sampler.cpp.o
obj/src/sampler.cpp.od: obj src/sampler.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSDEBUG) -c src/sampler.cpp -o obj/src/sampler.cpp.od
sampler.r: obj/src/sampler.cpp.o obj/src/pdac.cpp.o obj/src/nnf.cpp.o obj/src/var.cpp.o
	$(CXX) $(LDFLAGS) $(LDFLAGSREL) $^ -o sampler.r
sampler.d: obj/src/sampler.cpp.od obj/src/pdac.cpp.od obj/src/nnf.cpp.od obj/src/var.cpp.od
	$(CXX) $(LDFLAGS) $(LDFLAGSDEBUG) $^ -o sampler.d

obj/src/nnf.cpp.o: obj src/nnf.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSREL) -c src/nnf.cpp -o obj/src/nnf.cpp.o
obj/src/nnf.cpp.od: obj src/nnf.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSDEBUG) -c src/nnf.cpp -o obj/src/nnf.cpp.od

obj/src/var.cpp.o: obj src/var.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSREL) -c src/var.cpp -o obj/src/var.cpp.o
obj/src/var.cpp.od: obj src/var.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSDEBUG) -c src/var.cpp -o obj/src/var.cpp.od

obj/src/pc.cpp.o: obj src/pc.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSREL) -c src/pc.cpp -o obj/src/pc.cpp.o
obj/src/pc.cpp.od: obj src/pc.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGSDEBUG) -c src/pc.cpp -o obj/src/pc.cpp.od
pc.r: obj/src/pc.cpp.o obj/src/pdac.cpp.o obj/src/nnf.cpp.o obj/src/var.cpp.o
	$(CXX) $(LDFLAGS) $(LDFLAGSREL) $^ -o pc.r
pc.d: obj/src/pc.cpp.od obj/src/pdac.cpp.od obj/src/nnf.cpp.od obj/src/var.cpp.od
	$(CXX) $(LDFLAGS) $(LDFLAGSDEBUG) $^ -o pc.d

