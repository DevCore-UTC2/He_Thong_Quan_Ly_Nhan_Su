#include "../../include/Exception/LoiNghiepVu.h"

LoiNghiepVu::LoiNghiepVu(const string& thongBao) : thongBao(thongBao) {
}

LoiNghiepVu::~LoiNghiepVu() {
}

const char* LoiNghiepVu::what() const noexcept {
    return thongBao.c_str();
}

string LoiNghiepVu::getThongBao() const {
    return thongBao;
}

string LoiNghiepVu::getLoaiLoi() const {
    return "Loi nghiep vu";
}

LoiDuLieuKhongHopLe::LoiDuLieuKhongHopLe(const string& thongBao) : LoiNghiepVu(thongBao) {
}
string LoiDuLieuKhongHopLe::getLoaiLoi() const {
    return "Du lieu khong hop le";
}

LoiKhongTimThay::LoiKhongTimThay(const string& thongBao) : LoiNghiepVu(thongBao) {
}
string LoiKhongTimThay::getLoaiLoi() const {
    return "Khong tim thay";
}

LoiFile::LoiFile(const string& thongBao) : LoiNghiepVu(thongBao) {
}
string LoiFile::getLoaiLoi() const {
    return "Loi file";
}
