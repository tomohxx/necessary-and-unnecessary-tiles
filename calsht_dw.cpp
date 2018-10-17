#include "calsht_dw.hpp"

void CalshtDW::shift(int& lv, const int rv, Ull& lx, const Ull rx, Ull& ly, const Ull ry) const
{
  if(lv==rv){lx |= rx; ly |= ry;}
  else if(lv>rv){lv = rv; lx = rx; ly = ry;}
}

void CalshtDW::add(Vec& lhs, const Vec& rhs) const
{
  for(int j=9; j>=5; --j){
    int sht = lhs[j]+rhs[0];
    Ull disc_ = (lhs[j+10]<<9)|rhs[10];
    Ull wait_ = (lhs[j+20]<<9)|rhs[20];
    shift(sht,lhs[0]+rhs[j],disc_,(lhs[10]<<9)|rhs[j+10],wait_,(lhs[20]<<9)|rhs[j+20]);

    for(int k=5; k<j; ++k){
      shift(sht,lhs[k]+rhs[j-k],disc_,(lhs[k+10]<<9)|rhs[j-k+10],wait_,(lhs[k+20]<<9)|rhs[j-k+20]);
      shift(sht,lhs[j-k]+rhs[k],disc_,(lhs[j-k+10]<<9)|rhs[k+10],wait_,(lhs[j-k+20]<<9)|rhs[k+20]);
    }
    lhs[j] = sht;
    lhs[j+10] = disc_;
    lhs[j+20] = wait_;
  }
  for(int j=4; j>=0; --j){
    int sht = lhs[j]+rhs[0];
    Ull disc_ = (lhs[j+10]<<9)|rhs[10];
    Ull wait_ = (lhs[j+20]<<9)|rhs[20];

    for(int k=0; k<j; ++k){
      shift(sht,lhs[k]+rhs[j-k],disc_,(lhs[k+10]<<9)|rhs[j-k+10],wait_,(lhs[k+20]<<9)|rhs[j-k+20]);
    }
    lhs[j] = sht;
    lhs[j+10] = disc_;
    lhs[j+20] = wait_;
  }
}
  
void CalshtDW::add(Vec& lhs, const Vec& rhs, const int j) const
{
  int sht = lhs[j]+rhs[0];
  Ull disc_ = (lhs[j+10]<<9)|rhs[10];
  Ull wait_ = (lhs[j+20]<<9)|rhs[20];
  shift(sht,lhs[0]+rhs[j],disc_,(lhs[10]<<9)|rhs[j+10],wait_,(lhs[20]<<9)|rhs[j+20]);

  for(int k=5; k<j; ++k){
    shift(sht,lhs[k]+rhs[j-k],disc_,(lhs[k+10]<<9)|rhs[j-k+10],wait_,(lhs[k+20]<<9)|rhs[j-k+20]);
    shift(sht,lhs[j-k]+rhs[k],disc_,(lhs[j-k+10]<<9)|rhs[k+10],wait_,(lhs[j-k+20]<<9)|rhs[k+20]);
  }
  lhs[j] = sht;
  lhs[j+10] = disc_;
  lhs[j+20] = wait_;
}

CalshtDW::Iter CalshtDW::read_file(Iter first, Iter last, const char* filename) const
{
  std::ifstream fin(filename);
  Vec vec(30);
  
  if(fin.is_open()){
    while(first != last){
      for(int j=0; j<30; ++j) fin >> vec[j];
      *first++ = vec;
    }
  }
  return first;
}

CalshtDW::CalshtDW() : mp1(std::vector<Vec>(1953125,Vec(30))), mp2(std::vector<Vec>(78125,Vec(30))), itr1(mp1.begin()), itr2(mp2.begin())
{
  itr1 = read_file(mp1.begin(),mp1.end(),"index_dw_s.txt");
  itr2 = read_file(mp2.begin(),mp2.end(),"index_dw_h.txt");
}

