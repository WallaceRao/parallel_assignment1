
//random.cpp Calculate PI
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>


//always use argc and argv, as mpirun will pass the appropriate parms.
int main(int argc,char* argv[])
{


  long a = 1664525;
  long m = 1;

  long c = 1013904223;
  long n_seed = 12345;
  
  //calculate m
  for (int i = 0; i < 32; i ++)
  {
    m = m * 2;
  }
  
   //calculate capital A and capital C
 
  
  cout << "1" << endl;
  long sum = 0; // sum of random intergers that locate in the circle
    // process the first random
   long sqrtm = sqrt(m);
    float xi = (n_seed / sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
    float yi = (n_seed % sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
    if((xi*xi + yi*yi) <= 1)
        sum++;
    long n_pre = n_seed;
    // process the folloing randoms
    for(long i = 1; i < m; i++){
        if(i == m/4)
            cout << "i is 1/4 m,  " << i << endl;
        if(i == m/2)
            cout << "i is 1/2 m,  " << i << endl;
        if(i == m / 4 * 3)
            cout << "i is 3/4 m,  " << i << endl;
        if(i == m/8)
            cout << "i is 1/8 m,  " << i << endl;
      //n(i+1) = (an(i) + c) mod m
      long n = (a * n_pre + c) % m;
      float xi = (n / sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
      float yi = (n % sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
      if((xi*xi + yi*yi) <= 1)
        sum++;
      n_pre = n;
    }

    
    float PI = 4.0 * sum / m;

    std::cout << "The final result is " << PI << std::endl;
}