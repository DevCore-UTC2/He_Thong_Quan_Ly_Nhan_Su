#ifndef QUYTRINHTINHLUONG_H
#define QUYTRINHTINHLUONG_H

#include <iostream>
#include <memory>
#include <string>

using namespace std;

#include "../NhanVien/NhanVien.h"
#include "../PhongBan/HopDongLaoDong.h"
#include "../KPI/HeThongKPI.h"
#include "../KPI/IChinhSachThuong.h"
#include "../ChamCong/ChamCong.h"
#include "BangLuong.h"

class QuyTrinhTinhLuong {
public:
    virtual ~QuyTrinhTinhLuong();

    // Tinh luong cho 1 nhan vien trong ky cua KPI.
    //  - phuCapThem / phucLoiThem: khoan nhap tay cong them vao phu cap / khoan bo sung (mac dinh 0).
    //  - chamCong: ban ghi cham cong cua nhan vien (mac dinh nullptr = dung so ngay cong luu trong
    //    NhanVienToanThoiGian nhu truoc). Neu co truyen vao thi phai dung nhan vien, dung thang/nam
    //    cua KPI va da "Da chot cong", neu khong nem LoiChamCong.
    //  - Nhan vien co trang thai "Da nghi viec" thi nem LoiTrangThaiNhanVien.
    BangLuong tinhLuong(const HopDongLaoDong& hd, const HeThongKPI& kpi,
                        double phuCapThem = 0, double phucLoiThem = 0,
                        const ChamCong* chamCong = nullptr) const;

protected:
    virtual bool kiemTraLoaiNhanVien(const NhanVien& nv) const = 0;
    virtual double tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien& nv,
                                  const ChamCong* chamCong) const = 0;
    virtual double tinhPhuCap(double luongCoBan, const NhanVien& nv) const = 0;

    virtual double tinhKhoanBoSung(double luongCoBan, const NhanVien& nv,
                                   const HeThongKPI& kpi, const ChamCong* chamCong) const;
    virtual double tinhBaoHiem(double luongCoBan) const;
    virtual double tinhThue(double tongThuNhap, double baoHiem) const;
};

class LuongToanThoiGian : public QuyTrinhTinhLuong {
protected:
    bool kiemTraLoaiNhanVien(const NhanVien& nv) const override;
    double tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien& nv,
                          const ChamCong* chamCong) const override;
    double tinhPhuCap(double luongCoBan, const NhanVien& nv) const override;
    double tinhKhoanBoSung(double luongCoBan, const NhanVien& nv,
                           const HeThongKPI& kpi, const ChamCong* chamCong) const override;
};

class LuongBanThoiGian : public QuyTrinhTinhLuong {
protected:
    bool kiemTraLoaiNhanVien(const NhanVien& nv) const override;
    double tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien& nv,
                          const ChamCong* chamCong) const override;
    double tinhPhuCap(double luongCoBan, const NhanVien& nv) const override;
    double tinhBaoHiem(double luongCoBan) const override;
    double tinhThue(double tongThuNhap, double baoHiem) const override;
};

class LuongQuanLy : public QuyTrinhTinhLuong {
protected:
    bool kiemTraLoaiNhanVien(const NhanVien& nv) const override;
    double tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien& nv,
                          const ChamCong* chamCong) const override;
    double tinhPhuCap(double luongCoBan, const NhanVien& nv) const override;
};

class LuongChuyenGia : public QuyTrinhTinhLuong {
protected:
    bool kiemTraLoaiNhanVien(const NhanVien& nv) const override;
    double tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien& nv,
                          const ChamCong* chamCong) const override;
    double tinhPhuCap(double luongCoBan, const NhanVien& nv) const override;
    double tinhKhoanBoSung(double luongCoBan, const NhanVien& nv,
                           const HeThongKPI& kpi, const ChamCong* chamCong) const override;
};

class LuongFactory {
public:
    static unique_ptr<QuyTrinhTinhLuong> taoQuyTrinh(const NhanVien& nv);
    static unique_ptr<IChinhSachThuong> taoChinhSachThuong(const NhanVien& nv);
};

#endif
