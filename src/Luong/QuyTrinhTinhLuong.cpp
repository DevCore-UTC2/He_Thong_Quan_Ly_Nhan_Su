#include "QuyTrinhTinhLuong.h"
#include "LoiNghiepVu.h"
#include "NhanVienToanThoiGian.h"
#include "NhanVienBanThoiGian.h"
#include "QuanLy.h"
#include "ChuyenGia.h"
#include "ThuongNhanVien.h"
#include "ThuongQuanLy.h"
#include "ThuongChuyenGia.h"
#include "ChamCong.h"
#include <cstdlib>

static const double TY_LE_BAO_HIEM = 0.105;
static const double GIAM_TRU_BAN_THAN = 11000000;
static const double NGAY_CONG_CHUAN = 26;
static const double GIO_MOI_NGAY = 8;
static const double HE_SO_TANG_CA = 1.5;

QuyTrinhTinhLuong::~QuyTrinhTinhLuong() {
}

// Doc "10" -> 10; tra false neu khong phai so
static bool docSoNguyenDuong(const string& s, int& ketQua) {
    if (s.empty()) return false;
    char* cuoi = nullptr;
    long v = strtol(s.c_str(), &cuoi, 10);
    if (*cuoi != '\0') return false;
    ketQua = (int)v;
    return true;
}

// Cham cong dung de tinh luong phai: hop le, dung nhan vien, dung ky luong va da chot cong
static void kiemTraChamCongChoLuong(const ChamCong& cc, const NhanVien& nv, const HeThongKPI& kpi) {
    if (!cc.kiemTraHopLe()) {
        throw LoiChamCong("Du lieu cham cong khong hop le (kiem tra ma NV, thang/nam, so ngay cong, ngay nghi, trang thai)");
    }
    if (cc.getMaNhanVien() != nv.getMaNV()) {
        throw LoiChamCong("Cham cong cua nhan vien " + cc.getMaNhanVien() +
                          " khong thuoc nhan vien " + nv.getMaNV());
    }
    int thangKPI = 0, namKPI = 0;
    if (!docSoNguyenDuong(kpi.getThang(), thangKPI) || !docSoNguyenDuong(kpi.getNam(), namKPI) ||
        cc.getThang() != thangKPI || cc.getNam() != namKPI) {
        throw LoiChamCong("Cham cong thang " + to_string(cc.getThang()) + "/" + to_string(cc.getNam()) +
                          " khong khop ky luong " + kpi.getThang() + "/" + kpi.getNam());
    }
    if (cc.getTrangThai() != "Da chot cong") {
        throw LoiChamCong("Cong thang " + to_string(cc.getThang()) + "/" + to_string(cc.getNam()) +
                          " cua nhan vien " + nv.getMaNV() + " chua chot, khong the tinh luong");
    }
}

