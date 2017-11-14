# parallel_assignment1

Calculate PI value via MPI 

/*
 *                                SPECIFICATION
 *  The programe aims to calculate PI value by implementing leapfrog method.
 *
 *  The processing workflow is
 *  #1 The master generates the first random values for each slave process and send them out
 *  #2 Master generate a sequence of random integers and convert them to (x,y) format, check if
 *     the (x,y) locates in the circle or not.
 *  #3 At the same time slave processes receive the first random value and do the same compution with the
 *     master node. Count the number that how many random could locate in the circle, then send the result back to the master.
 *  #4 The master adds all return values from slaves and sum them up with its own result, then calcute and output PI value.
 */


Any question, contact raoyonghui0630@gmail.com
