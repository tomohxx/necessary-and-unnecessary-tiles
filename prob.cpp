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
  
  const int K = 34;
  const int M = 14;
  const int N = std::atoi(argv[1]);
  
  std::array<int,K> hd;
  std::array<int,K> wl;
  std::array<int,K> cnt;
  std::array<int,4*K> wall;
  int sht[18][8] = {};
  CalshtDW calsht;
  int mode;
  std::int64_t disc;
  std::int64_t wait;
  
  if(!calsht) return 1;
  
  std::mt19937 rand(std::random_device{}());
  std::ofstream fout("result.txt");
  
  for(int i=0; i<4; ++i){
    for(int j=0; j<K; ++j){
      wall[K*i+j] = j;
    }
  }
  
  auto start = std::chrono::system_clock::now();
  
  for(int i=0; i<N; ++i){
    hd.fill(0);
    wl.fill(4);
    
    for(int j=1; j<=M+17; ++j){
      int n = rand()%(4*K-j+1);
      ++hd[wall[n]];
      --wl[wall[n]];
      std::swap(wall[n],wall[4*K-j]);
      
      if(j>=M){
        int num = calsht(hd.data(),4,mode,disc,wait);
        ++sht[j-M][num];
        
        if(num>0){
          std::bitset<K> bs1(disc);
          cnt.fill(-1);
          
          for(int k=0; k<K; ++k){
            if(bs1[k]){
              --hd[k];
              calsht(hd.data(),4,mode,disc,wait);
              std::bitset<K> bs2(wait);
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
            ++sht[k-M][num];
            n = rand()%(4*K-k+1);
            std::swap(wall[n],wall[4*K-k]);
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
      ev += (j-1)*sht[i][j];
      fout << sht[i][j] << '\t';
    }
    fout << 1.0*sht[i][0]/N << '\t' << 1.0*(sht[i][0]+sht[i][1])/N << '\t' << ev/N << '\n';
  }
  
  return 0;
}
