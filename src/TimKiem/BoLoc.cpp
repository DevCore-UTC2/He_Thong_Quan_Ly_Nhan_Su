#include "../../include/TimKiem/BoLoc.h"
#include "../../include/Exception/LoiNghiepVu.h"
#include <algorithm>
#include <cctype>

static string chuThuong(string s) {
    for (size_t i = 0; i < s.size(); i++) {
        s[i] = (char)tolower((unsigned char)s[i]);
    }
    return s;
}

static bool chua(const string& van, const string& tuKhoa) {
    return chuThuong(van).find(chuThuong(tuKhoa)) != string::npos;
}

shared_ptr<NhanVien> BoLoc::timTheoMa(const DanhSachNhanVien& ds, const string& maNV) {
    for (size_t i = 0; i < ds.size(); i++) {
        if (ds[i] != nullptr && ds[i]->getMaNV() == maNV) return ds[i];
    }
    return nullptr;
}

BoLoc::DanhSachNhanVien BoLoc::locTheoTen(const DanhSachNhanVien& ds, const string& tuKhoa) {
    DanhSachNhanVien kq;
    for (size_t i = 0; i < ds.size(); i++) {
        if (ds[i] != nullptr && chua(ds[i]->getHoTen(), tuKhoa)) kq.push_back(ds[i]);
    }
    return kq;
}

BoLoc::DanhSachNhanVien BoLoc::locTheoLoai(const DanhSachNhanVien& ds, const string& tuKhoaLoai) {
    DanhSachNhanVien kq;
    for (size_t i = 0; i < ds.size(); i++) {
        if (ds[i] != nullptr && chua(ds[i]->getLoaiNhanVien(), tuKhoaLoai)) kq.push_back(ds[i]);
    }
    return kq;
}

vector<BangLuong> BoLoc::locLuongTheoMaNV(const vector<BangLuong>& ds, const string& maNV) {
    vector<BangLuong> kq;
    for (size_t i = 0; i < ds.size(); i++) {
        if (ds[i].getMaNV() == maNV) kq.push_back(ds[i]);
    }
    return kq;
}

vector<BangLuong> BoLoc::locLuongTheoKy(const vector<BangLuong>& ds, const string& thang, const string& nam) {
    vector<BangLuong> kq;
    for (size_t i = 0; i < ds.size(); i++) {
        if (ds[i].getThang() == thang && ds[i].getNam() == nam) kq.push_back(ds[i]);
    }
    return kq;
}

vector<BangLuong> BoLoc::locLuongTheoKhoang(const vector<BangLuong>& ds, double tu, double den) {
    if (tu > den) {
        throw LoiDuLieuKhongHopLe("Khoang luong khong hop le: can duoi lon hon can tren");
    }
    vector<BangLuong> kq;
    for (size_t i = 0; i < ds.size(); i++) {
        double l = ds[i].luongThucLinh();
        if (l >= tu && l <= den) kq.push_back(ds[i]);
    }
    return kq;
}

void BoLoc::sapXepLuongThucLinh(vector<BangLuong>& ds, bool giamDan) {
    sort(ds.begin(), ds.end(), [giamDan](const BangLuong& a, const BangLuong& b) {
        return giamDan ? a.luongThucLinh() > b.luongThucLinh()
                       : a.luongThucLinh() < b.luongThucLinh();
    });
}
