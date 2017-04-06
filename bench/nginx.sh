#!/bin/sh

# locate
if [ ! -n "$BASH_SOURCE" ]; then
    SCRIPT_DIR=`dirname "$(readlink -f "$0")"`
else
    F=$BASH_SOURCE
    while [ -h "$F" ]; do
        F="$(readlink "$F")"
    done
    SCRIPT_DIR=`dirname "$F"`
fi

cd $SCRIPT_DIR

[ ! -n "$WORKERS" ] && WORKERS=1

[ ! -n "$CONF" ] && CONF=nginx.conf

mkdir -p target/nginx/logs

/opt/nginx/sbin/nginx -p target/nginx -c $SCRIPT_DIR/nginx/$CONF -g "pid nginx.pid; error_log nginx.error.log; worker_processes $WORKERS;" $@

