#ifndef STORAGE_MANAGER_HPP
#define STORAGE_MANAGER_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <mutex>
#include <filesystem>

#include "../adapters/json_adapter.hpp"

namespace fs = std::filesystem;

class StorageManager {
private:
    std::string dataDir;
    mutable std::mutex mtx;

public:
    std::vector<std::shared_ptr<NhanVien>> dsNhanVien;
    std::vector<PhongBan> dsPhongBan;
    std::vector<HopDongLaoDong> dsHopDong;
    std::map<std::string, std::string> hopDongHetHanMap; // maHopDong -> ngayHetHan
    std::vector<ChamCong> dsChamCong;
    std::vector<HeThongKPI> dsKPI;
    std::vector<BangLuong> dsBangLuong;
    ThongKe thongKe;

    StorageManager(const std::string& dataDirectory = "data") : dataDir(dataDirectory) {
        if (!fs::exists(dataDir)) {
            fs::create_directories(dataDir);
        }
        loadAll();
        if (dsNhanVien.empty()) {
            initSampleData();
            saveAll();
        }
    }

    std::shared_ptr<NhanVien> timNhanVien(const std::string& maNV) const {
        for (const auto& nv : dsNhanVien) {
            if (nv && nv->getMaNV() == maNV) return nv;
        }
        return nullptr;
    }

    std::string getPhongBanCuaNV(const std::string& maNV) const {
        for (const auto& pb : dsPhongBan) {
            if (pb.coNhanVien(maNV)) return pb.getTenPhongBan();
        }
        return "Chưa phân bổ";
    }

    std::string getMaPhongBanCuaNV(const std::string& maNV) const {
        for (const auto& pb : dsPhongBan) {
            if (pb.coNhanVien(maNV)) return pb.getMaPhongBan();
        }
        return "";
    }

