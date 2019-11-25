CXX      = g++
CXXFLAGS = -O3 -Wall -std=c++11
RM       = rm -f

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) $< -c -o $@
mkind2.out: mkind2.o judwin.o
	$(CXX) $(CXXFLAGS) $^ -o $@
prob.out: prob.o calsht_dw.o
	$(CXX) $(CXXFLAGS) $^ -o $@
clean:
	$(RM) *.o
