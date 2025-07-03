#!/bin/bash
g++ main.cpp bull.cpp slog.cpp -o bull
chmod +x bull
sudo mv bull /usr/local/bin/
