#!/bin/sh

set -e

PORT=$(cat ../PORT.txt)
RPC_HOST="http://127.0.0.1:$PORT" node --experimental-fetch ./node_modules/.bin/uvu -r tsm -r esm test/uvu