    // ============================================================
    // DỮ LIỆU MẪU TỪ MAIN_TEST.CPP
    // ============================================================
    void initSampleData() {
        std::lock_guard<std::mutex> lock(mtx);
        dsNhanVien.clear();
        dsPhongBan.clear();
        dsHopDong.clear();
        hopDongHetHanMap.clear();
        dsChamCong.clear();
        dsKPI.clear();
        dsBangLuong.clear();

        // 1. NhanVienToanThoiGian NV001
        auto nvtt = std::make_shared<NhanVienToanThoiGian>();
        nvtt->setMaNV("NV001");
        nvtt->setHoTen("Nguyen Van An");
        nvtt->setNgaySinh("01/01/1990");
        nvtt->setSoDienThoai("0901234567");
        nvtt->setEmail("an@cty.vn");
        nvtt->setTrangThai("Dang lam");
        nvtt->setSoNgayCong(22);
        nvtt->setSoGioLam(200);
        nvtt->setCaLamViec("Ca sang");
        dsNhanVien.push_back(nvtt);

        // 2. NhanVienBanThoiGian NV002
        auto nvbt = std::make_shared<NhanVienBanThoiGian>();
        nvbt->setMaNV("NV002");
        nvbt->setHoTen("Tran Thi Bich");
        nvbt->setNgaySinh("15/06/2000");
        nvbt->setSoDienThoai("0912345678");
        nvbt->setEmail("bich@cty.vn");
        nvbt->setTrangThai("Dang lam");
        nvbt->setSoGioLam(80);
        nvbt->setLuongTheoGio(50000);
        dsNhanVien.push_back(nvbt);

        // 3. QuanLy QL001
        auto ql = std::make_shared<QuanLy>();
        ql->setMaNV("QL001");
        ql->setHoTen("Le Minh Quan");
        ql->setNgaySinh("20/03/1985");
        ql->setSoDienThoai("0932345678");
        ql->setEmail("quan@cty.vn");
        ql->setTrangThai("Dang lam");
        ql->setChucVu("Truong phong");
        ql->setCapQuanLy("Cap trung");
        ql->setThongTinDacThu("Phu trach mang kinh doanh");
        dsNhanVien.push_back(ql);

        // 4. ChuyenGia CG001
        auto cg = std::make_shared<ChuyenGia>();
        cg->setMaNV("CG001");
        cg->setHoTen("Pham Hoang Anh");
        cg->setNgaySinh("10/09/1980");
        cg->setSoDienThoai("0943456789");
        cg->setEmail("anh@cty.vn");
        cg->setTrangThai("Dang lam");
        cg->setChuyenMon("Lap trinh nhung");
        cg->setSoNamKinhNghiem(12);
        dsNhanVien.push_back(cg);

        // 5. Phong ban PB001: Phong Ky Thuat
        PhongBan pb1("PB001", "Phong Ky Thuat");
        pb1.themNhanVien(nvtt);
        pb1.themNhanVien(cg);
        pb1.themNhanVien(ql);
        dsPhongBan.push_back(pb1);

        // Phong ban PB002: Phong Kinh Doanh
        PhongBan pb2("PB002", "Phong Kinh Doanh");
        pb2.themNhanVien(nvbt);
        dsPhongBan.push_back(pb2);

        // 6. HopDongLaoDong
        HopDongLaoDong hd1("HD001", "Chinh thuc", "01/01/2026", 15000000.0, nvtt);
        dsHopDong.push_back(hd1);
        hopDongHetHanMap["HD001"] = "01/01/2029";

        HopDongLaoDong hd2("HD002", "Chinh thuc", "01/03/2026", 30000000.0, cg);
        dsHopDong.push_back(hd2);
        hopDongHetHanMap["HD002"] = "01/03/2029";

        HopDongLaoDong hd3("HD003", "Chinh thuc", "01/02/2026", 25000000.0, ql);
        dsHopDong.push_back(hd3);
        hopDongHetHanMap["HD003"] = "01/02/2029";

        HopDongLaoDong hd4("HD004", "Thoi vu", "01/05/2026", 8000000.0, nvbt);
        dsHopDong.push_back(hd4);
        hopDongHetHanMap["HD004"] = "01/11/2026"; // Sap het han

        // 7. ChamCong
        ChamCong cc1("NV001", 10, 2026, 22, 1);
        cc1.setTrangThai("Da chot cong");
        dsChamCong.push_back(cc1);

        ChamCong cc2("CG001", 10, 2026, 22, 0);
        cc2.setTrangThai("Da chot cong");
        dsChamCong.push_back(cc2);

        ChamCong cc3("QL001", 10, 2026, 24, 0);
        cc3.setTrangThai("Da chot cong");
        dsChamCong.push_back(cc3);

        ChamCong cc4("NV002", 10, 2026, 20, 2);
        cc4.setTrangThai("Dang cham cong");
        dsChamCong.push_back(cc4);

        // 8. KPI
        HeThongKPI kpi1("NV001", 85.0, "10", "2026");
        kpi1.setChinhSachThuong(new ThuongNhanVien());
        dsKPI.push_back(kpi1);

        HeThongKPI kpi2("CG001", 95.0, "10", "2026");
        kpi2.setChinhSachThuong(new ThuongChuyenGia());
        dsKPI.push_back(kpi2);

        HeThongKPI kpi3("QL001", 92.0, "10", "2026");
        kpi3.setChinhSachThuong(new ThuongQuanLy());
        dsKPI.push_back(kpi3);

        HeThongKPI kpi4("NV002", 75.0, "10", "2026");
        kpi4.setChinhSachThuong(new ThuongNhanVien());
        dsKPI.push_back(kpi4);

        // 9. Tinh Luong thang 10/2026 cho NV001 va CG001 (khop 100% voi main_test)
        try {
            auto qt1 = LuongFactory::taoQuyTrinh(*nvtt);
            if (qt1) dsBangLuong.push_back(qt1->tinhLuong(hd1, kpi1, 0, 0, &cc1));

            auto qt2 = LuongFactory::taoQuyTrinh(*cg);
            if (qt2) dsBangLuong.push_back(qt2->tinhLuong(hd2, kpi2, 0, 0, &cc2));

            auto qt3 = LuongFactory::taoQuyTrinh(*ql);
            if (qt3) dsBangLuong.push_back(qt3->tinhLuong(hd3, kpi3, 0, 0, &cc3));
        } catch (const LoiNghiepVu& e) {
            thongKe.ghiNhanLoi(e);
        }
    }

