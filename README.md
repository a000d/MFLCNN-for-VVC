# MFLCNN-for-VVC
Fast Inter Partitioning method for VVC



### This is the demonstration program for the paper *Multi-Feature Lightweight CNN for Fast Inter CU Partitioning in Versatile Video Coding*. The project includes:

1.Modified code integrated with VTM 10.0;\
2.ONNX inference code for model deployment;\
3.Model weight files in ONNX format;\
4.Pre-compiled executables for execution.

# How to run it
#### !The pre-build pragram can only run in Linux system.!

In our tests, the program for original VVC and accelerated VVC are all complied and exectued in the following environment: Ubuntu 20.24.2, AMD EPYC 9654 96 Core processor, 192G Memory 4800 MT/s

The pre-built program is located in the *./run/* folder. Before testing the program, the CTC sequence folder should be placed in this directory. The folder structure should be as follows:

![forder](forder.png)

Next, run the following command to generate shell scripts and an output folder:
``` shell
cd run
python gen_sh.py 
```
This will generate 300 script files in the  */sh* , as shown below:

![sh](sh.png)

These scripts need to be run one by one, which may take one or two days to complete. To accelerate this process, a Python thread manager can be used:

``` shell
python Thread_Mgr.py
```
This script will create multiple threads to run the scripts in the */sh/* folder, saving time.

#### *Note: If the computer performance is insufficient and too many programs run simultaneously, they may compete for system resources, resulting in inaccurate encoding time measurements.*


To adjust the number of concurrent threads, modify the configuration file  *Thread_Cfg.json* 
``` json
{
    "mem_threshold":"0.7",
    "max_thread":"1"
}
```
*mem_threshold* Checks the computer's memory usage. No new threads will be allocated when memory usage exceeds this threshold.

*max_thread* Specifies the maximum number of scripts allowed to run simultaneously. The manager will automatically start the next script once a running one completes.

Dataset can be available at https://ieee-dataport.org//documents/dataset-vvc-inter-frame-fast-partitioning-approach

The training code, training data extraction code will available soon.
