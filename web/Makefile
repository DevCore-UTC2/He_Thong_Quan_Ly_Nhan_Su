CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I../Include -Iserver -Iserver/third_party

ifeq ($(OS),Windows_NT)
    CXXFLAGS += -D_WIN32_WINNT=0x0A00 -DWIN32_LEAN_AND_MEAN
    LDFLAGS += -lws2_32 -lwsock32 -lcrypt32
    TARGET = server_app.exe
else
    LDFLAGS += -pthread
    TARGET = server_app
endif

SRCS = server/main_server.cpp \
       ../src/ChamCong/ChamCong.cpp \
       ../src/Exception/LoiNghiepVu.cpp \
       ../src/KPI/HeThongKPI.cpp \
       ../src/KPI/IChinhSachThuong.cpp \
       ../src/KPI/ThuongChuyenGia.cpp \
       ../src/KPI/ThuongNhanVien.cpp \
       ../src/KPI/ThuongQuanLy.cpp \
       ../src/Luong/BangLuong.cpp \
       ../src/Luong/QuyTrinhTinhLuong.cpp \
       ../src/NhanVien/ChuyenGia.cpp \
       ../src/NhanVien/NhanVien.cpp \
       ../src/NhanVien/NhanVienBanThoiGian.cpp \
       ../src/NhanVien/NhanVienToanThoiGian.cpp \
       ../src/NhanVien/QuanLy.cpp \
       ../src/PhongBan/HopDongLaoDong.cpp \
       ../src/PhongBan/PhongBan.cpp \
       ../src/ThongKe/ThongKe.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	del /Q $(TARGET) server\*.o 2>nul || true
else
	rm -f $(TARGET) server/*.o ../src/*/*.o
endif

.PHONY: all clean
