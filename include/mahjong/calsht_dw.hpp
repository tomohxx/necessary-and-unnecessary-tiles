#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <tuple>
#include <vector>

namespace mahjong {
  struct CalcDisc {};
  struct CalcWait {};

  class CalshtDW {
  private:
    using LArr = std::array<uint64_t, 30>;
    using RArr = std::array<uint32_t, 30>;
    using Iter = std::vector<RArr>::iterator;

    std::vector<RArr> mp1;
    std::vector<RArr> mp2;

    template <class CalcMode>
    void add1(LArr& lhs, const RArr& rhs, int m, int w, const CalcMode&) const;
    template <class CalcMode>
    void add2(LArr& lhs, const RArr& rhs, int m, int w, const CalcMode&) const;
    void read_file(Iter first, Iter last, std::filesystem::path file) const;
    template <class CalcMode>
    std::tuple<int, uint64_t> calc_lh(const std::array<int, 34>& t,
                                      int m,
                                      const CalcMode& calc_mode,
                                      const bool three_player = false) const;
    std::tuple<int, uint64_t> calc_sp(const std::array<int, 34>& t,
                                      const CalcDisc&,
                                      bool three_player = false) const;
    std::tuple<int, uint64_t> calc_sp(const std::array<int, 34>& t,
                                      const CalcWait&,
                                      bool three_player = false) const;
    std::tuple<int, uint64_t> calc_to(const std::array<int, 34>& t,
                                      const CalcDisc&,
                                      bool three_player = false) const;
    std::tuple<int, uint64_t> calc_to(const std::array<int, 34>& t,
                                      const CalcWait&,
                                      bool) const;

  public:
    CalshtDW(const std::string& dir);
    template <class CalcMode>
    std::tuple<int, unsigned int, uint64_t> operator()(const std::array<int, 34>& t,
                                                       int m,
                                                       unsigned int mode,
                                                       const CalcMode& calc_mode,
                                                       bool check_hand = false,
                                                       bool three_player = false) const;
  };
}
