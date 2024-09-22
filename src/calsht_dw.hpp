#ifndef CALSHT_DW_HPP
#define CALSHT_DW_HPP

#include <cstdint>
#include <filesystem>
#include <tuple>
#include <vector>

class CalshtDW {
private:
  using LVec = std::vector<int64_t>;
  using RVec = std::vector<int32_t>;
  using Iter = std::vector<RVec>::iterator;

  std::vector<RVec> mp1;
  std::vector<RVec> mp2;

#ifdef THREE_PLAYER
  RVec index1(int n) const;
#endif
  void shift(int& lv, int rv, int64_t& lx, int64_t rx, int64_t& ly, int64_t ry) const;
  void add1(LVec& lhs, const RVec& rhs, int m) const;
  void add2(LVec& lhs, const RVec& rhs, int m) const;
  Iter read_file(Iter first, Iter last, std::filesystem::path file) const;
  std::tuple<int, int64_t, int64_t> calc_lh(const int* t, int m) const;
  std::tuple<int, int64_t, int64_t> calc_sp(const int* t) const;
  std::tuple<int, int64_t, int64_t> calc_to(const int* t) const;

public:
  CalshtDW()
      : mp1(1953125, RVec(30)), mp2(78125, RVec(30)) {}
  void initialize(const std::string& dir);
  std::tuple<int, int, int64_t, int64_t> operator()(const std::vector<int>& t, int m, int mode) const;
};

#endif
