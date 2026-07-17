Step 1: Create cluster.conf containing the following configuration:
0,127.0.0.1,5000
1,127.0.0.1,5001
2,127.0.0.1,5002

Step 2: Create input data file node0.txt with: 100,yogesh\n 101,boby\n 103,sam
Step 3: Create input data file node1.txt with: 103,doraman\n 104,faran\n 105,kedar
Step 4: Create input data file node2.txt with: 106,ram\n 107,krishna\n, 108,xyz

Step 5: Compile the application with: make clean && make

Step 6: Open three terminal windows and execute the three nodes simultaneously:
	Terminal 1: ./dist_store 0 cluster.conf node0.txt
	Terminal 2: ./dist_store 1 cluster.conf node1.txt
	Terminal 3: ./dist_store 2 cluster.conf node2.txt

Step 7: Run as below and see the output

ubuntu@ubuntu:~/practice/dskvdb$ ./dskvdb 0 cluster.conf node0.txt
Initializing Node 0...
Node 0 starting ingestion from: node0.txt
Node 0 load job complete. Broadcasting DONE signal...
Node 0 waiting for synchronization barriers...

=========================================
          NODE 0 RUN STATISTICS
=========================================
  Records Read from input file : 3
  Records Sent (Remote)  : 2
  Records Recv (Remote)  : 2
  Records Stored Locally : 3
  Database Size          : 3
-----------------------------------------
  Local Database Storage Contents:
  108 -> yyy
  105 -> Radha
  102 -> Candy
=========================================

ubuntu@ubuntu:~/practice/dskvdb$

ubuntu@ubuntu:~/practice/dskvdb$ ./dskvdb 1 cluster.conf node1.txt
Initializing Node 1...
Node 1 starting ingestion from: node1.txt
Node 1 load job complete. Broadcasting DONE signal...
Node 1 waiting for synchronization barriers...

=========================================
          NODE 1 RUN STATISTICS
=========================================
  Records Read from input file : 2
  Records Sent (Remote)  : 1
  Records Recv (Remote)  : 2
  Records Stored Locally : 3
  Database Size          : 3
-----------------------------------------
  Local Database Storage Contents:
  106 -> Ram
  103 -> Mily
  100 -> Amol
=========================================

ubuntu@ubuntu:~/practice/dskvdb$


ubuntu@ubuntu:~/dskvdb$ ./dskvdb 2 cluster.conf node2.txt
Initializing Node 2...
Node 2 starting ingestion from: node2.txt
Node 2 load job complete. Broadcasting DONE signal...
Node 2 waiting for synchronization barriers...

=========================================
          NODE 2 RUN STATISTICS
=========================================
  Records Read from Disk : 4
  Records Sent (Remote)  : 3
  Records Recv (Remote)  : 2
  Records Stored Locally : 3
  Final Database Size    : 3
-----------------------------------------
  Local Database Storage Contents:
  107 -> xyz
  104 -> Yogesh
  101 -> Boby
=========================================
