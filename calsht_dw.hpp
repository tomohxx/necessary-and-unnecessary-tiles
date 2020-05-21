#ifndef CALSHT_DW_HPP
#define CALSHT_DW_HPP

#include <vector>
#include <cstdint>

class CalshtDW{
private:
  using Vec  = std::vector<std::int64_t>;
  using Iter = std::vector<Vec>::iterator;

  std::vector<Vec> mp1;
  std::vector<Vec> mp2;

  Iter itr1;
  Iter itr2;

  void shift(int& lv, int rv, std::int64_t& lx, std::int64_t rx, std::int64_t& ly, std::int64_t ry) const;
  void add1(Vec& lhs, const Vec& rhs, int m) const;
  void add2(Vec& lhs, const Vec& rhs, int m) const;
  Iter read_file(Iter first, Iter last, const char* filename) const;

public:
  CalshtDW();
  int calc_lh(const int* t, int m, std::int64_t& disc, std::int64_t& wait) const;
  int calc_sp(const int* t, std::int64_t& disc, std::int64_t& wait) const;
  int calc_sp(const int* t, int m, std::int64_t& disc, std::int64_t& wait) const {return calc_sp(t,disc,wait);}
  int calc_to(const int* t, std::int64_t& disc, std::int64_t& wait) const;
  int calc_to(const int* t, int m, std::int64_t& disc, std::int64_t& wait) const {return calc_to(t,disc,wait);}
  int operator()(const int* t, int m, int& mode, std::int64_t& disc, std::int64_t& wait) const;
  bool operator!() const;
};

#endif
