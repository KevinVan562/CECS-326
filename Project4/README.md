How to compile:
gcc -o fcfs schedule_fcfs.c list.c CPU.c
gcc -o priority schedule_priority.c list.c CPU.c
gcc -o roundrobin schedule_rr.c list.c CPU.c

How to run:
./fcfs schedule.txt
./priority pri-schedule.txt
./roundrobin rr-schedule.txt [quantum time(optional)]
