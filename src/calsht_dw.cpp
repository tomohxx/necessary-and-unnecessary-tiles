#include "calsht_dw.hpp"
#include "hash.hpp"
#include <algorithm>
#include <format>
#include <fstream>
#include <ranges>
#include <stdexcept>
constexpr int NUM_TIDS = 34;
const Hash<9> hash1;
const Hash<7> hash2;

void shift(uint64_t& lv, const uint64_t rv,
           uint64_t& lx, const uint64_t rx,
           uint64_t& ly, const uint64_t ry)
{
  if (lv == rv) {
    lx |= rx;
    ly |= ry;
  }
  else if (lv > rv) {
    lv = rv;
    lx = rx;
    ly = ry;
  }
}

CalshtDW::RArr CalshtDW::index1(const int n) const
{
  RArr ret{};

  std::fill_n(ret.begin(), 10, 14u);

  ret[0] = 0u;
  ret[1] = std::max(3u - n, 0u);
  ret[5] = std::max(2u - n, 0u);

  ret[10] = (n > 0 ? 1u : 0u);
  ret[11] = (n > 3 ? 1u : 0u);
  ret[15] = (n > 2 ? 1u : 0u);

  ret[20] = 0u;
  ret[21] = (n < 3 ? 1u : 0u);
  ret[25] = (n < 2 ? 1u : 0u);

  return ret;
}

void CalshtDW::add1(LArr& lhs, const RArr& rhs, const int m, const int w) const
{
  for (int j = m + 5; j >= 5; --j) {
    uint64_t sht = lhs[j] + rhs[0];
    uint64_t disc = (lhs[j + 10] << w) | rhs[10];
    uint64_t wait = (lhs[j + 20] << w) | rhs[20];

    shift(sht, lhs[0] + rhs[j],
          disc, (lhs[10] << w) | rhs[j + 10],
          wait, (lhs[20] << w) | rhs[j + 20]);

    for (int k = 5; k < j; ++k) {
      shift(sht, lhs[k] + rhs[j - k],
            disc, (lhs[k + 10] << w) | rhs[j - k + 10],
            wait, (lhs[k + 20] << w) | rhs[j - k + 20]);
      shift(sht, lhs[j - k] + rhs[k],
            disc, (lhs[j - k + 10] << w) | rhs[k + 10],
            wait, (lhs[j - k + 20] << w) | rhs[k + 20]);
    }

    lhs[j] = sht;
    lhs[j + 10] = disc;
    lhs[j + 20] = wait;
  }

  for (int j = m; j >= 0; --j) {
    uint64_t sht = lhs[j] + rhs[0];
    uint64_t disc = (lhs[j + 10] << w) | rhs[10];
    uint64_t wait = (lhs[j + 20] << w) | rhs[20];

    for (int k = 0; k < j; ++k) {
      shift(sht, lhs[k] + rhs[j - k],
            disc, (lhs[k + 10] << w) | rhs[j - k + 10],
            wait, (lhs[k + 20] << w) | rhs[j - k + 20]);
    }

    lhs[j] = sht;
    lhs[j + 10] = disc;
    lhs[j + 20] = wait;
  }
}

