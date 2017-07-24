
//first.cpp Adding numbers using two nodes C++ version
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "mpi.h"

//always use argc and argv, as mpirun will pass the appropriate parms.
int main(int argc,char* argv[])
{
  MPI::Init(argc,argv);

  // What is my ID and how many processes are in this pool?
  int myid = MPI::COMM_WORLD.Get_rank();
  int numproc = MPI::COMM_WORLD.Get_size();
  std::cout << "This is id " << myid << " out of " << numproc << std::endl;

  


  if (myid == 0) { // master

  long a = 1664525;
  long m = 1;
  int pow = 32;
  long c = 1013904223;
  long n_pre = 12345;
  long *n_start = new long[numproc];
  *n_start = n_pre;
  
  //calculate m
  for (int i = 0; i < pow; i ++)
  {
    m = m * 2;
  }
  long k = m / numproc;
  
  //calculate capital A and capital C
  long ca = 1;
  long cc = 0;  
  for(int i = 0; i < k; i ++)
    {
        ca = ca * a % m;
        long caim = 1 * c;  // c * pow(a,i) mod m
        for(int j = 0; j < i; j ++)
        {
            caim = (caim * a) % m;
        }
        cc += caim %m;
    }
  cc %= m;
  

  //calculate the initial "n" for each processor
  for(int i = 1; i < numproc; i ++)
  {
    long ni_pre =  *(n_start + i - 1);
    *(n_start + i) = (ca * ni_pre % m + cc % m) % m;  // x(i+k) = (Ax(i) + C) mod m
    // Master sends 'n_start' to slaves
     MPI::COMM_WORLD.Send(n_start + i, 1, MPI::LONG, i,0);
     MPI::COMM_WORLD.Send(k, 1, MPI::LONG, i,0);
     MPI::COMM_WORLD.Send(a, 1, MPI::LONG, i,0);
     MPI::COMM_WORLD.Send(c, 1, MPI::LONG, i,0);
     MPI::COMM_WORLD.Send(m, 1, MPI::LONG, i,0);
  }
  
    // Partial result for node 0
    int sum0 = 0;
    for(int i = 1; i <= k; i++){
      //n i+1 = (an i + c) mod m
      
      
      sum0 = sum0 + i;
    }

    //Master waits to receive 'sum1' from slave
    int sum1;
    MPI::COMM_WORLD.Recv(&sum1, 1, MPI::INT, 1,0);
    int result = sum0 + sum1;

    std::cout << "The final result is " << result << std::endl;
  }

  else {  // slave

    // Slave waits to receive 'N' from master
    int N;
    MPI::COMM_WORLD.Recv(&N, 1, MPI::INT, 0, 0);
    int sum1 = 0;
    for(int i = N/2 + 1; i <= N; i++){
      sum1 = sum1 + i;
    }

    // Slave sends 'sum1' to master
    MPI::COMM_WORLD.Send(&sum1, 1, MPI_INT, 0, 0);
  }
  MPI::Finalize();
}