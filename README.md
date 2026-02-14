# Necessary and Unnecessary Tiles

A tool for calculating necesaary tiles and unnecessary tiles for winning hands in Japanese mahjong.

[Read this in Japanese (日本語).](README.ja.md)

## What are necessary tiles and unecessary tiles in mahjong?

- Necessary tiles:
  - the tiles needed to win with **least number of exchanges**.
  - decrease the shanten number when one of them is drawn.
  - called "yuukouhai" or "ukeire" in Japanese.
- Unnecessary tiles:
  - the tiles unneeded to win with **least number of exchanges**.
  - keep the shanten number unchanged when one of them is discarded.
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
> **NOTE:** A compiler compatiable with C++20 or higher is needed.

### Compile options

#### `-DENABLE_NYANTEN`

It enables the table search algorithm to use the minimal perfect hash function used in Cryolite's nyanten[^1][^2]. Enabling this option can reduce the size of tables. However, the number of tiles in a hand that can be calculated shanten number is limited to 14 or less.

[^1]: https://github.com/Cryolite/nyanten
[^2]: https://www.slideshare.net/slideshow/a-fast-and-space-efficient-algorithm-for-calculating-deficient-numbers-a-k-a-shanten-numbers-pptx/269706666

#### `-DFIX_RANDOM_SEED`

It fixes the random seed used in the sample program.

#### Building tables

Build tables of parameters required for calculating shanten number. Create files `index_dw_h.bin` and `index_dw_s.bin`.

```
$ ./mkind
```

## Usage

1. Prepare a `std::array<int, 34>` array representing a hand.
   - The `n` th element stores the number of `n` th tiles.

   |         | 1           | 2            | 3           | 4            | 5            | 6            | 7          | 8       | 9       |
   | :------ | :---------- | :----------- | :---------- | :----------- | :----------- | :----------- | :--------- | :------ | :------ |
   | *Manzu* | 0 (1m)      | 1 (2m)       | 2 (3m)      | 3 (4m)       | 4 (5m)       | 5 (6m)       | 6 (7m)     | 7 (8m)  | 8 (9m)  |
   | *Pinzu* | 9 (1p)      | 10 (2p)      | 11 (3p)     | 12 (4p)      | 13 (5p)      | 14 (6p)      | 15 (7p)    | 16 (8p) | 17 (9p) |
   | *Souzu* | 18 (1s)     | 19 (2s)      | 20 (3s)     | 21 (4s)      | 22 (5s)      | 23 (6s)      | 24 (7s)    | 25 (8s) | 26 (9s) |
   | *Jihai* | 27 (*East*) | 28 (*South*) | 29 (*West*) | 30 (*North*) | 31 (*White*) | 32 (*Green*) | 33 (*Red*) |         |         |

   - For example, if you have *manzu* tiles (1, 2, 3), *pinzu* tiles (2, 4, 5, 7, 7, 9), and *jihai* tiles (*East*, *West*, *White*, *White*, *White*), define the following array.

   ```cpp
   std::array<int, 34> hand = {
       1, 1, 1, 0, 0, 0, 0, 0, 0, // Manzu
       0, 1, 0, 1, 1, 0, 2, 0, 1, // Pinzu
       0, 0, 0, 0, 0, 0, 0, 0, 0, // Souzu
       1, 0, 1, 0, 3, 0, 0        // Jihai
   };
   ```

1. Calculate the shanten number, necessary tiles and the unnecessary tiles.
    ```cpp
    std::tuple<int, int, uint64_t, uint64_t> CalshtDW::operator()(const std::array<int, 34>& t,
                                                                  int m,
                                                                  int mode,
                                                                  bool check_hand = false,
                                                                  bool three_player = false) const
    ```

    > **NOTE:** Normally, substitute the value obtained by dividing the number of tiles by 3 into `m`.

    > **NOTE:** `mode` specifies for which winning pattern calculate shanten number. When the pattern is "General Form", `mode` is 1, when "Seven Pairs": 2, "Thirteen Orphans": 4. When calculating the Shanten number for multiple winning patterns, specify the logical sum of them.

    > **NOTE:** This method returns the value of shunten number + 1, mode, necessary tiles, unnecessary tiles. The mode indicates which winning pattern (General Form, Seven Pairs, Thirteen Orphans) has the minimum shunten number. Each valid/unnecessary tile is represented by a 64-bit integer. Whether 1 or 0 of the `i`-th bit indicates whether the `i`-th tile is a necessary tile (or an unnecessary tile) or not.

    > **NOTE:** If you set `check_hand` to `true`, the hand will be validated. If you set `three_player` to `true`, it will calculate the number of shanten in three-player mahjong.

    For example, calculate the necessary tiles and unneccessary tiles of the hand defined above. It requires one of *manzu* tiles (1 to 9) or one of *jihai* tiles (*East*, *West*) for winning, however one of *manzu* tiles (2, 4, 5, 7, 9) or one of  *jihai* tiles (*East*, *West*, *White*) is uneeded. The source code is as follows:

    ```cpp
    #include "calsht_dw.hpp"
    #include <array>
    #include <bitset>
    #include <filesystem>
    #include <iostream>

    int main()
    {
      // Set the location of shanten tables
      CalshtDW calsht(std::filesystem::current_path());

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

## Sample Program

This program simultes single player mahjong. In each round, it discards one of unnecessary tiles, and then maximize the number of necessary tiles.

```
$ ./sample 1000000 0
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

- The first line shows the number of hand tiles, the second line shows the number of games, and the third line shows the execution time (milliseconds).
- The sixth line onward shows the ratio of each shanten number (-1 to 6), winning ratios, *tempai* ratios, expected values of shanten numbers from left to right.

## License

GNU General Public License v3.0.
