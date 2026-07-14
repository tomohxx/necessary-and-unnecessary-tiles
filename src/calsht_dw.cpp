#include "hash.hpp"
#include <algorithm>
#include <format>
#include <fstream>
#include <mahjong/calsht_dw.hpp>
#include <ranges>
#include <stdexcept>
#include <type_traits>
constexpr int NUM_TIDS = 34;
const Hash<9> hash1;
const Hash<7> hash2;

constexpr std::array<std::array<uint32_t, 30>, 5> index1 = {{
    {0u, 3u, 14u, 14u, 14u, 2u, 14u, 14u, 14u, 14u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u},
    {0u, 2u, 14u, 14u, 14u, 1u, 14u, 14u, 14u, 14u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u},
    {0u, 1u, 14u, 14u, 14u, 0u, 14u, 14u, 14u, 14u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u},
    {0u, 0u, 14u, 14u, 14u, 0u, 14u, 14u, 14u, 14u, 1u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u},
    {0u, 0u, 14u, 14u, 14u, 0u, 14u, 14u, 14u, 14u, 1u, 1u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u},
}};

void shift(uint64_t& lv, const uint64_t rv, uint64_t& lx, const uint64_t rx)
{
  if (lv == rv) {
    lx |= rx;
  }
  else if (lv > rv) {
    lv = rv;
    lx = rx;
  }
}

namespace mahjong {
  template <class CalcMode>
  struct Offset {};

  template <>
  struct Offset<CalcDisc> {
    static constexpr int value = 10;
  };

  template <>
  struct Offset<CalcWait> {
    static constexpr int value = 20;
  };

  template <class CalcMode>
  void CalshtDW::add1(LArr& lhs, const RArr& rhs, const int m, const int w, const CalcMode&) const
  {
    for (int j = m + 5; j >= 5; --j) {
      uint64_t sht = lhs[j] + rhs[0];
      uint64_t disc = (lhs[j + Offset<CalcMode>::value] << w) | rhs[Offset<CalcMode>::value];

      shift(sht, lhs[0] + rhs[j], disc, (lhs[Offset<CalcMode>::value] << w) | rhs[j + Offset<CalcMode>::value]);

      for (int k = 5; k < j; ++k) {
        shift(sht, lhs[k] + rhs[j - k], disc, (lhs[k + Offset<CalcMode>::value] << w) | rhs[j - k + Offset<CalcMode>::value]);
        shift(sht, lhs[j - k] + rhs[k], disc, (lhs[j - k + Offset<CalcMode>::value] << w) | rhs[k + Offset<CalcMode>::value]);
      }

      lhs[j] = sht;
      lhs[j + Offset<CalcMode>::value] = disc;
    }

    for (int j = m; j >= 0; --j) {
      uint64_t sht = lhs[j] + rhs[0];
      uint64_t disc = (lhs[j + Offset<CalcMode>::value] << w) | rhs[Offset<CalcMode>::value];

      for (int k = 0; k < j; ++k) {
        shift(sht, lhs[k] + rhs[j - k], disc, (lhs[k + Offset<CalcMode>::value] << w) | rhs[j - k + Offset<CalcMode>::value]);
      }

      lhs[j] = sht;
      lhs[j + Offset<CalcMode>::value] = disc;
    }
  }

  template <class CalcMode>
  void CalshtDW::add2(LArr& lhs, const RArr& rhs, const int m, const int w, const CalcMode&) const
  {
    const int j = m + 5;
    uint64_t sht = lhs[j] + rhs[0];
    uint64_t disc = (lhs[j + Offset<CalcMode>::value] << w) | rhs[Offset<CalcMode>::value];

    shift(sht, lhs[0] + rhs[j], disc, (lhs[Offset<CalcMode>::value] << w) | rhs[j + Offset<CalcMode>::value]);

    for (int k = 5; k < j; ++k) {
      shift(sht, lhs[k] + rhs[j - k], disc, (lhs[k + Offset<CalcMode>::value] << w) | rhs[j - k + Offset<CalcMode>::value]);
      shift(sht, lhs[j - k] + rhs[k], disc, (lhs[j - k + Offset<CalcMode>::value] << w) | rhs[k + Offset<CalcMode>::value]);
    }

    lhs[j] = sht;
    lhs[j + Offset<CalcMode>::value] = disc;
  }