BangLuong QuyTrinhTinhLuong::tinhLuong(const HopDongLaoDong& hd, const HeThongKPI& kpi,
                                       double phuCapThem, double phucLoiThem,
                                       const ChamCong* chamCong) const {
    if (!hd.kiemTraHopLe()) {
        throw LoiDuLieuKhongHopLe("Hop dong khong hop le (thieu thong tin hoac chua gan nhan vien)");
    }
    if (!kpi.kiemTraHopLe()) {
        throw LoiDuLieuKhongHopLe("Du lieu KPI khong hop le");
    }
    shared_ptr<NhanVien> nv = hd.getNguoiLaoDong();
    if (kpi.getMaNV() != nv->getMaNV()) {
        throw LoiDuLieuKhongHopLe("KPI (" + kpi.getMaNV() + ") khong thuoc nhan vien trong hop dong (" + nv->getMaNV() + ")");
    }
    if (!kiemTraLoaiNhanVien(*nv)) {
        throw LoiDuLieuKhongHopLe("Quy trinh tinh luong khong phu hop voi loai nhan vien: " + nv->getLoaiNhanVien());
    }
    if (nv->getTrangThai() == "Da nghi viec") {
        throw LoiTrangThaiNhanVien("Nhan vien " + nv->getMaNV() + " da nghi viec, khong tinh luong");
    }
    if (phuCapThem < 0 || phucLoiThem < 0) {
        throw LoiDuLieuKhongHopLe("Phu cap / phuc loi nhap them khong duoc am");
    }
    if (chamCong != nullptr) {
        kiemTraChamCongChoLuong(*chamCong, *nv, kpi);
    }

    double luongCoBan = tinhLuongCoBan(hd, *nv, chamCong);

    double phuCap = tinhPhuCap(luongCoBan, *nv) + phuCapThem;

    double thuongKPI = kpi.tinhTienThuong();

    double khoanBoSung = tinhKhoanBoSung(luongCoBan, *nv, kpi, chamCong) + phucLoiThem;

    double tongThuNhap = luongCoBan + phuCap + thuongKPI + khoanBoSung;
    double baoHiem = tinhBaoHiem(luongCoBan);
    double thue = tinhThue(tongThuNhap, baoHiem);

    return BangLuong(nv->getMaNV(), nv->getHoTen(), nv->getLoaiNhanVien(),
                     kpi.getThang(), kpi.getNam(),
                     luongCoBan, phuCap, thuongKPI, khoanBoSung, baoHiem, thue);
}

double QuyTrinhTinhLuong::tinhKhoanBoSung(double, const NhanVien&, const HeThongKPI&, const ChamCong*) const {
    return 0;
}

double QuyTrinhTinhLuong::tinhBaoHiem(double luongCoBan) const {
    return luongCoBan * TY_LE_BAO_HIEM;
}

double QuyTrinhTinhLuong::tinhThue(double tongThuNhap, double baoHiem) const {
    double chiuThue = tongThuNhap - baoHiem - GIAM_TRU_BAN_THAN;
    if (chiuThue <= 0) return 0;

    const double tran[3] = {5000000, 10000000, 18000000};
    const double tyLe[4] = {0.05, 0.10, 0.15, 0.20};

    double thue = 0;
    double mocTruoc = 0;
    for (int i = 0; i < 3; i++) {
        if (chiuThue > tran[i]) {
            thue += (tran[i] - mocTruoc) * tyLe[i];
            mocTruoc = tran[i];
        } else {
            return thue + (chiuThue - mocTruoc) * tyLe[i];
        }
    }
    return thue + (chiuThue - mocTruoc) * tyLe[3];
}

bool LuongToanThoiGian::kiemTraLoaiNhanVien(const NhanVien& nv) const {
    return dynamic_cast<const NhanVienToanThoiGian*>(&nv) != nullptr;
}

// So ngay cong dung de tinh luong: uu tien du lieu cham cong, khong co thi lay tu nhan vien
static int ngayCongThucTe(const NhanVienToanThoiGian& t, const ChamCong* chamCong) {
    return chamCong != nullptr ? chamCong->getSoNgayCong() : t.getSoNgayCong();
}

double LuongToanThoiGian::tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien& nv,
                                         const ChamCong* chamCong) const {
    const NhanVienToanThoiGian& t = static_cast<const NhanVienToanThoiGian&>(nv);
    return hd.getLuongThoaThuan() / NGAY_CONG_CHUAN * ngayCongThucTe(t, chamCong);
}

double LuongToanThoiGian::tinhPhuCap(double luongCoBan, const NhanVien&) const {
    return luongCoBan * 0.05;
}

double LuongToanThoiGian::tinhKhoanBoSung(double luongCoBan, const NhanVien& nv,
                                          const HeThongKPI&, const ChamCong* chamCong) const {
    const NhanVienToanThoiGian& t = static_cast<const NhanVienToanThoiGian&>(nv);
    int ngayCong = ngayCongThucTe(t, chamCong);
    if (ngayCong <= 0) return 0;

    double gioTangCa = t.getSoGioLam() - GIO_MOI_NGAY * ngayCong;
    if (gioTangCa <= 0) return 0;

    double donGiaGio = luongCoBan / (ngayCong * GIO_MOI_NGAY);
    return gioTangCa * donGiaGio * HE_SO_TANG_CA;
}

