#ifndef CALSHT_DW_HPP
#define CALSHT_DW_HPP

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <tuple>
#include <vector>
#ifndef ENABLE_NYANTEN
#define ENABLE_NYANTEN (false)
#endif

class CalshtDW {
private:
  using LArr = std::array<uint64_t, 30>;
  using RArr = std::array<uint32_t, 30>;
  using Iter = std::vector<RArr>::iterator;

  std::vector<RArr> mp1;
  std::vector<RArr> mp2;

  void add1(LArr& lhs, const RArr& rhs, int m, int w) const;
  void add2(LArr& lhs, const RArr& rhs, int m, int w) const;
  void read_file(Iter first, Iter last, std::filesystem::path file) const;
  std::tuple<int, uint64_t, uint64_t> calc_lh(const std::array<int, 34>& t,
                                              int m,
                                              const bool three_player = false) const;
  std::tuple<int, uint64_t, uint64_t> calc_sp(const std::array<int, 34>& t,
                                              bool three_player = false) const;
  std::tuple<int, uint64_t, uint64_t> calc_to(const std::array<int, 34>& t) const;

public:
  CalshtDW()
      : mp1(ENABLE_NYANTEN ? 405350 : 1953125),
        mp2(ENABLE_NYANTEN ? 43130 : 78125) {}
  void initialize(const std::string& dir);
  std::tuple<int, int, uint64_t, uint64_t> operator()(const std::array<int, 34>& t,
                                                      int m,
                                                      int mode,
                                                      bool check_hand = false,
                                                      bool three_player = false) const;
};

#endif
