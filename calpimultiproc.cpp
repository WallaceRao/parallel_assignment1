
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include "mpi.h"

using namespace std;

// !!! Modify this file path to your own directory when compile. 
char constants_file[] = "/home/s16093645/assignment1_git/parallel_assignment1/jumpconstants.dat";

map<int, long long> map_A;
map<int, long long> map_C;

/*
 *                                SPECIFICATION
 *  The programe aims to calculate PI value by implementing leapfrog method via PBS.
 *
 *  The processing workflow is
 *  #1 The master generates the first random values for each slave process and send them out
 *  #2 Master generate a sequence of random integers and convert them to (x,y) format, check if
 *     the (x,y) locates in the circle or not.
 *  #3 At the same time slave processes receive the first random value and do the same compution with the
 *     master node. Count the number that how many random could locate in the circle, then send the result back to the master.
 *  #4 The master adds all return values from slaves and sum them up with its own result, then calcute and output PI value.
 */


void readConstants()
{
   ifstream infile(constants_file);
   if(!infile)
      cout << "error: infile is null, file name: "<< constants_file << endl;
   string line;
   while (std::getline(infile, line))
   {
      std::istringstream iss(line);
      int k;
      long long a, c, flag;
      if (!(iss >> k >> a >> c >> flag))
      {  
         cout << "read constants file error" << endl;
         break;
      } // error
      map_A[k] = a;
      map_C[k] = c; 
   }    
}

int main(int argc,char* argv[])
{
   // Record the start time of this process
   clock_t start,finish;
   double totaltime;
   start = clock();
   MPI::Init(argc,argv);

   // What is my ID and how many processes are in this pool
   int myid = MPI::COMM_WORLD.Get_rank();
   int numproc = MPI::COMM_WORLD.Get_size();

   // Init a, m, c and seed random
   long a = 1664525;
   long long m = pow(2, 32);
   long c = 1013904223;
   long long n_seed = 12345;
   // n_start is an array saves the first randoms for each processor
   long long* n_start = new long long[numproc];
   *n_start = n_seed;
   // numproc are used, each will generate k randoms.
   long long k = m / numproc;
   // "sqrtm" is used to convert the random to (x,y) value
   long sqrtm = sqrt(m);

   if (myid == 0) // master
   {
      //calculate the first random for each processor
      cout << "There are totally "<< numproc << " processors." << endl;
      readConstants(); // Master read the constants file
      // Pick correct A and C from map_A and map_C
      long ca = map_A[numproc];
      long cc = map_C[numproc];
      for(int i = 1; i < numproc; i ++)
      {
         unsigned long long ni_pre =  *(n_start + i - 1);
         unsigned long long ni = ni_pre;
         ni =  (ca * ni % m + cc % m) % m;
         *(n_start + i) = ni;
         // Master sends the start random to specific processor
         MPI::COMM_WORLD.Send(&ni, 1, MPI_LONG_LONG_INT, i,0);
      }
      // Partial result for node 0
      long long sum = 0; // sum of random intergers that locate in the circle
      // process the first random
      float xi = (*n_start / sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
      float yi = (*n_start % sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
      if((xi*xi + yi*yi) <= 1)
         sum++;
      long long n_pre = *n_start;
      // process the folloing k-1 randoms
      for(long long i = 1; i < k; i++)
      {
         //n(i+1) = (an(i) + c) mod m
         long long n = (a * n_pre + c) % m;
         float xi = (n / sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
         float yi = (n % sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
         if((xi*xi + yi*yi) <= 1)
            sum++;
         n_pre = n;
      }
      //Master waits to receive 'sum' from slaves
      for(int i = 1; i < numproc; i ++)
      {
         long slaveSum;
         MPI::COMM_WORLD.Recv(&slaveSum, 1, MPI::LONG, i, 0);
         sum += slaveSum;
      }
      float PI = 4.0 * sum / m;
      cout << "The final result of PI is "<< PI << endl;
   } else {  // slave
      // Slave waits to receive the start random from master
      unsigned long long n_start;
      MPI::COMM_WORLD.Recv(&n_start, 1, MPI::LONG, 0, 0);
      long slaveSum = 0; // sum of random intergers that locate in the circle
      // process the first random
      float xi = (n_start / sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
      float yi = (n_start % sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
      if((xi*xi + yi*yi) <= 1)
         slaveSum++;
      long long n_pre = n_start;
      // process the folloing randoms
      for(long long i = 1; i < k; i++) {
         unsigned long n = (a * n_pre + c) % m;
         float xi = (n / sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
         float yi = (n % sqrtm -  sqrtm/2.0) / (sqrtm/2.0);
         if((xi*xi + yi*yi) <= 1)
            slaveSum++;
         n_pre = n;
      }
      // Slave sends 'slaveSum' to master
      MPI::COMM_WORLD.Send(&slaveSum, 1, MPI_LONG, 0, 0);
   }
   delete[] n_start;
   MPI::Finalize();
   // output the time consumed.
   if(myid == 0)
   {
      finish=clock();
      totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
      cout<<"the master ran for " <<totaltime << " seconds"<<endl;
   }
}
