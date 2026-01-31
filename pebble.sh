#!/bin/bash
# Helper script to run pebble commands with the SDK environment
export PATH="$HOME/.local/bin:$PATH"
source ~/.pebble-sdk/pebble-env/bin/activate
pebble "$@"
