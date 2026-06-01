# TDT4258 - 2026 - genAI-1-example README.md

## Introduction

A simple code example demonstrating how cline in vsCode can help with writing Arm assembler and test it from a C-program on a RPi4

We assume you have a folder with the RPi4-1-setup code example for Raspberry Pi 4 in NTNU-course TDT4258 - 2026 edition. That folder contains a simple ARM v7 assembler program (assembler.s) and a C program (main.c). The C program calls asmAdd and checks the result.  

Be sure to open vsCode with this folder as the current folder. Typing "make" in the terminal will compile the program. It can be run by typing "./test" in the terminal, og debugged by pressing F5 in the source code.

## Suggested genAI session

* Make a copy of the entire folder from the RPi4-1-setup example. Check that it works as intended. (Remove the asmroutine (exam-question) part of the code)
* open cline in vsCode
  * enter in the cline chat-window: "extend the program in this folder with an armV7 assembler routine demonstrating subtraction, and test it from the main program", click act and the arrow (send-icon)
  * read output from cline, and check the suggested code change for the assembler file, and the c-file, click save if you find it OK
    * (In my case I got an erroneous comment about using register R2 as scratch register!, and removed it). Also, watch out for other extra comments that might be wrong. AI chatbots are in general too eager to produce a lot of text, often much more than you want. (It can be reduced by adding /compact)
  * save the suggested code
  * let cline do make test and test the code for you
* Press start new task. Write "Make an assembler routine that demonstrates implementation of multiplication by repeated additions, and add 3 test-cases in the C-program"
  * read thru code and save if ok, both assembler and C file
  * test the code

  The result of one such session (using mistralai/Destral-Small-2-24B) will be posted at TDT4258 course web (canvas)

  Questions and comments can be e-mailed to course lecturer Lasse.
