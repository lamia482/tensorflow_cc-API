#include <stdio.h>
#include <unistd.h>

class Markt
{
public:
  unsigned long t1, t2;
  
  Markt(unsigned long long _freq)
  {
    freq = _freq;
    t1 = t2 = 0;
  }
  
  void mark1(void){t1 = (unsigned long)GetCycleCount();}
  void mark2(void){t2 = (unsigned long)GetCycleCount();}
  float sectime(void){return ((t2-t1)*1./freq);}
  float miltime(void){return 1000.*sectime();}
  float mictime(void){return 1000.*miltime();}
  
private:
  unsigned long long freq;

#if defined (__i386__)
static __inline__ unsigned long long GetCycleCount(void)
{
  unsigned long long int x;
  __asm__ volatile("rdtsc":"=A"(x));
  return x;
}
#elif defined (__x86_64__)
static __inline__ unsigned long long GetCycleCount(void)
{
  unsigned hi,lo;
  __asm__ volatile("rdtsc":"=a"(lo),"=d"(hi));
  return ((unsigned long long)lo)|(((unsigned long long)hi)<<32);
}
#endif

};
