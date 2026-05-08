#include <algorithm>
#include <array>
#include <cstdint>
#include <execution>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#ifdef ENABLE_PARALLEL
#define POLICY (std::execution::par)
#else
#define POLICY (std::execution::seq)
#endif
constexpr int MAX_SHT = 14;
constexpr int MAX_TILES = 14;
template <int N>
using Hand = std::array<int, N>;
template <int N>
using Hands = std::vector<std::pair<Hand<N>, std::size_t>>;

struct Delta {
  int a;
  int b;
  int c;
  int h;
  int m;
};

struct Data {
  uint32_t sht;
  uint32_t disc;
  uint32_t wait;

  operator uint32_t() const { return sht | (disc << 4) | (wait << 13); }
};

void chmin(Data& x, const Data y)
{
  if (x.sht > y.sht) {
    x.sht = y.sht;
    x.disc = y.disc;
    x.wait = y.wait;
  }
  else if (x.sht == y.sht) {
    x.disc |= y.disc;
    x.wait |= y.wait;
  }
}

template <int N>
void dp(const Hand<N>& hand, const std::vector<Delta>& deltas, std::array<uint32_t, 10>& sht)
{
  using std::array;

  array<array<array<array<array<Data, 5>, 2>, 5>, 5>, N + 1> table;

  std::fill(table[0][0][0][0].begin(), table[N][4][4][1].end(), Data{MAX_SHT, 0u, 0u});

  table[0][0][0][0][0] = {0, 0u, 0u};

  for (int n = 0; n < N; ++n) {
    for (const auto& delta : deltas) {
      for (int a = 0; a <= 4 - delta.a; ++a) {
        for (int b = 0; b <= std::min(4 - delta.b, a); ++b) {
          for (int h = 0; h <= 1 - delta.h; ++h) {
            for (int m = 0; m <= 4 - delta.m; ++m) {
              const auto& tmp = table[n][a][b][h][m];

              if (tmp.sht == MAX_SHT) continue;

              const auto d = a + delta.a - hand[n];

              chmin(table[n + 1][b + delta.b][delta.c][h + delta.h][m + delta.m],
                    {tmp.sht + std::max(d, 0),
                     d < 0 ? (tmp.disc | 1u << n) : tmp.disc,
                     d > 0 ? (tmp.wait | 1u << n) : tmp.wait});
            }
          }
        }
      }
    }
  }

  std::copy(table[N][0][0][0].cbegin(), table[N][0][0][1].cend(), sht.begin());
}

template <int N>
void deal(const int n, const int m, Hand<N>& hand, Hands<N>& hands)
{
  if (n >= N) {
    hands.push_back(std::make_pair(hand, hands.size()));
  }
  else {
#ifdef ENABLE_NYANTEN
    for (int i = 0; i <= std::min(m, 4); ++i) {
#else
    for (int i = 0; i <= 4; ++i) {
#endif
      hand[n] = i;
      deal<N>(n + 1, m - i, hand, hands);
    }
  }
}

int main()
{
  {
    std::ofstream fout("index_dw_s.bin", std::ios_base::out | std::ios_base::binary);

    if (!fout) {
      std::cerr << "Failed to open \"index_dw_s.bin\"" << std::endl;
      return 1;
    }

    Hand<9> hand{};
    Hands<9> hands;

    hands.reserve(1953125); // 5^9

    const std::vector<Delta> deltas = {
        {0, 0, 0, 0, 0},
        {1, 1, 1, 0, 1},
        {2, 2, 2, 0, 2},
        {3, 0, 0, 0, 1},
        {4, 1, 1, 0, 2},
        {2, 0, 0, 1, 0},
        {3, 1, 1, 1, 1},
        {4, 2, 2, 1, 2},
    };

    deal<9>(0, MAX_TILES, hand, hands);

    std::vector<std::array<uint32_t, 10>> dists(hands.size(), std::array<uint32_t, 10>{});

    std::for_each(POLICY, hands.cbegin(), hands.cend(),
                  [&deltas, &dists](const auto& hand_hash) {
                    dp<9>(hand_hash.first, deltas, dists[hand_hash.second]);
                  });

    std::for_each(dists.cbegin(), dists.cend(),
                  [&fout](const auto& dist) {
                    fout.write(reinterpret_cast<const char*>(dist.data()), dist.size() * sizeof(uint32_t));
                  });
  }

  {
    std::ofstream fout("index_dw_h.bin", std::ios_base::out | std::ios_base::binary);

    if (!fout) {
      std::cerr << "Failed to open \"index_dw_h.bin\"" << std::endl;
      return 1;
    }

    Hand<7> hand{};
    Hands<7> hands;

    hands.reserve(78125); // 5^7

    const std::vector<Delta> deltas = {
        {0, 0, 0, 0, 0},
        {3, 0, 0, 0, 1},
        {2, 0, 0, 1, 0},
    };

    deal<7>(0, MAX_TILES, hand, hands);

    std::vector<std::array<uint32_t, 10>> dists(hands.size(), std::array<uint32_t, 10>{});

    std::for_each(POLICY, hands.cbegin(), hands.cend(),
                  [&deltas, &dists](const auto& hand_hash) {
                    dp<7>(hand_hash.first, deltas, dists[hand_hash.second]);
                  });

    std::for_each(dists.cbegin(), dists.cend(),
                  [&fout](const auto& dist) {
                    fout.write(reinterpret_cast<const char*>(dist.data()), dist.size() * sizeof(uint32_t));
                  });
  }

  return 0;
}
