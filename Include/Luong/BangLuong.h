#ifndef BANGLUONG_H
#define BANGLUONG_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class BangLuong {
private:
    string maNV;
    string hoTen;
    string loaiNhanVien;
    string thang;
    string nam;
    double luongCoBan;
    double phuCap;
    double thuongKPI;
    double khoanBoSung;
    double baoHiem;
    double thue;

public:
    BangLuong();
    BangLuong(string maNV, string hoTen, string loaiNhanVien,
              string thang, string nam,
              double luongCoBan, double phuCap, double thuongKPI,
              double khoanBoSung, double baoHiem, double thue);

    string getMaNV() const;
    string getHoTen() const;
    string getLoaiNhanVien() const;
    string getThang() const;
    string getNam() const;
    double getLuongCoBan() const;
    double getPhuCap() const;
    double getThuongKPI() const;
    double getKhoanBoSung() const;
    double getBaoHiem() const;
    double getThue() const;

    double tongThuNhap() const;
    double luongThucLinh() const;

    void xuat() const;
    static void xuatTongHop(const vector<BangLuong>& dsLuong);
    string xuatHTML() const;
};

#endif
