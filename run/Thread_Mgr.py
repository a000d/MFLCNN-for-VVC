import os
import threading
import time
import random
import psutil
import json
import datetime

lock = threading.Lock()
thread_list=[]

mem_threshold = 0.7
max_thread = 1

def Log(v):
    with open("thread_log.txt",'a') as f:
        f.write("{}".format(v))
        f.write("\n")
        print(v)

def mem_permit():
    mem = psutil.virtual_memory()
    total = mem.total
    used = mem.used

    Log("memory:{}".format(float(used)/float(total)))
    if float(used)/float(total)>mem_threshold:
        return False
    return True

def Read_Cfg():
    global mem_threshold
    global max_thread
    with open("Thread_Cfg.json",'r') as f:
        cfg = f.read()
        cfg = json.loads(cfg)

        if(float(cfg["mem_threshold"])!=mem_threshold or max_thread != int(cfg["max_thread"])):
            Log("mem_threshold:{} max_thread:{}".format(float(cfg["mem_threshold"]),int(cfg["max_thread"])))

        mem_threshold = float(cfg["mem_threshold"])
        max_thread = int(cfg["max_thread"])
        


def thread_cmd(cmd_info):
    global thread_count

    (cmd, index) = cmd_info

    with lock:
        thread_list.append(index)
        sorted(thread_list)

        Log("{}---{}--Start,Running:{}, total:{}".format(datetime.datetime.now(),cmd,thread_list,len(thread_list)))

    


    os.system(cmd)

    with lock:
        thread_list.remove(index)
        Log(datetime.datetime.now())
        Log("{}---{}--Done ,Running:{}, total:{}".format(datetime.datetime.now(),cmd,thread_list,len(thread_list)))


cmd_list = []
Log(datetime.datetime.now())
for i in range(0,300):
    cmd_list.append(("bash ./sh/{}.sh".format(i), i))

for cmd in cmd_list:
    Read_Cfg()
    
    while len(thread_list) >= max_thread or not mem_permit():
        Read_Cfg()
        time.sleep(60)
    thread = threading.Thread(target=thread_cmd, args=(cmd, ))
    thread.start()
    
    if len(thread_list) <= 1:
        time.sleep(0.02)
    else:
        time.sleep(10)

while len(thread_list)>0:
    time.sleep(40)

exit()