    // ============================================================
    // LƯU TRỮ RA FILE
    // ============================================================
    void saveAll() {
        std::lock_guard<std::mutex> lock(mtx);
        saveEmployees();
        saveDepartments();
        saveContracts();
        saveAttendance();
        saveKPI();
        savePayroll();
    }

    void loadAll() {
        std::lock_guard<std::mutex> lock(mtx);
        loadEmployees();
        loadDepartments();
        loadContracts();
        loadAttendance();
        loadKPI();
        loadPayroll();
    }

    void saveEmployees() {
        json j = json::array();
        for (const auto& nv : dsNhanVien) {
            if (nv) {
                j.push_back(JsonAdapter::nhanVienToJson(nv, getMaPhongBanCuaNV(nv->getMaNV()), getPhongBanCuaNV(nv->getMaNV())));
            }
        }
        std::ofstream f(dataDir + "/employees.json");
        if (f.is_open()) f << j.dump(2);
    }

    void loadEmployees() {
        std::ifstream f(dataDir + "/employees.json");
        if (!f.is_open()) return;
        try {
            json j;
            f >> j;
            dsNhanVien.clear();
            for (const auto& item : j) {
                auto nv = JsonAdapter::jsonToNhanVien(item);
                if (nv) dsNhanVien.push_back(nv);
            }
        } catch (...) {}
    }

    void saveDepartments() {
        json j = json::array();
        for (const auto& pb : dsPhongBan) {
            json item;
            item["maPhongBan"] = pb.getMaPhongBan();
            item["tenPhongBan"] = pb.getTenPhongBan();
            json mems = json::array();
            for (const auto& nv : pb.getDanhSachNhanVien()) {
                if (nv) mems.push_back(nv->getMaNV());
            }
            item["members"] = mems;
            j.push_back(item);
        }
        std::ofstream f(dataDir + "/departments.json");
        if (f.is_open()) f << j.dump(2);
    }

    void loadDepartments() {
        std::ifstream f(dataDir + "/departments.json");
        if (!f.is_open()) return;
        try {
            json j;
            f >> j;
            dsPhongBan.clear();
            for (const auto& item : j) {
                PhongBan pb(item.value("maPhongBan", ""), item.value("tenPhongBan", ""));
                if (item.contains("members") && item["members"].is_array()) {
                    for (const auto& mId : item["members"]) {
                        std::string ma = mId.get<std::string>();
                        auto nv = timNhanVien(ma);
                        if (nv) pb.themNhanVien(nv);
                    }
                }
                dsPhongBan.push_back(pb);
            }
        } catch (...) {}
    }

    void saveContracts() {
        json j = json::array();
        for (const auto& hd : dsHopDong) {
            std::string maHD = hd.getMaHopDong();
            std::string hetHan = hopDongHetHanMap.count(maHD) ? hopDongHetHanMap[maHD] : "";
            j.push_back(JsonAdapter::hopDongToJson(hd, hetHan));
        }
        std::ofstream f(dataDir + "/contracts.json");
        if (f.is_open()) f << j.dump(2);
    }

