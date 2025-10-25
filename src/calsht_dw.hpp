#ifndef CALSHT_DW_HPP
#define CALSHT_DW_HPP

#include <filesystem>
#include <tuple>
#include <vector>
#ifndef ENABLE_NYANTEN
#define ENABLE_NYANTEN false
#endif

class CalshtDW {
private:
  using LVec = std::vector<uint64_t>;
  using RVec = std::vector<uint32_t>;
  using Iter = std::vector<RVec>::iterator;

  std::vector<RVec> mp1;
  std::vector<RVec> mp2;

  RVec index1(int n) const;
  void add1(LVec& lhs, const RVec& rhs, int m, int w) const;
  void add2(LVec& lhs, const RVec& rhs, int m, int w) const;
  void read_file(Iter first, Iter last, std::filesystem::path file) const;
  std::tuple<int, uint64_t, uint64_t> calc_lh(const std::array<int, 34>& t,
                                              int m,
                                              const bool three_player = false) const;
  std::tuple<int, uint64_t, uint64_t> calc_sp(const std::array<int, 34>& t,
                                              bool three_player = false) const;
  std::tuple<int, uint64_t, uint64_t> calc_to(const std::array<int, 34>& t) const;

public:
  CalshtDW()
      : mp1(ENABLE_NYANTEN ? 405350 : 1953125, RVec(30)),
        mp2(ENABLE_NYANTEN ? 43130 : 78125, RVec(30)) {}
  void initialize(const std::string& dir);
  std::tuple<int, int, uint64_t, uint64_t> operator()(const std::array<int, 34>& t,
                                                      int m,
                                                      int mode,
                                                      bool check_hand = false,
                                                      bool three_player = false) const;
};

#endif
