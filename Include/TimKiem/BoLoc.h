#ifndef BOLOC_H
#define BOLOC_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

#include "../NhanVien/NhanVien.h"
#include "../Luong/BangLuong.h"
#include "../ChamCong/ChamCong.h"

class BoLoc {
public:
    typedef vector<shared_ptr<NhanVien> > DanhSachNhanVien;

    static shared_ptr<NhanVien> timTheoMa(const DanhSachNhanVien& ds, const string& maNV);
    static DanhSachNhanVien locTheoTen(const DanhSachNhanVien& ds, const string& tuKhoa);
    static DanhSachNhanVien locTheoLoai(const DanhSachNhanVien& ds, const string& tuKhoaLoai);
    // Loc theo trang thai: "Dang lam", "Nghi phep", "Tam nghi", "Da nghi viec" (khong phan biet hoa thuong)
    static DanhSachNhanVien locTheoTrangThai(const DanhSachNhanVien& ds, const string& tuKhoaTrangThai);

    static vector<BangLuong> locLuongTheoMaNV(const vector<BangLuong>& ds, const string& maNV);
    static vector<BangLuong> locLuongTheoKy(const vector<BangLuong>& ds,
                                                 const string& thang, const string& nam);
    static vector<BangLuong> locLuongTheoKhoang(const vector<BangLuong>& ds, double tu, double den);
    static void sapXepLuongThucLinh(vector<BangLuong>& ds, bool giamDan = true);

    // ---- Cham cong ----
    typedef vector<ChamCong> DanhSachChamCong;

    // Tra ve con tro toi ban ghi cua nhan vien trong thang/nam (nullptr neu khong co).
    // Con tro chi dung duoc khi vector ds con song va chua bi them/xoa phan tu.
    static const ChamCong* timChamCong(const DanhSachChamCong& ds, const string& maNV, int thang, int nam);
    static DanhSachChamCong locChamCongTheoMaNV(const DanhSachChamCong& ds, const string& maNV);
    static DanhSachChamCong locChamCongTheoKy(const DanhSachChamCong& ds, int thang, int nam);
    // "Dang cham cong" hoac "Da chot cong"
    static DanhSachChamCong locChamCongTheoTrangThai(const DanhSachChamCong& ds, const string& tuKhoaTrangThai);
};

#endif