bool LuongBanThoiGian::kiemTraLoaiNhanVien(const NhanVien& nv) const {
    return dynamic_cast<const NhanVienBanThoiGian*>(&nv) != nullptr;
}

double LuongBanThoiGian::tinhLuongCoBan(const HopDongLaoDong&, const NhanVien& nv, const ChamCong*) const {
    const NhanVienBanThoiGian& t = static_cast<const NhanVienBanThoiGian&>(nv);
    return t.getSoGioLam() * t.getLuongTheoGio();
}

double LuongBanThoiGian::tinhPhuCap(double, const NhanVien&) const {
    return 0;
}

double LuongBanThoiGian::tinhBaoHiem(double) const {
    return 0;
}

double LuongBanThoiGian::tinhThue(double tongThuNhap, double) const {
    if (tongThuNhap >= 2000000) return tongThuNhap * 0.10;
    return 0;
}

bool LuongQuanLy::kiemTraLoaiNhanVien(const NhanVien& nv) const {
    return dynamic_cast<const QuanLy*>(&nv) != nullptr;
}

double LuongQuanLy::tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien&, const ChamCong*) const {
    return hd.getLuongThoaThuan();
}

double LuongQuanLy::tinhPhuCap(double luongCoBan, const NhanVien&) const {
    return luongCoBan * 0.20;
}

bool LuongChuyenGia::kiemTraLoaiNhanVien(const NhanVien& nv) const {
    return dynamic_cast<const ChuyenGia*>(&nv) != nullptr;
}

double LuongChuyenGia::tinhLuongCoBan(const HopDongLaoDong& hd, const NhanVien&, const ChamCong*) const {
    return hd.getLuongThoaThuan();
}

double LuongChuyenGia::tinhPhuCap(double luongCoBan, const NhanVien& nv) const {
    const ChuyenGia& cg = static_cast<const ChuyenGia&>(nv);
    double tyLe = 0.10 + 0.01 * cg.getSoNamKinhNghiem();
    if (tyLe > 0.30) tyLe = 0.30;
    return luongCoBan * tyLe;
}

double LuongChuyenGia::tinhKhoanBoSung(double luongCoBan, const NhanVien&,
                                       const HeThongKPI& kpi, const ChamCong*) const {
    if (kpi.getDiemKPI() >= 90) return luongCoBan * 0.10;
    return 0;
}

unique_ptr<QuyTrinhTinhLuong> LuongFactory::taoQuyTrinh(const NhanVien& nv) {
    if (dynamic_cast<const NhanVienToanThoiGian*>(&nv)) return unique_ptr<QuyTrinhTinhLuong>(new LuongToanThoiGian());
    if (dynamic_cast<const NhanVienBanThoiGian*>(&nv))  return unique_ptr<QuyTrinhTinhLuong>(new LuongBanThoiGian());
    if (dynamic_cast<const QuanLy*>(&nv))               return unique_ptr<QuyTrinhTinhLuong>(new LuongQuanLy());
    if (dynamic_cast<const ChuyenGia*>(&nv))            return unique_ptr<QuyTrinhTinhLuong>(new LuongChuyenGia());
    return nullptr;
}

unique_ptr<IChinhSachThuong> LuongFactory::taoChinhSachThuong(const NhanVien& nv) {
    if (dynamic_cast<const NhanVienToanThoiGian*>(&nv) || dynamic_cast<const NhanVienBanThoiGian*>(&nv))
        return unique_ptr<IChinhSachThuong>(new ThuongNhanVien());
    if (dynamic_cast<const QuanLy*>(&nv))    return unique_ptr<IChinhSachThuong>(new ThuongQuanLy());
    if (dynamic_cast<const ChuyenGia*>(&nv)) return unique_ptr<IChinhSachThuong>(new ThuongChuyenGia());
    return nullptr;
}