bool CalshtDW::operator!() const
{
  return mp1.empty() || mp2.empty() || itr1==mp1.begin() || itr2==mp2.begin();
}
  
int CalshtDW::calc_lh(const int* t, const int m, const int f, Ull& disc, Ull& wait) const
{
  Vec ret = mp2[std::accumulate(t+28,t+34,t[27],[](int x, int y){return 5*x+y;})];

  add(ret, mp1[std::accumulate(t+19,t+27,t[18],[](int x, int y){return 5*x+y;})]);
  add(ret, mp1[std::accumulate(t+10,t+18,t[9],[](int x, int y){return 5*x+y;})]);
  add(ret, mp1[std::accumulate(t+1,t+9,t[0],[](int x, int y){return 5*x+y;})], 5+f);
  
  disc = ret[15+f];
  wait = ret[25+f];
  return (static_cast<int>(ret[5+f])+f*3+2-m)/2;
}

int CalshtDW::calc_sp(const int* t, Ull& disc, Ull& wait) const
{
  int pair = 0, kind = 0;
  Ull disc_ = 0ULL;
  Ull wait_ = 0ULL;

  for(int i=0; i<34; ++i){
    if(t[i]==0){
      wait_ |= 1ULL<<i;
    }
    else if(t[i]==1){
      ++kind; disc_ |= 1ULL<<i; wait |= 1ULL<<i;
    }
    else if(t[i]==2){
      ++kind; ++pair;
    }
    else if(t[i]>2){
      ++kind; ++pair; disc |= 1ULL<<i;
    }
  }
  if(kind>7) disc |= disc_;
  else if(kind<7) wait |= wait_;
  
  if(pair==7) wait = 0ULL;
  
  return 7-pair+(kind<7 ? 7-kind:0);
}

int CalshtDW::calc_to(const int* t, Ull& disc, Ull& wait) const
{
  int pair = 0, kind = 0;
  Ull disc_ = 0ULL;
  Ull wait_ = 0ULL;

  for(int i : {0,8,9,17,18,26,27,28,29,30,31,32,33}){
    if(t[i]==0){
      wait |= 1ULL<<i;
    }
    else if(t[i]==1){
      ++kind; wait_ |= 1ULL<<i;
    }
    else if(t[i]==2){
      ++kind; ++pair; disc_ |= 1ULL<<i;
    }
    else if(t[i]>2){
      ++kind; ++pair; disc |= 1ULL<<i;
    }
  }
  for(int i : {1,2,3,4,5,6,7,10,11,12,13,14,15,16,19,20,21,22,23,24,25}){
    if(t[i]>0) disc |= 1ULL<<i;
  }
  if(pair>=2) disc |= disc_;
  else if(pair==0) wait |= wait_;
  
  return 14-kind-(pair>0 ? 1:0);
}
  
int CalshtDW::operator()(const int* t, const int m, const int f, int& mode, Ull& disc, Ull& wait) const
{
  if(f==4){
    Ull disc_ = 0ULL;
    Ull wait_ = 0ULL;
    int sht = calc_lh(t,m,f,disc,wait);
    int sht_ = calc_sp(t,disc_,wait_);
    mode = 1;
    
    if(sht>sht_){
      sht = sht_; mode = 2; disc = disc_; wait = wait_;
    }
    else if(sht==sht_){
      mode |= 2; disc |= disc_; wait |= wait_;
    }
    
    disc_ = 0ULL;
    wait_ = 0ULL;
    sht_ = calc_to(t,disc_,wait_);
    
    if(sht>sht_){
      sht = sht_; mode = 4; disc = disc_; wait = wait_;
    }
    else if(sht==sht_){
      mode |= 4; disc |= disc_; wait |= wait_;
    }
    return sht;
  }
  else{
    mode = 1;
    return calc_lh(t,m,f,disc,wait);
  }
}