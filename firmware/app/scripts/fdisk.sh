#!/bin/sh
if [ $# != 1 ];then
    echo 'please input the dev to be disk' 
    exit
fi
fdisk $1 <<EOF
d
1
d
2
d
3
d
4
n
p
1


t
b
w
EOF