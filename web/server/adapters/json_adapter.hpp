#ifndef JSON_ADAPTER_HPP
#define JSON_ADAPTER_HPP

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iostream>
#include "../third_party/nlohmann/json.hpp"

#include "../../../Include/NhanVien/NhanVien.h"
#include "../../../Include/NhanVien/NhanVienToanThoiGian.h"
#include "../../../Include/NhanVien/NhanVienBanThoiGian.h"
#include "../../../Include/NhanVien/QuanLy.h"
#include "../../../Include/NhanVien/ChuyenGia.h"
#include "../../../Include/PhongBan/PhongBan.h"
#include "../../../Include/PhongBan/HopDongLaoDong.h"
#include "../../../Include/ChamCong/ChamCong.h"
#include "../../../Include/KPI/HeThongKPI.h"
#include "../../../Include/KPI/IChinhSachThuong.h"
#include "../../../Include/KPI/ThuongNhanVien.h"
#include "../../../Include/KPI/ThuongQuanLy.h"
#include "../../../Include/KPI/ThuongChuyenGia.h"
#include "../../../Include/Luong/BangLuong.h"
#include "../../../Include/Luong/QuyTrinhTinhLuong.h"
#include "../../../Include/ThongKe/ThongKe.h"
#include "../../../Include/Exception/LoiNghiepVu.h"

using json = nlohmann::json;

class JsonAdapter {
public:
    // ==========================================
    // NHÂN VIÊN
    // ==========================================
    static json nhanVienToJson(const std::shared_ptr<NhanVien>& nv, const std::string& maPhongBan = "", const std::string& tenPhongBan = "") {
        if (!nv) return json::object();
        json j;
        j["maNV"] = nv->getMaNV();
        j["hoTen"] = nv->getHoTen();
        j["ngaySinh"] = nv->getNgaySinh();
        j["soDienThoai"] = nv->getSoDienThoai();
        j["email"] = nv->getEmail();
        j["trangThai"] = nv->getTrangThai();
        j["loaiNhanVien"] = nv->getLoaiNhanVien();
        j["maPhongBan"] = maPhongBan;
        j["tenPhongBan"] = tenPhongBan;
        j["hopLe"] = nv->kiemTraHopLe();

        if (auto tt = std::dynamic_pointer_cast<NhanVienToanThoiGian>(nv)) {
            j["soNgayCong"] = tt->getSoNgayCong();
            j["soGioLam"] = tt->getSoGioLam();
            j["caLamViec"] = tt->getCaLamViec();
            j["thongTinRiengHopLe"] = tt->kiemTraThongTinRieng();
            j["loaiId"] = "TOAN_THOI_GIAN";
        } else if (auto bt = std::dynamic_pointer_cast<NhanVienBanThoiGian>(nv)) {
            j["soGioLam"] = bt->getSoGioLam();
            j["luongTheoGio"] = bt->getLuongTheoGio();
            j["thongTinRiengHopLe"] = bt->kiemTraThongTinRieng();
            j["loaiId"] = "BAN_THOI_GIAN";
        } else if (auto ql = std::dynamic_pointer_cast<QuanLy>(nv)) {
            j["chucVu"] = ql->getChucVu();
            j["capQuanLy"] = ql->getCapQuanLy();
            j["thongTinDacThu"] = ql->getThongTinDacThu();
            j["thongTinRiengHopLe"] = ql->kiemTraThongTinRieng();
            j["loaiId"] = "QUAN_LY";
        } else if (auto cg = std::dynamic_pointer_cast<ChuyenGia>(nv)) {
            j["chuyenMon"] = cg->getChuyenMon();
            j["soNamKinhNghiem"] = cg->getSoNamKinhNghiem();
            j["thongTinRiengHopLe"] = cg->kiemTraThongTinRieng();
            j["loaiId"] = "CHUYEN_GIA";
        } else {
            j["loaiId"] = "KHAC";
        }
        return j;
    }

    static std::shared_ptr<NhanVien> jsonToNhanVien(const json& j) {
        std::string loai = j.value("loaiId", "");
        if (loai.empty()) {
            std::string loaiTen = j.value("loaiNhanVien", "");
            if (loaiTen.find("Toan") != std::string::npos) loai = "TOAN_THOI_GIAN";
            else if (loaiTen.find("Ban") != std::string::npos) loai = "BAN_THOI_GIAN";
            else if (loaiTen.find("Quan") != std::string::npos) loai = "QUAN_LY";
            else if (loaiTen.find("Chuyen") != std::string::npos) loai = "CHUYEN_GIA";
            else loai = "TOAN_THOI_GIAN";
        }

        std::shared_ptr<NhanVien> nv;
        if (loai == "TOAN_THOI_GIAN") {
            auto tt = std::make_shared<NhanVienToanThoiGian>();
            tt->setSoNgayCong(j.value("soNgayCong", 0));
            tt->setSoGioLam(j.value("soGioLam", 0.0f));
            tt->setCaLamViec(j.value("caLamViec", "Ca sang"));
            nv = tt;
        } else if (loai == "BAN_THOI_GIAN") {
            auto bt = std::make_shared<NhanVienBanThoiGian>();
            bt->setSoGioLam(j.value("soGioLam", 0.0f));
            bt->setLuongTheoGio(j.value("luongTheoGio", 0.0f));
            nv = bt;
        } else if (loai == "QUAN_LY") {
            auto ql = std::make_shared<QuanLy>();
            ql->setChucVu(j.value("chucVu", ""));
            ql->setCapQuanLy(j.value("capQuanLy", ""));
            ql->setThongTinDacThu(j.value("thongTinDacThu", ""));
            nv = ql;
        } else if (loai == "CHUYEN_GIA") {
            auto cg = std::make_shared<ChuyenGia>();
            cg->setChuyenMon(j.value("chuyenMon", ""));
            cg->setSoNamKinhNghiem(j.value("soNamKinhNghiem", 0));
            nv = cg;
        } else {
            nv = std::make_shared<NhanVienToanThoiGian>();
        }

        nv->setMaNV(j.value("maNV", ""));
        nv->setHoTen(j.value("hoTen", ""));
        nv->setNgaySinh(j.value("ngaySinh", ""));
        nv->setSoDienThoai(j.value("soDienThoai", ""));
        nv->setEmail(j.value("email", ""));
        nv->setTrangThai(j.value("trangThai", "Dang lam"));

        return nv;
    }

