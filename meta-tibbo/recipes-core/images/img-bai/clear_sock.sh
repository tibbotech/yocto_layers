#!/bin/sh

ps auxw | grep builder | grep python | grep -v grep | while read x0 x1 x2; do
  echo "Stopping PID:${x1} (${x2})";
  kill ${x1}
  sleep 1;
  kill -KILL ${x1};
done;
find ~/poky/ -name "bitbake.sock" -delete
find ~/poky/ -name "bitbake.lock" -delete
