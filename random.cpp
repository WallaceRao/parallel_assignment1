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

  long a = 1664525;
  long m = 1;
  int pow = 32;
  long c = 1013904223;
  long n_pre = 12345;
  for (int i = 0; i < pow; i ++)
  {
    m = m * 2;
  }

  //calculate C
  long *cc = new long[numproc];
  for(int i = 0; i < numproc; i ++)
  {
    long k = m/numproc * i;


  }


  long *p = new long[numproc];
  //calculate A
  for(int i = 0; i < numproc; i ++)
  {
    long k = m/numproc * i;
    long ca = 1;
    for(int i = 0; i < k; i ++)
    {
        ca = ca * a %m;
    }
    ca = ca % m;
  }
  p[0] = n_pre;
  p[1] =






  if (myid == 0) {

    // Get the number the user wants
    int N = atoi(argv[1]);

    // Master sends 'N' to slave
    MPI::COMM_WORLD.Send(&N, 1, MPI::INT, 1,0);

    // Partial result for node 0
    int sum0 = 0;
    for(int i = 1; i <= N/2; i++){
      sum0 = sum0 + i;
    }

    //Master waits to receive 'sum1' from slave
    int sum1;
    MPI::COMM_WORLD.Recv(&sum1, 1, MPI::INT, 1,0);
    int result = sum0 + sum1;

    std::cout << "The final result is " << result << std::endl;
  }

  else if (myid == 1) {

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
