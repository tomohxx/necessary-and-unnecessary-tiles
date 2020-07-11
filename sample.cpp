#include <iostream>
#include <fstream>
#include <iomanip>
#include <array>
#include <random>
#include <utility>
#include <algorithm>
#include <chrono>
#include <bitset>
#include <cstdint>
#include "calsht_dw.hpp"

int main(int argc, char* argv[])
{
  if(argc != 2){
    return 1;
  }

#ifdef THREE_PLAYER
  const int T = 27;
#else
  const int T = 34;
#endif
  const int M = 14;
  const int N = std::atoi(argv[1]);
  const int MODE = 7;

  std::vector<int> hd(K, 0);
  std::array<int,K> wl;
  std::array<int,K> cnt;
  std::array<int,4*T> wall;
  int table[18][8] = {};
  CalshtDW calsht;

  std::mt19937_64 rand(std::random_device{}());
  std::ofstream fout("result.txt");

  auto itr = wall.begin();

  for(int i=0; i<K; ++i){
#ifdef THREE_PLAYER
    if(i>0 && i<8) continue;
#endif
    for(int j=0; j<4; ++j){
      *itr++ = i;
    }
  }

  auto start = std::chrono::system_clock::now();

  for(int i=0; i<N; ++i){
    std::fill(hd.begin(), hd.end(), 0);
    std::fill(wl.begin(), wl.end(), 4);

    for(int j=1; j<=M+17; ++j){
      int n = rand()%(4*T-j+1);
      ++hd[wall[n]];
      --wl[wall[n]];
      std::swap(wall[n],wall[4*T-j]);

      if(j>=M){
        auto [sht,mode,disc,wait] = calsht(hd,M/3,MODE);
        ++table[j-M][sht];

        if(sht > 0){
          std::bitset<K> bs1(disc);
          cnt.fill(-1);

          for(int k=0; k<K; ++k){
            if(bs1[k]){
              --hd[k];
              auto [sht_,mode_,disc_,wait_] = calsht(hd,M/3,MODE);
              std::bitset<K> bs2(wait_);
              cnt[k] = 0;

              for(int l=0; l<K; ++l){
                cnt[k] += bs2[l] ? wl[l]:0;
              }
              ++hd[k];
            }
          }
          n = rand()%K;
          int tile = n;
          int max = cnt[n];

          for(int k=n+1; k<n+K; ++k){
            if(cnt[k%K]>max){
              tile = k%K;
              max = cnt[k%K];
            }
          }
          --hd[tile];
        }
        else{
          for(int k=j+1; k<=M+17; ++k){
            ++table[k-M][sht];
            n = rand()%(4*T-k+1);
            std::swap(wall[n],wall[4*T-k]);
          }
          break;
        }
      }
    }
  }

  auto end = std::chrono::system_clock::now();

  fout.setf(std::ios::left,std::ios::adjustfield);
  fout << std::setw(24) << "Number of Tiles" << std::setw(16) << M << '\n';
  fout << std::setw(24) << "Total" << std::setw(16) << N << '\n';
  fout << std::setw(24) << "Time (msec.)" << std::setw(16) << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << '\n';
  fout << std::setw(70) << "\nTurn\tShanten Number (-1 - 6)" << "Hora\tTempai\tExp.\n";

  for(int i=0; i<18; ++i){
    fout << i << '\t';
    double ev = 0;

    for(int j=0; j<8; ++j){
      ev += (j-1)*table[i][j];
      fout << table[i][j] << '\t';
    }
    fout << 1.0*table[i][0]/N << '\t' << 1.0*(table[i][0]+table[i][1])/N << '\t' << ev/N << '\n';
  }

  return 0;
}
