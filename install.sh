#!/bin/bash
g++ src/main.cpp src/bull.cpp src/slog.cpp -o bull
sudo chmod +x ./bull
sudo mv bull /usr/local/bin/
