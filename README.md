
# Authors
/**
 * @file myshell.c
 * @author Abdullah Saad
 * @date january 20 2021
 * @brief File containing the function operate for shell.
 **/


# Assignment #1: Writing a Shell
Implementing a simple UNIX shell program.
how they work (any assumptions or limitations) and how they were tested


valgrind --leak-check=full --show-leak-kinds=all ./myshell

==31887== 
==31887== HEAP SUMMARY:
==31887==     in use at exit: 0 bytes in 0 blocks
==31887==   total heap usage: 29 allocs, 29 frees, 44,987 bytes allocated


# Description
    A unix Shell allows to run other programs.



#   Getting Started





# Set 1 Function
NO LIMITATION for set fucntion 1 

1- Exit the program 

asaad02@percy :/home/undergrad/0/asaad02/Desktop/Shell-Cis-3110/1 > exit



myShell terminating.....

[Process completed]


2- Accept with no argummnets such as pwd ls 

Fri Feb  5 20:11:24 2021
asaad02@percy :/home/undergrad/0/asaad02/Desktop/Shell-Cis-3110 > ls
1  cis3110_profile  makefile  myshell  myshell.c  myshell.h  myshell.o  README.md



3- Accept with arguments such as ls -l , ./hello 2 , Also can run any program or make file

Fri Feb  5 20:11:31 2021
asaad02@percy :/home/undergrad/0/asaad02/Desktop/Shell-Cis-3110 > ls -l
total 76
drwxr-xr-x 2 asaad02 undergrad     8 Feb  5 20:10 1
-rw-r--r-- 1 asaad02 undergrad    62 Feb  5 18:53 cis3110_profile

4 - executeds any file  in the background using 

asaad02@percy :/home/undergrad/0/asaad02/Desktop/Shell-Cis-3110/1 > ps
  PID TTY          TIME CMD
 4572 pts/16   00:00:00 myshell
 9973 pts/16   00:00:00 bgSleep
16707 pts/16   00:00:00 bgSleep
21950 pts/16   00:00:00 bgSleep
28527 pts/16   00:00:00 bash
32244 pts/16   00:00:00 ps

[4]+  Done         ./bgSleep



# set funtion 2

1 : Redirecting stdout
asaad02@percy :/home/undergrad/0/asaad02/Desktop/Shell-Cis-3110/1 > ls -l > fileList.txt

Fri Feb  5 20:13:07 2021
asaad02@percy :/home/undergrad/0/asaad02/Desktop/Shell-Cis-3110/1 > more fileList.txt
total 31
-rwxr-xr-x 1 asaad02 undergrad 16712 Feb  5 16:43 bgSleep
-rw-r--r-- 1 asaad02 undergrad   684 Feb  5 16:00 bgSleep.c



2: simple pipe

asaad02@percy :/home/undergrad/0/asaad02/Desktop/Shell-Cis-3110/1 > ls -l | wc
      7      56     380

3: redirecting stdin:
such as
 more inputFile.txt
 ./interactive 2
 ./interactive 2 < inputFile.txt
 ./interactive 2 < inputFile.txt > outputFile
 more outputFile | sort
 ./interactive 2 < inputFile.txt | sort


 # set function 3
 Fri Feb  5 20:14:17 2021
asaad02@percy :/home/undergrad/0/asaad02/Desktop/Shell-Cis-3110/1 > echo $PATH
$PATH
/usr/bin:/bin:$HOME 

Fri Feb  5 20:14:25 2021
asaad02@percy :/home/undergrad/0/asaad02/Desktop/Shell-Cis-3110/1 > echo $HOME

$HOME
/home/undergrad/0/asaad02 

2 - And Accepting History command
such as 
*history prints out all of the input to the she
*history -c which clears the history file 
*history n which outputs only the last n command lines

The cd or “change directory” changes the notio and print the path of the directory.