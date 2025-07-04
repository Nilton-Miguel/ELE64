#!/bin/sh

audioclocksetup
cd /home/raspberrypi/Sandbox/ELE64/
chrt --rr 99 ./main