    // ==========================================
    // PHÒNG BAN
    // ==========================================
    static json phongBanToJson(const PhongBan& pb) {
        json j;
        j["maPhongBan"] = pb.getMaPhongBan();
        j["tenPhongBan"] = pb.getTenPhongBan();
        j["soLuongNhanVien"] = pb.getSoLuongNhanVien();
        j["hopLe"] = pb.kiemTraHopLe();

        json ds = json::array();
        std::string truongPhongTen = "";
        std::string truongPhongMa = "";

        for (const auto& nv : pb.getDanhSachNhanVien()) {
            json item = nhanVienToJson(nv, pb.getMaPhongBan(), pb.getTenPhongBan());
            ds.push_back(item);
            if (auto ql = std::dynamic_pointer_cast<QuanLy>(nv)) {
                if (ql->getChucVu().find("Truong") != std::string::npos ||
                    ql->getChucVu().find("Trưởng") != std::string::npos) {
                    truongPhongMa = ql->getMaNV();
                    truongPhongTen = ql->getHoTen();
                }
            }
        }
        j["danhSachNhanVien"] = ds;
        j["maTruongPhong"] = truongPhongMa;
        j["tenTruongPhong"] = truongPhongTen;
        return j;
    }

    // ==========================================
    // HỢP ĐỒNG LAO ĐỘNG
    // ==========================================
    static json hopDongToJson(const HopDongLaoDong& hd, const std::string& ngayHetHan = "") {
        json j;
        j["maHopDong"] = hd.getMaHopDong();
        j["loaiHopDong"] = hd.getLoaiHopDong();
        j["ngayKyKet"] = hd.getNgayKyKet();
        j["luongThoaThuan"] = hd.getLuongThoaThuan();
        j["ngayHetHan"] = ngayHetHan;
        j["hopLe"] = hd.kiemTraHopLe();

        auto nv = hd.getNguoiLaoDong();
        if (nv) {
            j["maNV"] = nv->getMaNV();
            j["hoTenNV"] = nv->getHoTen();
            j["loaiNhanVien"] = nv->getLoaiNhanVien();
            j["trangThaiNV"] = nv->getTrangThai();
        } else {
            j["maNV"] = "";
            j["hoTenNV"] = "Chưa gán";
            j["loaiNhanVien"] = "";
            j["trangThaiNV"] = "";
        }
        return j;
    }

    // ==========================================
    // CHẤM CÔNG
    // ==========================================
    static json chamCongToJson(const ChamCong& cc, const std::string& hoTen = "") {
        json j;
        j["maNhanVien"] = cc.getMaNhanVien();
        j["hoTen"] = hoTen;
        j["thang"] = cc.getThang();
        j["nam"] = cc.getNam();
        j["soNgayCong"] = cc.getSoNgayCong();
        j["soNgayNghi"] = cc.getSoNgayNghi();
        j["trangThai"] = cc.getTrangThai();
        j["hopLe"] = cc.kiemTraHopLe();
        return j;
    }

    // ==========================================
    // KPI
    // ==========================================
    static json kpiToJson(const HeThongKPI& kpi, const std::string& hoTen = "", const std::string& loaiNhanVien = "") {
        json j;
        j["maNV"] = kpi.getMaNV();
        j["hoTen"] = hoTen;
        j["loaiNhanVien"] = loaiNhanVien;
        j["diemKPI"] = kpi.getDiemKPI();
        j["thang"] = kpi.getThang();
        j["nam"] = kpi.getNam();
        j["tienThuong"] = kpi.tinhTienThuong();
        j["hopLe"] = kpi.kiemTraHopLe();
        return j;
    }

    // ==========================================
    // BẢNG LƯƠNG
    // ==========================================
    static json bangLuongToJson(const BangLuong& bl) {
        json j;
        j["maNV"] = bl.getMaNV();
        j["hoTen"] = bl.getHoTen();
        j["loaiNhanVien"] = bl.getLoaiNhanVien();
        j["thang"] = bl.getThang();
        j["nam"] = bl.getNam();
        j["luongCoBan"] = bl.getLuongCoBan();
        j["phuCap"] = bl.getPhuCap();
        j["thuongKPI"] = bl.getThuongKPI();
        j["khoanBoSung"] = bl.getKhoanBoSung();
        j["tongThuNhap"] = bl.tongThuNhap();
        j["baoHiem"] = bl.getBaoHiem();
        j["thue"] = bl.getThue();
        j["tongKhauTru"] = bl.getBaoHiem() + bl.getThue();
        j["luongThucLinh"] = bl.luongThucLinh();
        j["htmlSlip"] = bl.xuatHTML();
        return j;
    }

    // ==========================================
    // CHUYỂN HƯỚNG COUT ĐỂ LẤY BÁO CÁO CỦA THONGKE
    // ==========================================
    static std::string captureCout(std::function<void()> fn) {
        std::streambuf* oldCout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());
        try {
            fn();
        } catch (...) {
            std::cout.rdbuf(oldCout);
            throw;
        }
        std::cout.rdbuf(oldCout);
        return oss.str();
    }
};

#endif
