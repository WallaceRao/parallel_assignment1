
//random.cpp Calculate PI
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "mpi.h"
using namespace std;
//always use argc and argv, as mpirun will pass the appropriate parms.
int main(int argc,char* argv[])
{
  MPI::Init(argc,argv);

  // What is my ID and how many processes are in this pool?
  int myid = MPI::COMM_WORLD.Get_rank();
  int numproc = MPI::COMM_WORLD.Get_size();
  std::cout << "This is id " << myid << " out of " << numproc << std::endl;
  
  long a = 1664525;
  long m = 1;

  long c = 1013904223;
  long n_seed = 1234;
  long *n_start = new long[numproc];
   *n_start = n_seed;
  
  //calculate m
  m = pow(2, 32);
  long k = m / numproc;
  cout << " 1" << endl;
   //calculate capital A and capital C
  long ca128 = 2305417729;
  long cc128 = 2065617536;  
  
  
  if (myid == 0) { // master
    //calculate the initial "n" for each processor
    cout << "2" << endl;
    for(int i = 1; i < numproc; i ++)
    {
	long times = k;
        long ni_pre =  *(n_start + i - 1);
	long ni = ni_pre;
	while(times > 0)
	{
	  ni =  (ca128 * ni % m + cc128 % m) % m;
	  times -= 128;
	}
        *(n_start + i) = ni;  // x(i+k) = (Ax(i) + C) mod m
        // Master sends 'n_start', k, a, c, m to slaves
        MPI::COMM_WORLD.Send(&ni, 1, MPI::LONG, i,0);
	cout << "ni is" << ni << " i is" << i << endl;

    }
  
    // Partial result for node 0
    long sum = 0; // sum of random intergers that locate in the circle
    // process the first random
    long sqrtm = sqrt(m);
    float xi = (*n_start / sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
    float yi = (*n_start % sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
    if((xi*xi + yi*yi) <= 1)
        sum++;
    long n_pre = *n_start;
    // process the folloing randoms
    for(int i = 1; i <= k; i++){
      //n(i+1) = (an(i) + c) mod m
      long n = (a * n_pre + c) % m;
      float xi = (n / sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
      float yi = (n % sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
      if((xi*xi + yi*yi) <= 1)
        sum++;
      n_pre = n;
    }

    //Master waits to receive 'sum' from slaves
    
    for(int i = 1; i < numproc; i ++)
    {
        int slaveSum;
        MPI::COMM_WORLD.Recv(&slaveSum, 1, MPI::LONG, i, 0);
        sum += slaveSum;
    }
        
    float PI = 4.0 * sum / m;

    std::cout << "The final result is " << PI << std::endl;
  }

  else {  // slave
    // Slave waits to receive 'N' from master
    long n_start;
    MPI::COMM_WORLD.Recv(&n_start, 1, MPI::LONG, 0, 0);
    long slaveSum = 0; // sum of random intergers that locate in the circle
    // process the first random
    long sqrtm = sqrt(m);
    float xi = (n_start / sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
    float yi = (n_start % sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
    if((xi*xi + yi*yi) <= 1)
        slaveSum++;
    long n_pre = n_start;
    // process the folloing randoms
    for(int i = 1; i <= k; i++){
      //n(i+1) = (an(i) + c) mod m
      long n = (a * n_pre + c) % m;
      float xi = (n / sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
      float yi = (n % sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
      if((xi*xi + yi*yi) <= 1)
        slaveSum++;
      n_pre = n;
    }
    // Slave sends 'slaveSum' to master
    MPI::COMM_WORLD.Send(&slaveSum, 1, MPI_LONG, 0, 0);
  }
  MPI::Finalize();
}
