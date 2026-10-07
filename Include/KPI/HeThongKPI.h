#ifndef HETHONGKPI_H
#define HETHONGKPI_H

#include <iostream>
#include <string>
#include "IChinhSachThuong.h"

using namespace std;

class HeThongKPI {
private:
    string maNV;
    double diemKPI;
    string thang;
    string nam;
    IChinhSachThuong* chinhSachThuong;

public:
    HeThongKPI();
    HeThongKPI(string maNV, double diemKPI, string thang, string nam);
    virtual ~HeThongKPI();

    // Nhập và xuất
    void nhap();
    void xuat() const;

    // Getters
    string getMaNV() const;
    double getDiemKPI() const;
    string getThang() const;
    string getNam() const;

    // Setters
    void setMaNV(string maNV);
    void setDiemKPI(double diemKPI);
    void setThang(string thang);
    void setNam(string nam);

    // Cập nhật thông tin
    void capNhatThongTin();

    // Kiểm tra hợp lệ
    bool kiemTraHopLe() const;

    // Thiết lập chính sách thưởng (Strategy Pattern)
    void setChinhSachThuong(IChinhSachThuong* chinhSach);
    
    // Tính tiền thưởng dựa trên chính sách
    double tinhTienThuong() const;
};

#endif
