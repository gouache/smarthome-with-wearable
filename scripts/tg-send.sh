#!/bin/bash

to=$1
msg=$2
img=$3
(sleep 5; echo "msg $to $msg"; echo "safe_quit") | telegram-cli -W -R
(sleep 5; echo "send_photo $to $img"; echo "safe_quit") | telegram-cli -W -R
