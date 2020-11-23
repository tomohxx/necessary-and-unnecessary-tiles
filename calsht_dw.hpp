#ifndef CALSHT_DW_HPP
#define CALSHT_DW_HPP

#include <cstdint>
#include <filesystem>
#include <tuple>
#include <vector>
#include "constant.hpp"

class CalshtDW{
private:
  using Vec  = std::vector<std::int64_t>;
  using Iter = std::vector<Vec>::iterator;

  std::vector<Vec> mp1;
  std::vector<Vec> mp2;

#ifdef THREE_PLAYER
  Vec index1(int n) const;
#endif
  void shift(int& lv, int rv, std::int64_t& lx, std::int64_t rx, std::int64_t& ly, std::int64_t ry) const;
  void add1(Vec& lhs, const Vec& rhs, int m) const;
  void add2(Vec& lhs, const Vec& rhs, int m) const;
  Iter read_file(Iter first, Iter last, std::filesystem::path file) const;
  std::tuple<int,std::int64_t,std::int64_t> calc_lh(const int* t, int m) const;
  std::tuple<int,std::int64_t,std::int64_t> calc_sp(const int* t) const;
  std::tuple<int,std::int64_t,std::int64_t> calc_to(const int* t) const;

public:
  CalshtDW() : mp1(std::vector<Vec>(1953125,Vec(30))), mp2(std::vector<Vec>(78125,Vec(30))) {}
  void initialize(std::filesystem::path dir);
  std::tuple<int,int,std::int64_t,std::int64_t> operator()(const std::vector<int>& t, int m, int mode) const;
};

#endif