  void CalshtDW::read_file(Iter first, Iter last, std::filesystem::path file) const
  {
    std::ifstream fin(file, std::ios_base::in | std::ios_base::binary);

    if (!fin) {
      throw std::runtime_error(std::format("Reading file does not exist: {}", file.string()));
    }

    for (; first != last; ++first) {
      for (int j = 0; j < 10; ++j) {
        RArr::value_type tmp;

        fin.read(reinterpret_cast<char*>(&tmp), sizeof(RArr::value_type));
        (*first)[j] = tmp & 0xF;
        (*first)[j + 10] = (tmp >> 4) & 0x1FF;
        (*first)[j + 20] = (tmp >> 13) & 0x1FF;
      }
    }
  }

  template <class CalcMode>
  std::tuple<int, uint64_t> CalshtDW::calc_lh(const std::array<int, NUM_TIDS>& t,
                                              const int m,
                                              const CalcMode& calc_mode,
                                              const bool three_player) const
  {
    LArr ret{};

    std::ranges::copy(mp2[hash2(t.cbegin() + 27)], ret.begin());

    add1(ret, mp1[hash1(t.cbegin() + 18)], m, 9, calc_mode);
    add1(ret, mp1[hash1(t.cbegin() + 9)], m, 9, calc_mode);

    if (three_player) {
      add1(ret, index1[t[8]], m, 1, calc_mode);
      add2(ret, index1[t[0]], m, 8, calc_mode);
    }
    else {
      add2(ret, mp1[hash1(t.cbegin())], m, 9, calc_mode);
    }

    return {static_cast<int>(ret[m + 5]), ret[m + 5 + Offset<CalcMode>::value]};
  }

  std::tuple<int, uint64_t> CalshtDW::calc_sp(const std::array<int, NUM_TIDS>& t,
                                              const CalcDisc&,
                                              const bool three_player) const
  {
    int pair = 0;
    int kind = 0;
    uint64_t disc = 0ull;  // 不要牌
    uint64_t disc_ = 0ull; // 不要牌候補

    for (int i = 0; i < NUM_TIDS; ++i) {
      if (three_player && i > 0 && i < 8) continue;
      if (t[i] >= 1) ++kind;
      if (t[i] >= 2) ++pair;
      if (t[i] == 1) disc_ |= 1ull << i;
      if (t[i] > 2) disc |= 1ull << i;
    }

    if (kind > 7) disc |= disc_;

    return {7 - pair + (kind < 7 ? 7 - kind : 0), disc};
  }

  std::tuple<int, uint64_t> CalshtDW::calc_sp(const std::array<int, NUM_TIDS>& t,
                                              const CalcWait&,
                                              const bool three_player) const
  {
    int pair = 0;
    int kind = 0;
    uint64_t wait = 0ul;  // 有効牌
    uint64_t wait_ = 0ul; // 有効牌候補

    for (int i = 0; i < NUM_TIDS; ++i) {
      if (three_player && i > 0 && i < 8) continue;
      if (t[i] >= 1) ++kind;
      if (t[i] >= 2) ++pair;
      if (t[i] == 0) wait_ |= 1ull << i;
      if (t[i] == 1) wait |= 1ull << i;
    }

    if (kind < 7) wait |= wait_;

    return {7 - pair + (kind < 7 ? 7 - kind : 0), wait};
  }

  std::tuple<int, uint64_t> CalshtDW::calc_to(const std::array<int, NUM_TIDS>& t, const CalcDisc&) const
  {
    int pair = 0;
    int kind = 0;
    uint64_t disc = 0ull;  // 不要牌
    uint64_t disc_ = 0ull; // 不要牌候補

    for (const int i : {0, 8, 9, 17, 18, 26, 27, 28, 29, 30, 31, 32, 33}) {
      if (t[i] >= 1) ++kind;
      if (t[i] >= 2) ++pair;
      if (t[i] == 2) disc_ |= 1ull << i;
      if (t[i] > 2) disc |= 1ull << i;
    }

    for (const int i : {1, 2, 3, 4, 5, 6, 7, 10, 11, 12, 13, 14, 15, 16, 19, 20, 21, 22, 23, 24, 25}) {
      if (t[i] > 0) disc |= 1ul << i;
    }

    if (pair >= 2) disc |= disc_;

    return {14 - kind - (pair > 0 ? 1 : 0), disc};
  }

