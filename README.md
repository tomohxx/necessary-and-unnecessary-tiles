# Necessary and Unnecessary Tiles

A tool for calculating shanten numbers and identifying necessary and unnecessary tiles in mahjong.

[Read this in Japanese (日本語).](README.ja.md)

## What are necessary tiles and unnecessary tiles in mahjong?

- Necessary tiles:
   - tiles needed to win with the **fewest tile exchanges**.
   - drawing one of them decreases the shanten number.
   - called "yuukouhai" or "ukeire" in Japanese.
- Unnecessary tiles:
   - tiles not needed to win with the **fewest tile exchanges**.
   - discarding one of them keeps the shanten number unchanged.
   - called "yojouhai" in Japanese.

## Build

### Debug mode

```
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Debug
$ make
```

### Release mode

```
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ make
```
> [!NOTE]
> A compiler compatible with C++20 or later is required.

### Compile options

#### `-DENABLE_NYANTEN`

This enables the table search algorithm to use the minimal perfect hash function used in Cryolite's nyanten[^1][^2]. Enabling this option can reduce the size of the tables. However, the number of tiles in a hand for which shanten numbers can be calculated is limited to 14 or less.

[^1]: https://github.com/Cryolite/nyanten
[^2]: https://www.slideshare.net/slideshow/a-fast-and-space-efficient-algorithm-for-calculating-deficient-numbers-a-k-a-shanten-numbers-pptx/269706666

#### `-DFIX_RANDOM_SEED`

It fixes the random seed used in the example program.

#### Building tables

Build the parameter tables required for calculating necessary and unnecessary tiles. This creates `index_dw_h.bin` and `index_dw_s.bin`.

```
$ ./mkind
```

> [!TIP]
> Intel Threading Building Blocks (`TBB`) is optional and, if available, will be automatically linked to enable parallel builds.

## Usage

1. Prepare a `std::array<int, 34>` array representing a hand.
   - The `n`-th element stores the number of copies of the `n`-th tile.

      |         | 1           | 2            | 3           | 4            | 5            | 6            | 7          | 8       | 9       |
      | :------ | :---------- | :----------- | :---------- | :----------- | :----------- | :----------- | :--------- | :------ | :------ |
      | *Manzu* | 0 (1m)      | 1 (2m)       | 2 (3m)      | 3 (4m)       | 4 (5m)       | 5 (6m)       | 6 (7m)     | 7 (8m)  | 8 (9m)  |
      | *Pinzu* | 9 (1p)      | 10 (2p)      | 11 (3p)     | 12 (4p)      | 13 (5p)      | 14 (6p)      | 15 (7p)    | 16 (8p) | 17 (9p) |
      | *Souzu* | 18 (1s)     | 19 (2s)      | 20 (3s)     | 21 (4s)      | 22 (5s)      | 23 (6s)      | 24 (7s)    | 25 (8s) | 26 (9s) |
      | *Jihai* | 27 (*East*) | 28 (*South*) | 29 (*West*) | 30 (*North*) | 31 (*White*) | 32 (*Green*) | 33 (*Red*) |         |         |

   - For example, if the hand is *123m245779p13555z*, define the array as follows.

      ```cpp
      std::array<int, 34> hand = {
         1, 1, 1, 0, 0, 0, 0, 0, 0, // Manzu
         0, 1, 0, 1, 1, 0, 2, 0, 1, // Pinzu
         0, 0, 0, 0, 0, 0, 0, 0, 0, // Souzu
         1, 0, 1, 0, 3, 0, 0        // Jihai
      };
      ```

1. Calculate the shanten number and the necessary and unnecessary tiles.
   ```cpp
   std::tuple<int, unsigned int, uint64_t, uint64_t> CalshtDW::operator()(const std::array<int, 34>& t,
                                                                          int m,
                                                                          unsigned int mode,
                                                                          bool check_hand = false,
                                                                          bool three_player = false) const
   ```

> [!NOTE]
> Normally, set `m` to the number of tiles divided by 3.

> [!NOTE]
> `mode` specifies which winning patterns to calculate shanten numbers for. Use 1 for General Form, 2 for Seven Pairs, and 4 for Thirteen Orphans. When calculating shanten numbers for multiple winning patterns, specify their bitwise OR.

