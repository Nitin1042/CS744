#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 <n>"
    exit 1
fi

n=$1

# Array to store the PIDs of the created client processes
client_pids=()

# Function to clean up client processes when the script is terminated
cleanup() {
    echo "Terminating all client processes..."
    for pid in "${client_pids[@]}"; do
        kill $pid 2>/dev/null
    done
    exit 0
}

# Trap SIGINT (Ctrl+C) to call the cleanup function
trap cleanup SIGINT

# Create n client processes
for i in $(seq 1 $n); do
    ./test_client &  # Launch the client program in the background
    client_pids+=($!)  # Store the PID of the client process
    echo "Started client $i with PID ${client_pids[-1]}"
done

echo "Press CTRL+C to terminate all clients and exit."

while true; do
	sleep 1
done
