#ifndef CALSHT_DW_HPP
#define CALSHT_DW_HPP

#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>

class CalshtDW{
private:
  using Ull  = unsigned long long int;
  using Vec  = std::vector<Ull>;
  using Iter = std::vector<Vec>::iterator;

  std::vector<Vec> mp1;
  std::vector<Vec> mp2;
  
  Iter itr1;
  Iter itr2;

  void shift(int& lv, int rv, Ull& lx, Ull rx, Ull& ly, Ull ry) const;
  void add(Vec& lhs, const Vec& rhs) const;
  void add(Vec& lhs, const Vec& rhs, int j) const;
  Iter read_file(Iter first, Iter last, const char* filename) const;
  
public:
  CalshtDW();
  int calc_lh(const int* t, int m, Ull& disc, Ull& wait) const;
  int calc_sp(const int* t, Ull& disc, Ull& wait) const;
  int calc_sp(const int* t, int m, Ull& disc, Ull& wait) const {return calc_sp(t,disc,wait);}
  int calc_to(const int* t, Ull& disc, Ull& wait) const;
  int calc_to(const int* t, int m, Ull& disc, Ull& wait) const {return calc_to(t,disc,wait);}
  int operator()(const int* t, int m, int& mode, Ull& disc, Ull& wait) const;
  bool operator!() const;
};

#endif