void CalshtDW::add2(LArr& lhs, const RArr& rhs, const int m, const int w) const
{
  const int j = m + 5;
  uint64_t sht = lhs[j] + rhs[0];
  uint64_t disc = (lhs[j + 10] << w) | rhs[10];
  uint64_t wait = (lhs[j + 20] << w) | rhs[20];

  shift(sht, lhs[0] + rhs[j],
        disc, (lhs[10] << w) | rhs[j + 10],
        wait, (lhs[20] << w) | rhs[j + 20]);

  for (int k = 5; k < j; ++k) {
    shift(sht, lhs[k] + rhs[j - k],
          disc, (lhs[k + 10] << w) | rhs[j - k + 10],
          wait, (lhs[k + 20] << w) | rhs[j - k + 20]);
    shift(sht, lhs[j - k] + rhs[k],
          disc, (lhs[j - k + 10] << w) | rhs[k + 10],
          wait, (lhs[j - k + 20] << w) | rhs[k + 20]);
  }

  lhs[j] = sht;
  lhs[j + 10] = disc;
  lhs[j + 20] = wait;
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

std::tuple<int, uint64_t, uint64_t> CalshtDW::calc_lh(const std::array<int, NUM_TIDS>& t,
                                                      const int m,
                                                      const bool three_player) const
{
  LArr ret{};

  std::ranges::copy(mp2[hash2(t.cbegin() + 27)], ret.begin());

  add1(ret, mp1[hash1(t.cbegin() + 18)], m, 9);
  add1(ret, mp1[hash1(t.cbegin() + 9)], m, 9);

  if (three_player) {
    add1(ret, index1(t[8]), m, 1);
    add2(ret, index1(t[0]), m, 8);
  }
  else {
    add2(ret, mp1[hash1(t.cbegin())], m, 9);
  }

  return {static_cast<int>(ret[m + 5]), ret[m + 15], ret[m + 25]};
}

std::tuple<int, uint64_t, uint64_t> CalshtDW::calc_sp(const std::array<int, NUM_TIDS>& t,
                                                      const bool three_player) const
{
  int pair = 0;
  int kind = 0;
  uint64_t disc = 0ul;  // 不要牌
  uint64_t wait = 0ul;  // 有効牌
  uint64_t disc_ = 0ul; // 不要牌候補
  uint64_t wait_ = 0ul; // 有効牌候補

  for (int i = 0; i < NUM_TIDS; ++i) {
    if (three_player && i > 0 && i < 8) continue;

    if (t[i] == 0) {
      wait_ |= 1ul << i;
    }
    else if (t[i] == 1) {
      ++kind;
      disc_ |= 1ul << i;
      wait |= 1ul << i;
    }
    else if (t[i] == 2) {
      ++kind;
      ++pair;
    }
    else if (t[i] > 2) {
      ++kind;
      ++pair;
      disc |= 1ul << i;
    }
  }

  if (kind > 7) disc |= disc_;
  else if (kind < 7) wait |= wait_;

  return {7 - pair + (kind < 7 ? 7 - kind : 0), disc, wait};
}

std::tuple<int, uint64_t, uint64_t> CalshtDW::calc_to(const std::array<int, NUM_TIDS>& t) const
{
  int pair = 0;
  int kind = 0;
  uint64_t disc = 0ul;  // 不要牌
  uint64_t wait = 0ul;  // 有効牌
  uint64_t disc_ = 0ul; // 不要牌候補
  uint64_t wait_ = 0ul; // 有効牌候補

  for (const int i : {0, 8, 9, 17, 18, 26, 27, 28, 29, 30, 31, 32, 33}) {
    if (t[i] == 0) {
      wait |= 1ul << i;
    }
    else if (t[i] == 1) {
      ++kind;
      wait_ |= 1ul << i;
    }
    else if (t[i] == 2) {
      ++kind;
      ++pair;
      disc_ |= 1ul << i;
    }
    else if (t[i] > 2) {
      ++kind;
      ++pair;
      disc |= 1ul << i;
    }
  }

  for (const int i : {1, 2, 3, 4, 5, 6, 7, 10, 11, 12, 13, 14, 15, 16, 19, 20, 21, 22, 23, 24, 25}) {
    if (t[i] > 0) disc |= 1ul << i;
  }

  if (pair >= 2) disc |= disc_;
  else if (pair == 0) wait |= wait_;

  return {14 - kind - (pair > 0 ? 1 : 0), disc, wait};
}

void CalshtDW::initialize(const std::string& dir)
{
  read_file(mp1.begin(), mp1.end(), std::filesystem::path(dir) / "index_dw_s.bin");
  read_file(mp2.begin(), mp2.end(), std::filesystem::path(dir) / "index_dw_h.bin");
}

std::tuple<int, int, uint64_t, uint64_t> CalshtDW::operator()(const std::array<int, 34>& t,
                                                              const int m,
                                                              const int mode,
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

    if (ENABLE_NYANTEN && n > 14) {
      throw std::invalid_argument(std::format("Invalid sum of hand's tiles: {}", n));
    }

    if (m < 0 || m > 4) {
      throw std::invalid_argument(std::format("Invalid sum of hands's melds: {}", m));
    }

    if (mode < 0 || mode > 7) {
      throw std::invalid_argument(std::format("Invalid caluculation mode: {}", mode));
    }
  }

  std::tuple<int, int, uint64_t, uint64_t> ret{1024, 0, 0, 0};

  if (mode & 1) {
    if (auto [sht, disc, wait] = calc_lh(t, m, three_player); sht < std::get<0>(ret)) {
      ret = {sht, 1, disc, wait};
    }
    else if (sht == std::get<0>(ret)) {
      std::get<1>(ret) |= 1;
      std::get<2>(ret) |= disc;
      std::get<3>(ret) |= wait;
    }
  }

  if ((mode & 2) && m == 4) {
    if (auto [sht, disc, wait] = calc_sp(t, three_player); sht < std::get<0>(ret)) {
      ret = {sht, 2, disc, wait};
    }
    else if (sht == std::get<0>(ret)) {
      std::get<1>(ret) |= 2;
      std::get<2>(ret) |= disc;
      std::get<3>(ret) |= wait;
    }
  }

  if ((mode & 4) && m == 4) {
    if (auto [sht, disc, wait] = calc_to(t); sht < std::get<0>(ret)) {
      ret = {sht, 4, disc, wait};
    }
    else if (sht == std::get<0>(ret)) {
      std::get<1>(ret) |= 4;
      std::get<2>(ret) |= disc;
      std::get<3>(ret) |= wait;
    }
  }

  return ret;
}
