#ifndef THONGKE_H
#define THONGKE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

using namespace std;

#include "../NhanVien/NhanVien.h"
#include "../PhongBan/PhongBan.h"
#include "../PhongBan/HopDongLaoDong.h"
#include "../KPI/HeThongKPI.h"
#include "../ChamCong/ChamCong.h"
#include "../Luong/BangLuong.h"
#include "../Exception/LoiNghiepVu.h"

// Thong ke: tong hop so lieu tu cac module hien co (nhan vien, phong ban, hop dong,
// cham cong, KPI, bang luong, loi nghiep vu).
// - Cac ham thong ke du lieu la static: nhan vao danh sach (vector) roi in ket qua, KHONG sua du lieu goc.
// - Loi nghiep vu la ngoai le (exception) nen khong luu o dau ca; doi tuong ThongKe giu nhat ky loi,
//   goi ghiNhanLoi() trong khoi catch de cong don.
class ThongKe
{
private:
    map<string, int> demLoi;        // loai loi -> so lan xuat hien
    vector<string> nhatKyLoi;       // "[Loai loi] thong bao" theo thu tu xay ra

public:
    ThongKe();

    // ===== Loi nghiep vu =====
    void ghiNhanLoi(const LoiNghiepVu& loi);   // ghi nhan LoiDuLieuKhongHopLe, LoiChamCong, ...
    void ghiNhanLoi(const exception& loi);     // loi khac (std::exception) -> loai "Loi he thong"
    int tongSoLoi() const;
    int demTheoLoai(const string& loaiLoi) const;   // VD: "Loi cham cong"
    void xoaNhatKyLoi();
    void thongKeLoi(int soDongNhatKyToiDa = 10) const;

    // ===== Du lieu cac module khac =====
    static void thongKeNhanVien(const vector<shared_ptr<NhanVien>>& dsNhanVien);
    static void thongKePhongBan(const vector<PhongBan>& dsPhongBan);
    static void thongKeHopDong(const vector<HopDongLaoDong>& dsHopDong);
    static void thongKeChamCong(const vector<ChamCong>& dsChamCong);
    static void thongKeKPI(const vector<HeThongKPI>& dsKPI);
    static void thongKeBangLuong(const vector<BangLuong>& dsLuong);

    // ===== Bao cao tong hop: goi lan luot tat ca cac thong ke tren =====
    // Nhan vien duoc lay tu cac phong ban (trung ma chi tinh mot lan).
    void baoCaoTongHop(const vector<PhongBan>& dsPhongBan,
                       const vector<HopDongLaoDong>& dsHopDong,
                       const vector<ChamCong>& dsChamCong,
                       const vector<HeThongKPI>& dsKPI,
                       const vector<BangLuong>& dsLuong) const;
};

#endif
