#ifndef QUYTRINHTINHLUONG_H
#define QUYTRINHTINHLUONG_H

#include <iostream>
#include <memory>
#include <string>

using namespace std;

#include "../NhanVien/NhanVien.h"
#include "../HopDong/HopDongLaoDong.h"
#include "../KPI/HeThongKPI.h"
#include "../KPI/IChinhSachThuong.h"
#include "BangLuong.h"

class QuyTrinhTinhLuong {
public:
    virtual ~QuyTrinhTinhLuong();

    BangLuong tinhLuong(const HopDongLaoDong& hd, const HeThongKPI& kpi) const;

protected:
    virtual bool kiemTraLoaiNhanVien(const NhanVien& nv) const = 0;
    virtual double tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien& nv) const = 0;
    virtual double tinhPhuCap(double luongCoBan, const NhanVien& nv) const = 0;

    virtual double tinhKhoanBoSung(double luongCoBan, const NhanVien& nv,
                                   const HeThongKPI& kpi) const;
    virtual double tinhBaoHiem(double luongCoBan) const;
    virtual double tinhThue(double tongThuNhap, double baoHiem) const;
};

class LuongToanThoiGian : public QuyTrinhTinhLuong {
protected:
    bool kiemTraLoaiNhanVien(const NhanVien& nv) const override;
    double tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien& nv) const override;
    double tinhPhuCap(double luongCoBan, const NhanVien& nv) const override;
    double tinhKhoanBoSung(double luongCoBan, const NhanVien& nv,
                           const HeThongKPI& kpi) const override;
};

class LuongBanThoiGian : public QuyTrinhTinhLuong {
protected:
    bool kiemTraLoaiNhanVien(const NhanVien& nv) const override;
    double tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien& nv) const override;
    double tinhPhuCap(double luongCoBan, const NhanVien& nv) const override;
    double tinhBaoHiem(double luongCoBan) const override;
    double tinhThue(double tongThuNhap, double baoHiem) const override;
};

class LuongQuanLy : public QuyTrinhTinhLuong {
protected:
    bool kiemTraLoaiNhanVien(const NhanVien& nv) const override;
    double tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien& nv) const override;
    double tinhPhuCap(double luongCoBan, const NhanVien& nv) const override;
};

class LuongChuyenGia : public QuyTrinhTinhLuong {
protected:
    bool kiemTraLoaiNhanVien(const NhanVien& nv) const override;
    double tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien& nv) const override;
    double tinhPhuCap(double luongCoBan, const NhanVien& nv) const override;
    double tinhKhoanBoSung(double luongCoBan, const NhanVien& nv,
                           const HeThongKPI& kpi) const override;
};

class LuongFactory {
public:
    static unique_ptr<QuyTrinhTinhLuong> taoQuyTrinh(const NhanVien& nv);
    static unique_ptr<IChinhSachThuong> taoChinhSachThuong(const NhanVien& nv);
};

#endif
