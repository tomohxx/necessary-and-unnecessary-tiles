#include "calsht_dw.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <filesystem>
#include <format>
#include <iostream>
#include <random>
#include <utility>
#ifndef INDEX_FILE_PATH
#define INDEX_FILE_PATH std::filesystem::current_path()
#endif
#ifndef FIX_RANDOM_SEED
#define FIX_RANDOM_SEED false
#endif

int main(int argc, char* argv[])
{
  if (argc != 3) {
    std::cerr << std::format("Usage: {} [Number of Rounds] [Whether three player]\n", argv[0]);
    return 1;
  }

  constexpr int NUM_TIDS = 34;
  constexpr int NUM_TILES = 13;
  constexpr int NUM_TURNS = 18;
  constexpr int MAX_SHT = 8;
  constexpr int MODE = 7;
  const int NUM_ROUNDS = std::atoi(argv[1]);
  const bool THREE_PLAYER = std::atoi(argv[2]);
  std::vector<int> hand(NUM_TIDS, 0);
  std::vector<int> wall34(NUM_TIDS, 0);
  std::vector<int> wall136;
  std::array<int, NUM_TIDS> cnt{};
  std::array<std::array<int, MAX_SHT>, NUM_TURNS> table{};
  std::conditional<FIX_RANDOM_SEED,
                   std::integral_constant<int, 0>,
                   std::random_device>::type seed_gen;
  std::mt19937_64 rand(seed_gen());
  CalshtDW calsht;

  calsht.initialize(INDEX_FILE_PATH);
  wall136.reserve(NUM_TIDS * 4);

  for (int i = 0; i < NUM_TIDS; ++i) {
    if (THREE_PLAYER && i > 0 && i < 8) continue;

    for (int j = 0; j < 4; ++j) {
      wall136.push_back(i);
    }
  }

  for (int i = 0; i < NUM_ROUNDS; ++i) {
    std::fill(hand.begin(), hand.end(), 0);
    std::fill(wall34.begin(), wall34.end(), 4);
    std::shuffle(wall136.begin(), wall136.end(), rand);

    for (int j = 0; j < NUM_TILES; ++j) {
      ++hand[wall136[j]];
      --wall34[j];
    }

    for (int j = 0; j < NUM_TURNS; ++j) {
      ++hand[wall136[j + NUM_TILES]];
      --wall34[j + NUM_TILES];

      const auto [sht, mode, disc, wait] = calsht(hand,
                                                  NUM_TILES / 3,
                                                  MODE,
                                                  false,
                                                  THREE_PLAYER);

      ++table[j][sht];

      if (sht == 0) {
        for (int k = j + 1; k < NUM_TURNS; ++k) {
          ++table[k][0];
        }

        break;
      }

      cnt.fill(-1);

      for (int k = 0; k < NUM_TIDS; ++k) {
        if (disc & (1ull << k)) {
          --hand[k];

          const auto [sht_, mode_, disc_, wait_] = calsht(hand,
                                                          NUM_TILES / 3,
                                                          MODE,
                                                          false,
                                                          THREE_PLAYER);

          cnt[k] = 0;

          for (int l = 0; l < NUM_TIDS; ++l) {
            cnt[k] += wait_ & (1ull << l) ? wall34[l] : 0;
          }

          ++hand[k];
        }
      }

      const int tile = std::distance(cnt.begin(),
                                     std::max_element(cnt.begin(), cnt.end()));

      assert(hand[tile] > 0);
      --hand[tile];
    }
  }

  std::cout << std::format("{:<24s}{:<16d}\n", "Number of Tiles", NUM_TILES);
  std::cout << std::format("{:<24s}{:<16d}\n", "Number of Rounds", NUM_ROUNDS);
  std::cout << std::format("Turn\tShanten Number (-1 - 6)\tHora\tTempai\tExp.\n");

  for (int i = 0; i < NUM_TURNS; ++i) {
    std::cout << i << "\t";

    double ev = 0;

    for (int j = 0; j < MAX_SHT; ++j) {
      ev += (j - 1) * table[i][j];
      std::cout << table[i][j] << "\t";
    }

    std::cout << std::format("{:.6f}\t{:.6f}\t{:.6f}\n",
                             1. * table[i][0] / NUM_ROUNDS,
                             1. * (table[i][0] + table[i][1]) / NUM_ROUNDS,
                             ev / NUM_ROUNDS);
  }

  return 0;
}
