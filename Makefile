CXX      = g++
CXXFLAGS = -Ofast -g -Wall -std=c++11 -static -I ../include
VPATH    = ../include
RM       = del
.PHONY: all clean
%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) $< -c -o $@
%.exe: %.o
	$(CXX) $(CXXFLAGS) $^ -o $@
all: prob_rand.exe prob_efft.exe prob_cnt.exe prob_mcts.exe prob_recu.exe prob_recud.exe
prob_rand.exe: calsht_dw.o calscr.o score.o
prob_efft.exe: calsht_dw.o calscr.o score.o
prob_cnt.exe: calsht_dwc.o calscr.o score.o
prob_mcts.exe: calsht_dw.o calscr.o mjmcts.o score.o
prob_recu.exe: calsht_dw.o calscr.o mjts_recu3.o score.o mjmcts2.o
prob_recud.exe: calsht_dw.o calscr.o mjtsd21.o score.o
clean:
	$(RM) *.o
