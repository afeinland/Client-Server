# Course: CS 100 <Winter 2014>
#
# First Name: Alex
# Last Name: Feinland
# CS Login: afein001
# UCR email: afein001@ucr.edu
# SID: 861056485
#
# Lecture Section: <001>
# Lab Section: <021>
# TA: Bryan Marsh 
#
# Assignment <Lab X>
#
# I hereby certify that the code in this file 
# is ENTIRELY my own original work.

CFLAGS= -ggdb -Wall
CC= g++

all: conServer conClient

conServer: conServer.cpp
	$(CC) $(CFLAGS) conServer.cpp -pthread -o conServer


conClient: conClient.cpp
	$(CC) $(CFLAGS) conClient.cpp -pthread -o conClient

clean:
	rm -f conServer conClient