  std::tuple<int, uint64_t> CalshtDW::calc_to(const std::array<int, NUM_TIDS>& t, const CalcWait&) const
  {
    int pair = 0;
    int kind = 0;
    uint64_t wait = 0ul;  // 有効牌
    uint64_t wait_ = 0ul; // 有効牌候補

    for (const int i : {0, 8, 9, 17, 18, 26, 27, 28, 29, 30, 31, 32, 33}) {
      if (t[i] >= 1) ++kind;
      if (t[i] >= 2) ++pair;
      if (t[i] == 0) wait |= 1ull << i;
      if (t[i] == 1) wait_ |= 1ull << i;
    }

    if (pair == 0) wait |= wait_;

    return {14 - kind - (pair > 0 ? 1 : 0), wait};
  }

  CalshtDW::CalshtDW(const std::string& dir)
#ifndef ENABLE_NYANTEN
      : mp1(1953125), mp2(78125)
#else
      : mp1(405350), mp2(43130)
#endif
  {
    read_file(mp1.begin(), mp1.end(), std::filesystem::path(dir) / "index_dw_s.bin");
    read_file(mp2.begin(), mp2.end(), std::filesystem::path(dir) / "index_dw_h.bin");
  }

  template <class CalcMode>
  std::tuple<int, unsigned int, uint64_t> CalshtDW::operator()(const std::array<int, 34>& t,
                                                               const int m,
                                                               const unsigned int mode,
                                                               const CalcMode& calc_mode,
                                                               const bool check_hand,
                                                               const bool three_player) const
  {
    if (check_hand) {
      int n = 0;

      for (int i = 0; i < NUM_TIDS; ++i) {
        if (t[i] < 0 || t[i] > 4) {
          throw std::invalid_argument(std::format("Invalid number of hand's tiles at {}: {}", i, t[i]));
        }

        n += t[i];
      }

#ifdef ENABLE_NYANTEN
      if (n > 14) {
        throw std::invalid_argument(std::format("Invalid sum of hand's tiles: {}", n));
      }
#endif

      if (m < 0 || m > 4) {
        throw std::invalid_argument(std::format("Invalid sum of hands's melds: {}", m));
      }

      if (mode > 7u) {
        throw std::invalid_argument(std::format("Invalid caluculation mode: {}", mode));
      }
    }

    std::tuple<int, unsigned int, uint64_t> ret{1024, 0u, 0ull};

    if (mode & 1u) {
      if (auto [sht, disc_or_wait] = calc_lh(t, m, calc_mode, three_player); sht < std::get<0>(ret)) {
        ret = {sht, 1u, disc_or_wait};
      }
      else if (sht == std::get<0>(ret)) {
        std::get<1>(ret) |= 1;
        std::get<2>(ret) |= disc_or_wait;
      }
    }

    if ((mode & 2u) && m == 4) {
      if (auto [sht, disc_or_wait] = calc_sp(t, calc_mode, three_player); sht < std::get<0>(ret)) {
        ret = {sht, 2u, disc_or_wait};
      }
      else if (sht == std::get<0>(ret)) {
        std::get<1>(ret) |= 2;
        std::get<2>(ret) |= disc_or_wait;
      }
    }

    if ((mode & 4u) && m == 4) {
      if (auto [sht, disc_or_wait] = calc_to(t, calc_mode); sht < std::get<0>(ret)) {
        ret = {sht, 4u, disc_or_wait};
      }
      else if (sht == std::get<0>(ret)) {
        std::get<1>(ret) |= 4;
        std::get<2>(ret) |= disc_or_wait;
      }
    }

    return ret;
  }

  template std::tuple<int, unsigned int, uint64_t>
  CalshtDW::operator()<CalcDisc>(const std::array<int, 34>& t,
                                 int m,
                                 unsigned int mode,
                                 const CalcDisc& calc_mode,
                                 bool check_hand,
                                 bool three_player) const;

  template std::tuple<int, unsigned int, uint64_t>
  CalshtDW::operator()<CalcWait>(const std::array<int, 34>& t,
                                 int m,
                                 unsigned int mode,
                                 const CalcWait& calc_mode,
                                 bool check_hand,
                                 bool three_player) const;
}
