#!/usr/bin/env bash

set -a
source config.env
set +a

python http_server.py
