#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "../NhanVien/NhanVien.h"
#include "../PhongBan/PhongBan.h"
#include "../PhongBan/HopDongLaoDong.h"
#include "../KPI/HeThongKPI.h"
#include "../Luong/BangLuong.h"
#include "../ChamCong/ChamCong.h"
#include "../TimKiem/BoLoc.h"

class FileManager {
public:
    typedef BoLoc::DanhSachNhanVien DanhSachNhanVien;

    static void luuNhanVien(const DanhSachNhanVien& ds, const string& duongDan);
    static DanhSachNhanVien docNhanVien(const string& duongDan);

    static void luuHopDong(const vector<HopDongLaoDong>& ds, const string& duongDan);
    static vector<HopDongLaoDong> docHopDong(const string& duongDan, const DanhSachNhanVien& nhanVien);

    static void luuPhongBan(const vector<PhongBan>& ds, const string& duongDan);
    static vector<PhongBan> docPhongBan(const string& duongDan, const DanhSachNhanVien& nhanVien);

    static void luuKPI(const vector<HeThongKPI>& ds, const string& duongDan);
    static vector<HeThongKPI> docKPI(const string& duongDan);

    // Cham cong: moi dong = maNV|thang|nam|soNgayCong|soNgayNghi|trangThai
    static void luuChamCong(const vector<ChamCong>& ds, const string& duongDan);
    static vector<ChamCong> docChamCong(const string& duongDan);

    static void luuBangLuong(const vector<BangLuong>& ds, const string& duongDan);
    static vector<BangLuong> docBangLuong(const string& duongDan);
};

#endif
