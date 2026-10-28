#!/bin/bash
cd "$(dirname "$0")"

echo "======================================================="
echo "  HE THONG QUAN LY NHAN SU - KHOI DONG SERVER"
echo "======================================================="

if [ ! -f "server_app" ]; then
    echo "[1/3] Dang bien dich server C++ lan dau..."
    g++ -std=c++17 -O2 -I"../Include" -I"server" -I"server/third_party" \
        server/main_server.cpp ../src/*/*.cpp -pthread -o server_app
    if [ $? -ne 0 ]; then
        echo "[LOI] Bien dich that bai! Kiem tra g++ va dependencies."
        exit 1
    fi
    echo "[OK] Bien dich thanh cong server_app!"
else
    echo "[1/3] Da tim thay server_app san sang."
fi

echo "[2/3] Dang khoi dong Web Server tai cong 8080..."
if which xdg-open > /dev/null; then
    xdg-open http://localhost:8080 &
elif which open > /dev/null; then
    open http://localhost:8080 &
fi

./server_app 8080