> [!NOTE]
> This method returns **the shanten number + 1**, the mode, the necessary tiles, and the unnecessary tiles. The mode indicates which winning pattern (General Form, Seven Pairs, or Thirteen Orphans) gives the minimum shanten number. Necessary and unnecessary tiles are each represented as a 64-bit integer. The `n`-th bit indicates whether the `n`-th tile is a necessary tile or an unnecessary tile.

> [!NOTE]
> If you set `check_hand` to `true`, the hand is validated. If you set `three_player` to `true`, the values are calculated for three-player mahjong.

As an example, the following code calculates the necessary and unnecessary tiles for the hand defined above. For this hand, the necessary tiles are the *Pinzu* tiles 1 through 9 and the honor tiles *East* and *West*, while the unnecessary tiles are *East*, *West*, and *White*.

```cpp
#include <array>
#include <bitset>
#include <filesystem>
#include <iostream>
#include <mahjong/calsht_dw.hpp>

int main()
{
   // Set the location of shanten tables
   mahjong::CalshtDW calsht(std::filesystem::current_path());

   std::array<int, 34> hand = {
         1, 1, 1, 0, 0, 0, 0, 0, 0, // manzu
         0, 1, 0, 1, 1, 0, 2, 0, 1, // pinzu
         0, 0, 0, 0, 0, 0, 0, 0, 0, // souzu
         1, 0, 1, 0, 3, 0, 0        // jihai
   };

   const auto [sht, mode, disc, wait] = calsht(hand, 4, 7);

   std::cout << sht << std::endl;
   std::cout << mode << std::endl;
   std::cout << std::bitset<34>(disc) << std::endl;
   std::cout << std::bitset<34>(wait) << std::endl;

   return 0;
}
```
Output:
```
3
1
0010101000000000101011010000000000
0000101000000000111111111000000000
```

## Example

This program simulates single-player mahjong. In each turn, it discards a tile so that the shanten number remains unchanged and the number of necessary tiles after the discard is maximized.

```
$ ./example 1000000 0
Number of Tiles         13
Number of Rounds        1000000
Turn    Shanten Number (-1 - 6) Hora    Tempai  Exp.
0       2       693     23529   195244  439517  284432  55027   1556    0.000002        0.000695        3.154765
1       26      3112    60375   312973  435687  168941  18689   197     0.000026        0.003138        2.763747
2       141     9568    118865  408529  368585  88330   5955    27      0.000141        0.009709        2.424794
3       545     22353   191576  460334  279950  43418   1818    6       0.000545        0.022898        2.134347
4       1506    43322   267422  466972  199617  20579   582     0       0.001506        0.044828        1.883937
5       3387    72600   336739  440685  136592  9811    186     0       0.003387        0.075987        1.664672
6       6648    109206  392442  395760  91240   4632    72      0       0.006648        0.115854        1.469922
7       11567   151710  431745  342462  60138   2346    32      0       0.011567        0.163277        1.295060
8       18472   197480  454823  288434  39572   1207    12      0       0.018472        0.215952        1.136823
9       27662   244033  463545  238203  25920   631     6       0       0.027662        0.271695        0.992603
10      39141   290456  459546  193400  17099   356     2       0       0.039141        0.329597        0.859936
11      52963   334236  446255  155060  11275   209     2       0       0.052963        0.387199        0.738083
12      68788   375125  424899  123453  7621    112     2       0       0.068788        0.443913        0.626338
13      86615   411186  399887  97110   5132    69      1       0       0.086615        0.497801        0.523169
14      106427  442362  371754  75882   3530    45      0       0       0.106427        0.548789        0.427861
15      127854  468652  341791  59239   2440    24      0       0       0.127854        0.596506        0.339831
16      150697  489724  311857  45984   1726    12      0       0       0.150697        0.640421        0.258354
17      174602  505612  282727  35836   1216    7       0       0       0.174602        0.680214        0.183473
```

- The first line shows the number of hand tiles, and the second line shows the number of rounds.
- From the fourth line onward, each line shows, from left to right, the turn number, the ratio of each shanten number (-1 to 6), the winning rate, the tempai rate, and the expected shanten number.

## License

GNU General Public License v3.0.
