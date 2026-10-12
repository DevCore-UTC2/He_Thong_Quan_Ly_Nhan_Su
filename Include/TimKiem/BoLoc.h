#ifndef BOLOC_H
#define BOLOC_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

#include "../NhanVien/NhanVien.h"
#include "../Luong/BangLuong.h"

class BoLoc {
public:
    typedef vector<shared_ptr<NhanVien> > DanhSachNhanVien;

    static shared_ptr<NhanVien> timTheoMa(const DanhSachNhanVien& ds, const string& maNV);
    static DanhSachNhanVien locTheoTen(const DanhSachNhanVien& ds, const string& tuKhoa);
    static DanhSachNhanVien locTheoLoai(const DanhSachNhanVien& ds, const string& tuKhoaLoai);

    static vector<BangLuong> locLuongTheoMaNV(const vector<BangLuong>& ds, const string& maNV);
    static vector<BangLuong> locLuongTheoKy(const vector<BangLuong>& ds,
                                                 const string& thang, const string& nam);
    static vector<BangLuong> locLuongTheoKhoang(const vector<BangLuong>& ds, double tu, double den);
    static void sapXepLuongThucLinh(vector<BangLuong>& ds, bool giamDan = true);
};

#endif