    void loadContracts() {
        std::ifstream f(dataDir + "/contracts.json");
        if (!f.is_open()) return;
        try {
            json j;
            f >> j;
            dsHopDong.clear();
            hopDongHetHanMap.clear();
            for (const auto& item : j) {
                std::string maHD = item.value("maHopDong", "");
                std::string loaiHD = item.value("loaiHopDong", "");
                std::string ngayKy = item.value("ngayKyKet", "");
                double luong = item.value("luongThoaThuan", 0.0);
                std::string maNV = item.value("maNV", "");
                std::string hetHan = item.value("ngayHetHan", "");

                auto nv = timNhanVien(maNV);
                HopDongLaoDong hd(maHD, loaiHD, ngayKy, luong, nv);
                dsHopDong.push_back(hd);
                if (!hetHan.empty()) {
                    hopDongHetHanMap[maHD] = hetHan;
                }
            }
        } catch (...) {}
    }

    void saveAttendance() {
        json j = json::array();
        for (const auto& cc : dsChamCong) {
            auto nv = timNhanVien(cc.getMaNhanVien());
            std::string hoTen = nv ? nv->getHoTen() : "";
            j.push_back(JsonAdapter::chamCongToJson(cc, hoTen));
        }
        std::ofstream f(dataDir + "/attendance.json");
        if (f.is_open()) f << j.dump(2);
    }

    void loadAttendance() {
        std::ifstream f(dataDir + "/attendance.json");
        if (!f.is_open()) return;
        try {
            json j;
            f >> j;
            dsChamCong.clear();
            for (const auto& item : j) {
                ChamCong cc(item.value("maNhanVien", ""),
                            item.value("thang", 1),
                            item.value("nam", 2026),
                            item.value("soNgayCong", 0),
                            item.value("soNgayNghi", 0));
                cc.setTrangThai(item.value("trangThai", "Dang cham cong"));
                dsChamCong.push_back(cc);
            }
        } catch (...) {}
    }

    void saveKPI() {
        json j = json::array();
        for (const auto& kpi : dsKPI) {
            auto nv = timNhanVien(kpi.getMaNV());
            std::string hoTen = nv ? nv->getHoTen() : "";
            std::string loai = nv ? nv->getLoaiNhanVien() : "";
            j.push_back(JsonAdapter::kpiToJson(kpi, hoTen, loai));
        }
        std::ofstream f(dataDir + "/kpi.json");
        if (f.is_open()) f << j.dump(2);
    }

    void loadKPI() {
        std::ifstream f(dataDir + "/kpi.json");
        if (!f.is_open()) return;
        try {
            json j;
            f >> j;
            dsKPI.clear();
            for (const auto& item : j) {
                std::string maNV = item.value("maNV", "");
                double diem = item.value("diemKPI", 0.0);
                std::string thang = item.value("thang", "10");
                std::string nam = item.value("nam", "2026");
                HeThongKPI kpi(maNV, diem, thang, nam);
                auto nv = timNhanVien(maNV);
                if (nv) {
                    auto cs = LuongFactory::taoChinhSachThuong(*nv);
                    if (cs) {
                        kpi.setChinhSachThuong(cs.release());
                    }
                }
                dsKPI.push_back(kpi);
            }
        } catch (...) {}
    }

    void savePayroll() {
        json j = json::array();
        for (const auto& bl : dsBangLuong) {
            j.push_back(JsonAdapter::bangLuongToJson(bl));
        }
        std::ofstream f(dataDir + "/payroll.json");
        if (f.is_open()) f << j.dump(2);
    }

    void loadPayroll() {
        std::ifstream f(dataDir + "/payroll.json");
        if (!f.is_open()) return;
        try {
            json j;
            f >> j;
            dsBangLuong.clear();
            for (const auto& item : j) {
                BangLuong bl(item.value("maNV", ""),
                             item.value("hoTen", ""),
                             item.value("loaiNhanVien", ""),
                             item.value("thang", ""),
                             item.value("nam", ""),
                             item.value("luongCoBan", 0.0),
                             item.value("phuCap", 0.0),
                             item.value("thuongKPI", 0.0),
                             item.value("khoanBoSung", 0.0),
                             item.value("baoHiem", 0.0),
                             item.value("thue", 0.0));
                dsBangLuong.push_back(bl);
            }
        } catch (...) {}
    }
};

#endif
