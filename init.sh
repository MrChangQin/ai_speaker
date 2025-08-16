#!/bin/sh

#  查看key=0x000004d2的共享内存的第二列
# ipcs -m | grep 0x000004d2 | awk '{print $2}'

# 删除key=0x000004d2的共享内存
# ipcrm -m `ipcs -m | grep 0x000004d2 | awk '{print $2}'`

id=`ipcs -m | grep 0x000004d2 | awk '{print $2}'`

if [ ! -z $id ]; then
    ipcrm -m $id
fi
