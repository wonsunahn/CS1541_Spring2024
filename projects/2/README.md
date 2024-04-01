- [CS 1541 - Introduction to Computer Architecture](#cs-1541---introduction-to-computer-architecture)
- [Introduction](#introduction)
  * [Description](#description)
  * [Processor Design](#processor-design)
- [Building and Running](#building-and-running)
  * [Environment Setup](#environment-setup)
  * [Directory Structure and Makefile Script](#directory-structure-and-makefile-script)
  * [Program Output](#program-output)
    + [Solution Binary](#solution-binary)
    + [Current Binary](#current-binary)
  * [Configuration Files](#configuration-files)
  * [Trace Files](#trace-files)
- [Your Tasks](#your-tasks)
  * [Task 1: Implement the Cache Block Array](#task-1-implement-the-cache-block-array)
  * [Task 2: Implement the Write-back and Write-through Caches](#task-2-implement-the-write-back-and-write-through-caches)
    + [Source Code](#source-code)
  * [Task 3: Analyze Performance Data for Arrays and Linked-lists](#task-3-analyze-performance-data-for-arrays-and-linked-lists)
    + [Generating Performance Data on your Laptop](#generating-performance-data-on-your-laptop)
  * [Submission](#submission)
- [Resources](#resources)

# CS 1541 - Introduction to Computer Architecture
Spring Semester 2024 - Project 2

Please accept Project 2 on **GitHub Classroom** using the following link: https://classroom.github.com/a/XjSFGfHp

* DUE: Apr 26 (Friday), 2024 11:59 PM

# Introduction

## Description

The goal of this project is to create a software simulator for an in-order
processor with a configurable memory hierarchy.  I've already explained why
building simulators is important in Project 1, so I will not belabor that
point. :)

## Processor Design

The processor that you will simulate is configured with a memory hierarchy that
looks as follows:

<img alt="Memory hierarchy" src=Project2_diagram.png width=700>

We will only test a 1-wide processor because we are focusing on memory stalls
this time.  You will also ignore all pipeline hazards for the same reason.

The processor has split data and instruction L1 caches and a unified L2 cache
below it.  Below that sits the DRAM memory.  In the test configurations, the L2
cache is always configured as a write-back (WB) cache to reduce bandwidth
consumption to memory.  The L1 cache is configured as either a write-through
(WT) cache or a write-back (WB) cache.  Write-back and write-through caches
work in exactly the same way as explained during the lecture.  Please review
the lecture if you don't remember.

The processor pipeline has a write-buffer to maintain pending stores.  The
write-buffer allows store instructions which may potentially suffer a long
delay to commit immediately as long as there is space in the write-buffer.
While the store is pending, load instructions that hit on the pending store can
return immediately as well.  The write-buffer is assumed to be fully
associative (as is true in most processors).

Each cache also has a dedicated write-buffer for write-back memory requests.
We are going to assume for simplicity that all cache write-buffers are
infinitely sized.  That means that the processor will never suffer stalls due
to write-backs of dirty blocks.

Also, each cache is a non-blocking cache, meaning that while the cache is
waiting on a cache miss, the cache can continue servicing other memory
requests, be they cache hits or cache misses.  This is done through the Miss
Status Handling Register (MSHR) table that we discussed briefly in class.
Each entry in the table has an ID for each missing memory request and the
required action when the request returns from the lower memory hierarchy.
This allows the cache to handle multiple outstanding misses that may return
out-of-order from the lower memory hierarchy.  We are also going to assume
that the MSHR table is infitely sized so that we can have an infinite number
of outstanding misses and never have to block.

# Building and Running

## Environment Setup

Please refer to [Project 1 Environment
Setup](https://github.com/wonsunahn/CS1541_Project1/#creating-build-environment-on-local-machine)
on how to set up your local VSCode developement environment.  If you already
did the setup, you don't need to do anything in addition.  If you have trouble
with the setup, please feel free to use GitHub codespaces that fires up a
Docker container VSCode development environment pre-configured with the
necessary packages.

## Directory Structure and Makefile Script

Here is an overview of the directory structure:

```
# Source code you will be **modifying**.
CacheCore.cpp / CacheCore.h : A cache block array organized into rows and columns.
Cache.h / Cache.cpp : Contains classes for a write-back cache and a write-through cache.

# Source code newly added as part of Project 2.
CacheLine.h : A cache line (a.k.a. a cache block) with tag, valid bit, dirty bit, and age.
Counter.h : A counter for statistics reporting.
DRAM.h : DRAM memory, which mostly acts like a cache that always hits.
MemObj.cpp / MemObj.h : Parent class for all memory objects (caches and DRAM).
MemRequest.cpp / MemRequest.h : Memory request that gets passed around memory objects.
WriteBuffer.h : Write buffer attached to the processor pipeline
log2i.cpp / log2i.h : Contains the log2i function, a log2 for integers.

# Directory newly added as part of Project 2.
doc/ : Directory where Doxygen documentation of the source code is kept.

# Source code inherited from Project 1 with small modifications.
five_stage_solution(.linux/.exe/.mac) : **Reference solution** binaries for each OS.
Makefile : The build script for the Make tool.
config.c / config.h : Functions used to parse and read in the processor configuration file.
CPU.c / CPU.h : Implements the five stages of the processor pipeline, modified to consider memory stalls.
five_stage.c : Main function. Parses commandline arguments and invokes the five stages at every clock cycle.
trace.c / trace.h : Functions to read and write the trace file.
trace_generator.c : Utility program to generate a trace file of your own.
trace_reader.c : Utility program to read and print out the contents of a trace file in human readable format.
confs/ : Directory where processor configuration files are.
diffs/ : Directory with diffs between outputs/ and outputs_solution/ are stored.
outputs/ : Directory where outputs after running five_stage are stored.
outputs_solution/ : Directory where outputs produced by five_stage_solution are stored.
plot_confs/ : Directory where processor configurations for the plot generation are.
plot_outputs/ : Directory where outputs after running five_stage are stored for plot generation.
plots_outputs_solution/ : Directory where outputs after running five_stage_solution are stored for plot generation.
traces/ : Directory where instruction trace files used to test the simulator are stored.
```

In order to build the project and run the simulations, you only need to do 'make' to invoke the 'Makefile' script:

```
make
```

The output should being with:

```
g++ -c -g -Wall -Wno-format-security -std=c++11 -I/usr/include/glib-2.0/ -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ five_stage.c
g++ -c -g -Wall -Wno-format-security -std=c++11 -I/usr/include/glib-2.0/ -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ config.c
g++ -c -g -Wall -Wno-format-security -std=c++11 -I/usr/include/glib-2.0/ -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ CPU.c
g++ -c -g -Wall -Wno-format-security -std=c++11 -I/usr/include/glib-2.0/ -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ trace.c
...
```

As in Project 1, the results of the simulations are stored in the outputs/
directory and also side-by-side diffs with the outputs_solution/ directory are
generated and stored in the diffs/ directory.  When you debug the program, you
will find these side-by-side diffs useful.

You can generate your own traces using the trace_generator and put it inside
the traces/ directory or create a new configuration inside the confs/
directory, and they will be incorporated into the results automatically by the
Makefile script.  

The uses of the 'make build', 'make clean', and 'make distclean' commands are
identical to Project 1.

## Program Output

As before five_stage.c reads in a trace file (a binary file containing a
sequence of executed instructions) and simulates the processor described above.
But since the cache implementation is as of now incomplete, the L1 and L2
caches will always miss and you will always go to DRAM memory (unless you hit
in the write-buffer, of course).  Your goal is to complete the cache
implementation so that the accesses hit in the caches when appropriate.

### Solution Binary

Let's start by looking at five_stage_solution (the solution binary) to see how
the output *should* look like.

Try doing the following, choosing the file extension that is appropriate for your OS:

```
./five_stage_solution(.linux/.exe/.mac) -t traces/two_stores.tr -c confs/l1-wb.conf -d
```

Or, alternatively open the 'outputs_solution/two_stores.l1-wb.out' file after doing 'make'.

And you should see the following output at the beginning:

```
Memory system setup successful.

Printing all memory objects ... 

[DL1Cache]
device type = cache
write policy = WB
hit time = 2
capacity = 8192
block size = 64
associativity = 1
lower level = L2Cache

[IL1Cache]
device type = cache
write policy = WB
hit time = 2
capacity = 8192
block size = 64
associativity = 1
lower level = L2Cache

[L2Cache]
device type = cache
write policy = WB
hit time = 10
capacity = 16384
block size = 64
associativity = 4
lower level = Memory

[Memory]
device type = dram
hit time = 100

[WriteBuffer]
device type = dram
hit time = 1

======================================================================
...
```

This is just saying that the configuration file has been successfully parsed
and the memory hierarchy objects have been created and initialized correctly
with the configuration parameters.  You can see that all caches are configured
to be 'WB' or write-back.

Next, you see the following output:
```
======================================================================
[IF CYCLE: 1] STORE: (Seq: 1)(Addr: 0x00009900)(PC: 0x00000000)
IL1Cache->access(MemRead, addr: 0x00000000, latency: 2)
L2Cache->access(MemRead, addr: 0x00000000, latency: 12)
Memory->access(MemRead, addr: 0x00000000, latency: 112)

Printing all cache contents ...
[DL1Cache]
[IL1Cache]
(0, 0) tag=0x0:valid=1:dirty=0:age=0
[L2Cache]
(0, 0) tag=0x0:valid=1:dirty=0:age=0
[WriteBuffer]

Fast forwarding cycle: 1 -> 112
======================================================================
```

This shows your first memory access.  It is saying that the instruction fetch
(PC: 0) for the store instruction generated a read memory request (addr: 0) to
the instruction L1 cache.  Since initially the cache is empty, it will go down
all the way to memory to fetch that cache block.  You can see that at each
level of the memory hierarchy, latency increases equal to the configured hit
time parameter for that memory object.  When the memory request returns, the
current cycle is fast forwarded by the number of cycles that the CPU stalls
waiting for the memory request (1 -> 112).  Since the CPU is going to be
stalling and doing nothing for the duration of the request, it makes sense to
fast forward to the point where the request returns.  The number of stall
cycles is calculated as 'latency - 1' (112 - 1).  One is subtracted because one
cycle is the latency for a pipeline stage and anything beyond that is what
causes a stall.

Next, the cache contents of all the caches are dumped to output.  Only valid
blocks are dumped.  The block string representation is interpreted as follows:

```
(0, 0) tag=0x0:valid=1:dirty=0:age=0
```

This means that this cache block is in row 0, column 0 of the cache block
array.  Row is the set index and column is the index of the cache block within
the set.  Please use the cache visualizer under resources/cache_demo/ of the
course repository to visualize the rows and columns.  The rest is the various
metadata for that cache block.

Next, you see the following output:
```
======================================================================
[IF CYCLE: 113] STORE: (Seq: 2)(Addr: 0x00009920)(PC: 0x00000004)
IL1Cache->access(MemRead, addr: 0x00000004, latency: 2)

Printing all cache contents ...
[DL1Cache]
[IL1Cache]
(0, 0) tag=0x0:valid=1:dirty=0:age=0
[L2Cache]
(0, 0) tag=0x0:valid=1:dirty=0:age=0
[WriteBuffer]

Fast forwarding cycle: 113 -> 114
======================================================================
```

You see an instruction fetch for the next PC: 4.  Due to spatial locality, this
read to the instruction L1 cache hits.  You can see that this time, the memory
request just accesses the instruction L1 cache and immediately returns.  The
cache contents remain the same since no new blocks are allocated.  The current
cycle is fast forwarded by one cycle since the L1 cache access latency was 2.

Next, you see the following output:
```
======================================================================
[MEM CYCLE: 116] STORE: (Seq: 1)(Addr: 0x00009900)(PC: 0x00000000)
WriteBuffer->access(MemWrite, addr: 0x00009900, latency: 1)
DL1Cache->access(MemWrite, addr: 0x00009900, latency: 1)
L2Cache->access(MemRead, addr: 0x00009900, latency: 1)
Memory->access(MemRead, addr: 0x00009900, latency: 1)

Printing all cache contents ...
[DL1Cache]
(100, 0) tag=0x4:valid=1:dirty=1:age=0
[IL1Cache]
(0, 0) tag=0x0:valid=1:dirty=0:age=0
[L2Cache]
(0, 0) tag=0x0:valid=1:dirty=0:age=0
(36, 0) tag=0x9:valid=1:dirty=0:age=0
[WriteBuffer]
tag=0x9900:pendingUntil:228
======================================================================
```

You see the first STORE instruction performing the write access at the MEM
stage with addr: 0x00009900.  It first gets registered as a pending store in
the write-buffer and then the MemWrite request is propagated down the memory
hierarchy.  It misses in the data L1 cache, which means that a new block must
be allocated in the L1 and the data for the block read in from lower memory.
So the MemWrite request is now mutated into a MemRead request to access the L2
cache.  It misses there again and you have to go all the way DRAM memory.  

Now note that the latency of the memory request always shows as 1 even though
it has to go all the way to memory.  This is because the store is immediately
committed as pending in the write-buffer without any stalling, and the write
request performs behind the scenes.  The write-buffer entry will be released at
cycle 228 as indicated in the entry:

```
tag=0x9900:pendingUntil:228
```

That is also why there is no fast forwarding that needs to happen here.

Looking at the cache contents, we see the new cache block read into the L2 cache:

```
(36, 0) tag=0x9:valid=1:dirty=0:age=0
```

It's clean because it hasn't been modified.  We also see the new cache block in the DL1 cache:

```
(100, 0) tag=0x4:valid=1:dirty=1:age=0
```

This block is dirty because it was written to with the store data.

Next, you see the following output:

```
======================================================================
[MEM CYCLE: 117] STORE: (Seq: 2)(Addr: 0x00009920)(PC: 0x00000004)
WriteBuffer->access(MemWrite, addr: 0x00009920, latency: 1)
DL1Cache->access(MemWrite, addr: 0x00009920, latency: 1)

Printing all cache contents ...
[DL1Cache]
(100, 0) tag=0x4:valid=1:dirty=1:age=0
[IL1Cache]
(0, 0) tag=0x0:valid=1:dirty=0:age=0
[L2Cache]
(0, 0) tag=0x0:valid=1:dirty=0:age=0
(36, 0) tag=0x9:valid=1:dirty=0:age=0
[WriteBuffer]
tag=0x9900:pendingUntil:228
tag=0x9920:pendingUntil:119
======================================================================
```

This is the MEM stage for the second store to addr: 0x00009920.  The previous
address 0x00009900 and 0x00009920 are on the same cache block so the data L1
cache hit.  Again, the store is registered as pending in the write-buffer so no
delay is incurred.  Note that for new pending store, the store is only pending
until cycle 119 (2 cycles later) as the store hits in the L1 cache.

Next, you see a summary of statistics for each memory object:
```
Printing all memory stats ... 

DL1Cache:readHits=0:readMisses=0:writeHits=1:writeMisses=1:writeBacks=0
IL1Cache:readHits=1:readMisses=1:writeHits=0:writeMisses=0:writeBacks=0
L2Cache:readHits=0:readMisses=2:writeHits=0:writeMisses=0:writeBacks=0
Memory:readHits=2:writeHits=0
WriteBuffer:readHits=0:readMisses=0:writeHits=0:writeMisses=2:writeOverflows=0
+ Memory stall cycles : 112
+ Number of cycles : 118
+ IPC (Instructions Per Cycle) : 0.0169
```

There are a few invariants here:

* L2Cache hits + L2Cache misses = DL1Cache misses + IL1Cache misses + DL1Cache writeBacks + IL1Cache writeBacks
* Memory hits = L2Cache misses

Make sure they hold in your code too.

And then some final statistics:
```
+ Memory stall cycles : 112
+ Number of cycles : 118
+ IPC (Instructions Per Cycle) : 0.0169
```

### Current Binary

As I said, the implementation is incomplete, so the L1 and L2 caches will
always miss and you will always go to DRAM memory.  Let's look at five_stage
(the current binary) to see how the output looks like *now*.

Try doing the following:

```
./five_stage(.linux/.exe/.mac) -t traces/two_stores.tr -c confs/l1-wb.conf -d
```

Or, alternatively open the 'outputs/two_stores.l1-wb.out' file after doing 'make'.

And you should see the following output after the preamble in the beginning:

```
======================================================================
[IF CYCLE: 1] STORE: (Seq: 1)(Addr: 0x00009900)(PC: 0x00000000)
IL1Cache->access(MemRead, addr: 0x00000000, latency: 2)
L2Cache->access(MemRead, addr: 0x00000000, latency: 12)
Memory->access(MemRead, addr: 0x00000000, latency: 112)

Printing all cache contents ...
[DL1Cache]
[IL1Cache]
[L2Cache]
[WriteBuffer]

Fast forwarding cycle: 1 -> 112
======================================================================
======================================================================
[IF CYCLE: 113] STORE: (Seq: 2)(Addr: 0x00009920)(PC: 0x00000004)
IL1Cache->access(MemRead, addr: 0x00000004, latency: 2)
L2Cache->access(MemRead, addr: 0x00000004, latency: 12)
Memory->access(MemRead, addr: 0x00000004, latency: 112)

Printing all cache contents ...
[DL1Cache]
[IL1Cache]
[L2Cache]
[WriteBuffer]

Fast forwarding cycle: 113 -> 224
======================================================================
```

You can see that on the instruction fetch for the first store, the read misses
at all levels of the cache.  But afterwards, no blocks are added to any of the
caches.  This causes the second instruction fetch to also miss, even though it
is to the same cache block.  Your job is to make this work.

## Configuration Files

You can find 2 configuration files under the confs/ directory.  Each will
configure a different processor when passed as the -c option to five_stage.
The files are: l1-wb.conf and l1-wt.conf.

* l1-wb.conf.conf : Configuration where L1 caches are write-back (WB) caches.
* l1-wt.conf.conf : Configuration where L1 caches are write-through (WT) caches.

Other than the difference in the L1 write policy, all other parameters are equal.

Here is how the l1-wb.conf file looks like:

```
# Processor pipeline
[pipeline]
width         = 1
instSource    = IL1Cache
dataSource    = WriteBuffer

# Instruction L1 cache
[IL1Cache]
deviceType    = cache
size          = 8192            # 8 * 1024
assoc         = 1
bsize         = 64
writePolicy   = WB
replPolicy    = LRU
hitDelay      = 2 
lowerLevel    = L2Cache

# Write buffer
[WriteBuffer]
deviceType    = writebuffer
size          = 3               # 3 entries
hitDelay      = 1 
lowerLevel    = DL1Cache

# Data L1 cache
[DL1Cache]
deviceType    = cache
size          = 8192            # 8 * 1024
assoc         = 1
bsize         = 64
writePolicy   = WB
replPolicy    = LRU
hitDelay      = 2
lowerLevel    = L2Cache

# L2 cache
[L2Cache]
deviceType    = cache
size          = 16384           # 16 * 1024
assoc         = 4
bsize         = 64
writePolicy   = WB
replPolicy    = LRU
hitDelay      = 10
lowerLevel    = Memory

# DRAM memory
[Memory]
deviceType    = dram
size          = 64
assoc         = 1
bsize         = 64
writePolicy   = WB
replPolicy    = LRU
hitDelay      = 100
lowerLevel    = null
```

Here is what some of those items mean:

[pipeline]
* width = 1 : It is a 1-wide processor.
* instSource = IL1Cache : The pipeline uses IL1Cache as its data source.
* dataSource = WriteBuffer : The pipeline uses WriteBuffer as its data source.

[WriteBuffer]
* deviceType    = writebuffer : The device type is a write-buffer.
* size          = 3 : Capacity is 3 entries.
* hitDelay      = 1 : Delay required to access the write-buffer is 1 cycle. 
* lowerLevel    = DL1Cache : The memory object below this level is DL1Cache.

[IL1Cache]
* deviceType = cache : The device type is a cache.
* size = 8192 : Capacity is 8 KB.
* assoc = 1 : Associativity is 1-way (a.k.a. direct-mapped).
* bsize = 64 : Cache block size is 64 bytes.
* writePolicy = WB : Write policy is write-back (not write-through).
* replPolicy = LRU : Replacement policy is LRU (this is the only option).
* hitDelay = 2 : Delay required to access the cache is 2 cycles.
* lowerLevel = L2Cache : The memory object below this level is L2Cache.

The instSource, dataSource, and lowerLevel parameters name a memory object by
the section name that defines that object.  In this way, the memory hierarchy
can be configured with an arbitrary number of levels.

## Trace Files

You can find 8 trace files under the traces/ directory.  I've listed them in the orer of difficulty.

* one_load.tr : One load instruction.
* one_store.tr : One store instruction.
* two_loads.tr : Two load instructions.  The second load hits due to spatial locality.
* two_stores.tr : Two store instructions.  The second store hits due to spatial locality.
* many_loads.tr : Many load instructions.  The many loads all land on the same set and cause cache block replacements at multiple levels of the cache.  LRU replacement policy is tested as well.
* many_stores.tr : Many store instructions.  The many stores all land on the same set and cause cache block replacements at multiple levels of the cache.  LRU replacement policy is tested as well.  The additional difficulty here is that since the blocks are dirty, write-backs will need to occur for write-back caches.
* many_loads_then_stores.tr : All the loads in many_loads.tr followed by all the stores in many_stores.tr. 
* many_stores_then_loads.tr : The opposite of many_loads_then_stores.tr.
* sample.tr : A moderately long trace of instructions (681 instructions).

# Your Tasks

All the places where you have to complete code is clearly marked with '// TODO'
comments.  The only files that you would have to modify are CacheCore.cpp /
CacheCore.h and Cache.cpp / Cache.h.  While completing the methods with '//
TODO' comments, feel free to add more member variables and member functions to
the above files as required.

## Task 1: Implement the Cache Block Array

Complete the implementation of CacheCore.cpp / CacheCore.h.  The 'content'
cache block array has been already allocated for you.  It is up to you to
interpret that array to the row and column organization shown in the cache
visualizer for a set-associative cache.  Start by implementing the index2Row
and index2Column functions as specified according to the configured
associativity.  Then go on to implement the accessLine and allocateLine
functions that searches for a block and allocates block in the cache
respectively.

## Task 2: Implement the Write-back and Write-through Caches

Complete the implementation of Cache.h / Cache.cpp.  As of now, all accesses
miss in the cache and no cache block allocation is done.  Complete the read,
write, and writeBack functions according to what we learned in the lecture.

### Source Code

I had to write some of the code in C++ this time because there was no way to
cleanly implement memory objects with just C.  While you may have never learned
C++, don't get intimidated, it's just C with classes thrown in.  The syntax for
C++ classes is almost identical to Java classes with small variations.  

Since you are using C++ for the first time, I made sure to rigorously document
all the variables and functions so everything is clear.  I also used
[Doxygen](https://www.doxygen.nl/index.html) to auto-generate HTML
documentation from the source code comments.  The HTML files are under
doc/html/ and you can open it with any web browser.  Start from 'index.html'.
Click on the 'Classes' tab then choose 'MemObj'.  That's a good place to start
because it shows you the class inheritance hierarchy starting from MemObj which
you can navigate by clicking on any of the children classes.  Pay special
attention to 'CacheCore', 'WBCache', and 'WTCache' documentation since those
are the classes that you will be modifying.  Doxygen only works with classes so
C functions are not listed.

For those of you who are not familiar with C++, here are a few pointers to
differences with Java:

1. Inheritance

   The syntax for inheritance is:

   ```
   class Cache: public MemObj
   ```

   This means that Cache inherits from the MemObj class.  The 'public' specifier
means that public members in MemObj remain public in Cache.

1. Overriding Methods and Abstract Methods

   In C++, you may often see the 'virtual' keyword before a function:

   ```
   virtual void read(MemRequest *mreq) = 0;
   ```

   Unlike Java, if you want to override a method in the child class, you have to
declare it as 'virtual' in the parent class.  The above method is declared as
part of the Cache class and it is declared as virtual because it is overriden
in the children classes WBCache and WTCache.

   In addition, the ' = 0;' notation says that, this method is an *abstract
method*.  An abstract method is a method with no implementation.  So it's much
like an interface in Java in spirit.  Any class with an abstract method that is
not overriden is called an *abstract class* and objects cannot be instantiated
for an abstract class.  Much like how interfaces cannot be instantiated for
Java.  So you cannot create a Cache object in this case.

1. Creating and Freeing Objects

   Creating objects in C++ is almost identical to Java: you use the 'new' keyword.
The key difference is in freeing of objects.  Java does automatic garbage
collection.  With C++, the programmer has to manually free the object using the
'delete' keyword:

   ```
   mreq = new MemRequest(dinst.inst.Addr, MemRead);
   ...
   delete mreq;
   ```

   As you can see in the above code in CPU.c, you have to delete every object you
create using 'new' or else you will have a memory leak.

[As in Project 1](https://github.com/wonsunahn/CS1541_Project1/#debugging-cc),
the binary is already instrumented with [ASAN (Google Address
Sanitizer)](https://github.com/google/sanitizers/wiki/AddressSanitizer) if you
are running on Mac or Linux (thoth), so it will automatically report back
memory errors like memory leaks, dangling pointer accesses, and buffer
overflows.  If you are running on Windows, unfortunately ASAN is not yet ported
so you can try running your code on thoth to detect memory errors.  You can
also use VSCode debugger to step through your code.

## Task 3: Analyze Performance Data for Arrays and Linked-lists

I have generated instruction traces for the array.cpp and linked-list.cpp files
that we discussed in class.  The source code for
[array.cpp](https://github.com/wonsunahn/CS1541_Project_Tracer/blob/main/array.cpp)
and
[linked-list.cpp](https://github.com/wonsunahn/CS1541_Project_Tracer/blob/main/linked-list.cpp)
are available from the [CS1541_Project_Tracer project repository](
https://github.com/wonsunahn/CS1541_Project_Tracer/tree/main).  The
CS1541_Project_Tracer uses the Pin binary instrumentation tool to instrument a
32-bit x86 binary with instructions to generate traces while a program is
running.  The tool needs a Genuine Intel CPU to run, which many of you don't
have, so I have pre-generated the traces on thoth under the directory
/afs/cs.pitt.edu/courses/1541/plot_traces.

I have also written three new configuration files for the purposes of
performance analysis under the plot_confs/ folder.  All the configuration files
omit the WriteBuffer for easier performance analysis, as we are primarily
interested in understanding cache behavior in this section.

To generate simulation results from the above traces and configurations files,
simple do on thoth.cs.pitt.edu:

```
make plots
```

This will generate two tables of statistics one for IPC (IPC.dat) and one for
cache behavior (MissRates.dat).  Then using GNUPlot, it will generate line
graphs out of those tables: IPC.pdf for the former and MissRates.pdf for the
latter.  You will analyze these tables and line graphs to answer questions in
the Project 2 Retrospective.  If you are not done with the source code
implementation, you can still generate statistics using the provided solution
simulator.  The tables and line graphs generated from the solution are named:
IPC_solution.dat, IPC_solution.pdf, MissRates_solution.dat, and
MissRates_solution.pdf.  If your simulator results do not match the solution
simulator results, please use the solution simulator results to answer the
Retrospective questions.

### Generating Performance Data on your Laptop

You can choose to generate the performance data on your laptop if that is what
you prefer.  But you will first have to copy over the trace files for the
experiments from thoth.  I've intentionally not included the traces files in
the git repository because they are quite large.  You will need approximately
600 MB in total.  If you have that mush space, please cd into the plot_traces folder from the root of the Project 2 repository:

```
cd plot_traces
```

Then, remote copy the traces from thoth using scp (replacing \[PittID\] with your ID):

```
scp [PittID]@thoth.cs.pitt.edu:/afs/cs.pitt.edu/courses/1541/plot_traces/* .
```

Then, edit the [Makefile](Mak$efile) to modify the PLOT_TRACES_DIR variable:

```
PLOT_TRACES_DIR = plot_traces
```

Then, you should be able to generate the plots as in thoth:

```
make plots
```

## Submission

*One member*  will submit the project on behalf of the group.  The submitting
member will press the "View or edit group" link at the top-right corner of the
assignment page after submission to add his/her partner.  That way, both of you
will get a grade.  

You will do two submissions for this deliverable.

1. **(90 points)** Project 2 Source Code 
   
   As for Project 1, the grading will be based on your diff results.

1. **(20 points)** Project 2 Retrospective 

   To be released on GradeScope.  

# Resources

Please refer to [Project 1 Resources](https://github.com/wonsunahn/CS1541_Project1/#resources) on how to
test and debug your program.

