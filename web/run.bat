@echo off
setlocal
chcp 65001 >nul
title He Thong Quan Ly Nhan Su - Web Server
cd /d "%~dp0"

echo =======================================================
echo   HE THONG QUAN LY NHAN SU - KHOI DONG SERVER
echo =======================================================

rem --- Muon bat dang nhap: bo chu "rem" o 2 dong duoi va doi mat khau ---
rem set HRMS_USER=admin
rem set HRMS_PASS=doi-mat-khau-o-day

set NEED_BUILD=0
if not exist "server_app.exe" set NEED_BUILD=1
if "%NEED_BUILD%"=="0" (
    powershell -NoProfile -Command "if ((Get-Item 'server\main_server.cpp').LastWriteTime -gt (Get-Item 'server_app.exe').LastWriteTime) { exit 1 }"
    if errorlevel 1 set NEED_BUILD=1
)

if "%NEED_BUILD%"=="1" (
    where g++ >nul 2>nul
    if errorlevel 1 (
        echo [LOI] Khong tim thay g++. Hay cai MinGW-w64 va them vao PATH.
        pause
        exit /b 1
    )
    echo [1/3] Dang bien dich server C++ ^(lan dau mat 1-2 phut, vui long doi^)...
    g++ -std=c++17 -O2 -I"..\Include" -I"server" -I"server\third_party" -D_WIN32_WINNT=0x0A00 -DWIN32_LEAN_AND_MEAN server\main_server.cpp ..\src\ChamCong\*.cpp ..\src\Exception\*.cpp ..\src\KPI\*.cpp ..\src\Luong\*.cpp ..\src\NhanVien\*.cpp ..\src\PhongBan\*.cpp ..\src\ThongKe\*.cpp -lws2_32 -lwsock32 -lcrypt32 -o server_app.exe
    if errorlevel 1 (
        echo [LOI] Bien dich that bai! Vui long kiem tra trinh bien dich g++.
        pause
        exit /b 1
    )
    echo [OK] Bien dich thanh cong server_app.exe!
) else (
    echo [1/3] Da co san server_app.exe, bo qua bien dich.
)

echo [2/3] Dang khoi dong Web Server tai cong 8080...
rem Mo trinh duyet SAU 3 giay de server kip khoi dong
start "" /min cmd /c "timeout /t 3 /nobreak >nul & start http://localhost:8080"
echo [3/3] Trinh duyet se tu mo tai http://localhost:8080
echo.
echo  *** Dong cua so nay de TAT server ***
echo.

server_app.exe 8080

echo.
echo Server da dung. Neu bao loi cong 8080 thi co the may dang chay mot ban server khac.
pause
