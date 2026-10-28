#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "third_party/httplib.h"
#include "third_party/nlohmann/json.hpp"
#include "storage/storage_manager.hpp"
#include "adapters/json_adapter.hpp"

#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <mutex>
#include <cstdlib>

using json = nlohmann::json;

// Khoa toan cuc: httplib chay nhieu luong, con StorageManager la du lieu dung chung.
// Moi API chay tuan tu de tranh hai request cung sua danh sach mot luc.
static std::recursive_mutex g_apiMutex;

// Helper: Response JSON
static void jsonResponse(httplib::Response& res, int statusCode, bool success, const json& data, const std::string& message = "") {
    json body;
    body["success"] = success;
    body["message"] = message;
    body["data"] = data;
    res.status = statusCode;
    res.set_content(body.dump(), "application/json; charset=utf-8");
}

static void errorResponse(httplib::Response& res, int statusCode, const std::string& message) {
    jsonResponse(res, statusCode, false, nullptr, message);
}

// Kiem tra ngay dang DD/MM/YYYY: dung dinh dang, dung lich (co nam nhuan), nam 1900-2100.
// Tra ve true neu hop le va (neu can) ghi ra khoa so sanh yyyymmdd.
static bool ngayHopLe(const std::string& s, long* khoa = nullptr) {
    if (s.size() != 10 || s[2] != '/' || s[5] != '/') return false;
    for (size_t i = 0; i < s.size(); ++i) {
        if (i == 2 || i == 5) continue;
        if (s[i] < '0' || s[i] > '9') return false;
    }
    int d = std::stoi(s.substr(0, 2));
    int m = std::stoi(s.substr(3, 2));
    int y = std::stoi(s.substr(6, 4));
    if (y < 1900 || y > 2100 || m < 1 || m > 12 || d < 1) return false;
    static const int ngayThang[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int toiDa = ngayThang[m - 1];
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) toiDa = 29;
    if (d > toiDa) return false;
    if (khoa) *khoa = (long)y * 10000 + m * 100 + d;
    return true;
}

// Kiem tra ngay ky / ngay het han cua hop dong. Tra ve chuoi loi, rong neu hop le.
static std::string kiemTraNgayHopDong(const std::string& ngayKy, const std::string& hetHan) {
    long kKy = 0, kHet = 0;
    if (!ngayHopLe(ngayKy, &kKy))
        return "Ngày ký kết không hợp lệ. Nhập đúng định dạng DD/MM/YYYY, ví dụ 01/01/2026 (không nhập chữ hoặc số âm)";
    if (!hetHan.empty()) {
        if (!ngayHopLe(hetHan, &kHet))
            return "Ngày hết hạn không hợp lệ. Nhập đúng định dạng DD/MM/YYYY hoặc để trống nếu không thời hạn";
        if (kHet < kKy) return "Ngày hết hạn không được trước ngày ký kết";
    }
    return "";
}

int main(int argc, char* argv[]) {
    int port = 8080;
    if (const char* envPort = std::getenv("PORT")) {
        if (*envPort) port = std::stoi(envPort);
    }
    if (argc > 1) {
        port = std::stoi(argv[1]);
    }

    std::string dataDir = "data";
    std::string publicDir = "public";

    // Khởi tạo StorageManager
    StorageManager storage(dataDir);

    httplib::Server svr;

    // CORS handler
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });

    // Dang nhap HTTP Basic - TUY CHON: chi bat khi dat bien moi truong HRMS_USER va HRMS_PASS.
    // Khong dat thi chay nhu cu (phu hop chay may ca nhan). Trinh duyet tu hoi mat khau, khong can sua frontend.
    {
        const char* u = std::getenv("HRMS_USER");
        const char* pw = std::getenv("HRMS_PASS");
        if (u && pw && *u && *pw) {
            const std::string expected = "Basic " + httplib::detail::base64_encode(std::string(u) + ":" + pw);
            svr.set_pre_routing_handler([expected](const httplib::Request& req, httplib::Response& res) {
                if (req.method == "OPTIONS") return httplib::Server::HandlerResponse::Unhandled;
                const std::string got = req.get_header_value("Authorization");
                bool ok = got.size() == expected.size();
                unsigned char diff = 0;
                for (size_t i = 0; ok && i < got.size(); ++i) diff |= (unsigned char)(got[i] ^ expected[i]);
                if (ok && diff == 0) return httplib::Server::HandlerResponse::Unhandled;
                res.status = 401;
                res.set_header("WWW-Authenticate", "Basic realm=\"HRMS\", charset=\"UTF-8\"");
                res.set_content("Yeu cau dang nhap", "text/plain; charset=utf-8");
                return httplib::Server::HandlerResponse::Handled;
            });
            std::cout << "[BAO MAT] Da bat dang nhap Basic (HRMS_USER/HRMS_PASS)\n";
        } else {
            std::cout << "[CANH BAO] Chua dat HRMS_USER/HRMS_PASS: API dang mo, khong can dang nhap\n";
        }
    }

    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    // ============================================================
    // 1. API NHÂN VIÊN
    // ============================================================
    svr.Get("/api/employees", [&storage](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string q = req.get_param_value("q");
        std::string type = req.get_param_value("type");
        std::string dept = req.get_param_value("dept");
        std::string status = req.get_param_value("status");

        json arr = json::array();
        for (const auto& nv : storage.dsNhanVien) {
            if (!nv) continue;
            std::string maPB = storage.getMaPhongBanCuaNV(nv->getMaNV());
            std::string tenPB = storage.getPhongBanCuaNV(nv->getMaNV());

            // Lọc theo search query (maNV, hoTen, email, soDienThoai)
            if (!q.empty()) {
                std::string qL = q;
                std::transform(qL.begin(), qL.end(), qL.begin(), ::tolower);
                std::string hoTenL = nv->getHoTen();
                std::transform(hoTenL.begin(), hoTenL.end(), hoTenL.begin(), ::tolower);
                std::string maNVL = nv->getMaNV();
                std::transform(maNVL.begin(), maNVL.end(), maNVL.begin(), ::tolower);

                if (hoTenL.find(qL) == std::string::npos && maNVL.find(qL) == std::string::npos) {
                    continue;
                }
            }

            // Lọc theo phòng ban
            if (!dept.empty() && maPB != dept) continue;

            // Lọc theo trạng thái
            if (!status.empty() && nv->getTrangThai() != status) continue;

            json item = JsonAdapter::nhanVienToJson(nv, maPB, tenPB);

            // Lọc theo loại
            if (!type.empty() && item.value("loaiId", "") != type && item.value("loaiNhanVien", "") != type) {
                continue;
            }

            arr.push_back(item);
        }
        jsonResponse(res, 200, true, arr, "Lấy danh sách nhân viên thành công");
    });

    svr.Get(R"(/api/employees/([A-Za-z0-9_-]+))", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string id = req.matches[1];
        auto nv = storage.timNhanVien(id);
        if (!nv) {
            errorResponse(res, 404, "Không tìm thấy nhân viên có mã " + id);
            return;
        }
        std::string maPB = storage.getMaPhongBanCuaNV(nv->getMaNV());
        std::string tenPB = storage.getPhongBanCuaNV(nv->getMaNV());
        jsonResponse(res, 200, true, JsonAdapter::nhanVienToJson(nv, maPB, tenPB));
    });

    svr.Post("/api/employees", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        try {
            json body = json::parse(req.body);
            std::string maNV = body.value("maNV", "");
            if (maNV.empty()) {
                errorResponse(res, 400, "Mã nhân viên không được để trống");
                return;
            }
            if (storage.timNhanVien(maNV) != nullptr) {
                errorResponse(res, 400, "Mã nhân viên " + maNV + " đã tồn tại trong hệ thống");
                return;
            }

            auto nv = JsonAdapter::jsonToNhanVien(body);
            if (!nv->kiemTraHopLe()) {
                errorResponse(res, 400, "Dữ liệu nhân viên không hợp lệ (kiểm tra họ tên, ngày sinh, email, sđt)");
                return;
            }

            // Gán vào phòng ban nếu có
            std::string maPB = body.value("maPhongBan", "");
            if (!maPB.empty()) {
                for (auto& pb : storage.dsPhongBan) {
                    if (pb.getMaPhongBan() == maPB) {
                        pb.themNhanVien(nv);
                        break;
                    }
                }
            }

            storage.dsNhanVien.push_back(nv);
            storage.saveAll();
            jsonResponse(res, 201, true, JsonAdapter::nhanVienToJson(nv, maPB, storage.getPhongBanCuaNV(maNV)), "Thêm nhân viên thành công");
        } catch (const std::exception& e) {
            errorResponse(res, 400, std::string("Lỗi xử lý: ") + e.what());
        }
    });

    svr.Put(R"(/api/employees/([A-Za-z0-9_-]+))", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string id = req.matches[1];
        auto nv = storage.timNhanVien(id);
        if (!nv) {
            errorResponse(res, 404, "Không tìm thấy nhân viên " + id);
            return;
        }

        try {
            json body = json::parse(req.body);
            if (body.contains("hoTen")) nv->setHoTen(body["hoTen"]);
            if (body.contains("ngaySinh")) nv->setNgaySinh(body["ngaySinh"]);
            if (body.contains("soDienThoai")) nv->setSoDienThoai(body["soDienThoai"]);
            if (body.contains("email")) nv->setEmail(body["email"]);
            if (body.contains("trangThai")) nv->setTrangThai(body["trangThai"]);

            if (auto tt = std::dynamic_pointer_cast<NhanVienToanThoiGian>(nv)) {
                if (body.contains("soNgayCong")) tt->setSoNgayCong(body["soNgayCong"]);
                if (body.contains("soGioLam")) tt->setSoGioLam(body["soGioLam"]);
                if (body.contains("caLamViec")) tt->setCaLamViec(body["caLamViec"]);
            } else if (auto bt = std::dynamic_pointer_cast<NhanVienBanThoiGian>(nv)) {
                if (body.contains("soGioLam")) bt->setSoGioLam(body["soGioLam"]);
                if (body.contains("luongTheoGio")) bt->setLuongTheoGio(body["luongTheoGio"]);
            } else if (auto ql = std::dynamic_pointer_cast<QuanLy>(nv)) {
                if (body.contains("chucVu")) ql->setChucVu(body["chucVu"]);
                if (body.contains("capQuanLy")) ql->setCapQuanLy(body["capQuanLy"]);
                if (body.contains("thongTinDacThu")) ql->setThongTinDacThu(body["thongTinDacThu"]);
            } else if (auto cg = std::dynamic_pointer_cast<ChuyenGia>(nv)) {
                if (body.contains("chuyenMon")) cg->setChuyenMon(body["chuyenMon"]);
                if (body.contains("soNamKinhNghiem")) cg->setSoNamKinhNghiem(body["soNamKinhNghiem"]);
            }

            if (!nv->kiemTraHopLe()) {
                errorResponse(res, 400, "Thông tin cập nhật không hợp lệ");
                return;
            }

            // Cập nhật chuyển phòng ban nếu có
            if (body.contains("maPhongBan")) {
                std::string newPB = body["maPhongBan"];
                std::string curPB = storage.getMaPhongBanCuaNV(id);
                if (curPB != newPB) {
                    if (!curPB.empty()) {
                        for (auto& pb : storage.dsPhongBan) {
                            if (pb.getMaPhongBan() == curPB) pb.xoaNhanVien(id);
                        }
                    }
                    if (!newPB.empty()) {
                        for (auto& pb : storage.dsPhongBan) {
                            if (pb.getMaPhongBan() == newPB) pb.themNhanVien(nv);
                        }
                    }
                }
            }

            storage.saveAll();
            jsonResponse(res, 200, true, JsonAdapter::nhanVienToJson(nv, storage.getMaPhongBanCuaNV(id), storage.getPhongBanCuaNV(id)), "Cập nhật nhân viên thành công");
        } catch (const std::exception& e) {
            errorResponse(res, 400, std::string("Lỗi: ") + e.what());
        }
    });

    svr.Delete(R"(/api/employees/([A-Za-z0-9_-]+))", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string id = req.matches[1];
        auto it = std::find_if(storage.dsNhanVien.begin(), storage.dsNhanVien.end(), [&id](const std::shared_ptr<NhanVien>& nv) {
            return nv && nv->getMaNV() == id;
        });

        if (it == storage.dsNhanVien.end()) {
            errorResponse(res, 404, "Không tìm thấy nhân viên " + id);
            return;
        }

        // Gỡ khỏi mọi phòng ban
        for (auto& pb : storage.dsPhongBan) {
            pb.xoaNhanVien(id);
        }

        storage.dsNhanVien.erase(it);
        storage.saveAll();
        jsonResponse(res, 200, true, nullptr, "Đã xóa nhân viên " + id + " thành công");
    });

    // ============================================================
    // 2. API PHÒNG BAN
    // ============================================================
    svr.Get("/api/departments", [&storage](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        json arr = json::array();
        for (const auto& pb : storage.dsPhongBan) {
            arr.push_back(JsonAdapter::phongBanToJson(pb));
        }
        jsonResponse(res, 200, true, arr);
    });

    svr.Get(R"(/api/departments/([A-Za-z0-9_-]+))", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string id = req.matches[1];
        for (const auto& pb : storage.dsPhongBan) {
            if (pb.getMaPhongBan() == id) {
                jsonResponse(res, 200, true, JsonAdapter::phongBanToJson(pb));
                return;
            }
        }
        errorResponse(res, 404, "Không tìm thấy phòng ban " + id);
    });

    svr.Post("/api/departments", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        try {
            json body = json::parse(req.body);
            std::string maPB = body.value("maPhongBan", "");
            std::string tenPB = body.value("tenPhongBan", "");

            if (maPB.empty() || tenPB.empty()) {
                errorResponse(res, 400, "Mã phòng ban và tên phòng ban không được để trống");
                return;
            }

            for (const auto& pb : storage.dsPhongBan) {
                if (pb.getMaPhongBan() == maPB) {
                    errorResponse(res, 400, "Mã phòng ban " + maPB + " đã tồn tại");
                    return;
                }
            }

            PhongBan pb(maPB, tenPB);
            if (!pb.kiemTraHopLe()) {
                errorResponse(res, 400, "Thông tin phòng ban không hợp lệ");
                return;
            }

            storage.dsPhongBan.push_back(pb);
            storage.saveDepartments();
            jsonResponse(res, 201, true, JsonAdapter::phongBanToJson(pb), "Tạo phòng ban mới thành công");
        } catch (const std::exception& e) {
            errorResponse(res, 400, std::string("Lỗi: ") + e.what());
        }
    });

    svr.Put(R"(/api/departments/([A-Za-z0-9_-]+))", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string id = req.matches[1];
        for (auto& pb : storage.dsPhongBan) {
            if (pb.getMaPhongBan() == id) {
                try {
                    json body = json::parse(req.body);
                    if (body.contains("tenPhongBan")) pb.setTenPhongBan(body["tenPhongBan"]);
                    storage.saveDepartments();
                    jsonResponse(res, 200, true, JsonAdapter::phongBanToJson(pb), "Cập nhật phòng ban thành công");
                    return;
                } catch (const std::exception& e) {
                    errorResponse(res, 400, e.what());
                    return;
                }
            }
        }
        errorResponse(res, 404, "Không tìm thấy phòng ban " + id);
    });

    svr.Delete(R"(/api/departments/([A-Za-z0-9_-]+))", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string id = req.matches[1];
        auto it = std::find_if(storage.dsPhongBan.begin(), storage.dsPhongBan.end(), [&id](const PhongBan& pb) {
            return pb.getMaPhongBan() == id;
        });

        if (it == storage.dsPhongBan.end()) {
            errorResponse(res, 404, "Không tìm thấy phòng ban " + id);
            return;
        }

        storage.dsPhongBan.erase(it);
        storage.saveDepartments();
        jsonResponse(res, 200, true, nullptr, "Đã xóa phòng ban " + id);
    });

    svr.Post(R"(/api/departments/([A-Za-z0-9_-]+)/members)", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string id = req.matches[1];
        try {
            json body = json::parse(req.body);
            std::string maNV = body.value("maNV", "");
            auto nv = storage.timNhanVien(maNV);
            if (!nv) {
                errorResponse(res, 404, "Không tìm thấy nhân viên " + maNV);
                return;
            }

            for (auto& pb : storage.dsPhongBan) {
                if (pb.getMaPhongBan() == id) {
                    if (pb.coNhanVien(maNV)) {
                        errorResponse(res, 400, "Nhân viên đã có trong phòng ban này");
                        return;
                    }
                    // Gỡ khỏi phòng ban cũ nếu có
                    for (auto& otherPb : storage.dsPhongBan) {
                        if (otherPb.getMaPhongBan() != id) otherPb.xoaNhanVien(maNV);
                    }
                    pb.themNhanVien(nv);
                    storage.saveDepartments();
                    jsonResponse(res, 200, true, JsonAdapter::phongBanToJson(pb), "Đã thêm nhân viên vào phòng ban");
                    return;
                }
            }
            errorResponse(res, 404, "Không tìm thấy phòng ban " + id);
        } catch (const std::exception& e) {
            errorResponse(res, 400, e.what());
        }
    });

    svr.Delete(R"(/api/departments/([A-Za-z0-9_-]+)/members/([A-Za-z0-9_-]+))", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string id = req.matches[1];
        std::string empId = req.matches[2];

        for (auto& pb : storage.dsPhongBan) {
            if (pb.getMaPhongBan() == id) {
                if (pb.xoaNhanVien(empId)) {
                    storage.saveDepartments();
                    jsonResponse(res, 200, true, JsonAdapter::phongBanToJson(pb), "Đã gỡ nhân viên khỏi phòng ban");
                } else {
                    errorResponse(res, 404, "Nhân viên không tồn tại trong phòng ban này");
                }
                return;
            }
        }
        errorResponse(res, 404, "Không tìm thấy phòng ban " + id);
    });

    // ============================================================
    // 3. API HỢP ĐỒNG LAO ĐỘNG
    // ============================================================
    svr.Get("/api/contracts", [&storage](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        json arr = json::array();
        for (const auto& hd : storage.dsHopDong) {
            std::string maHD = hd.getMaHopDong();
            std::string hetHan = storage.hopDongHetHanMap.count(maHD) ? storage.hopDongHetHanMap[maHD] : "";
            arr.push_back(JsonAdapter::hopDongToJson(hd, hetHan));
        }
        jsonResponse(res, 200, true, arr);
    });

    svr.Post("/api/contracts", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        try {
            json body = json::parse(req.body);
            std::string maHD = body.value("maHopDong", "");
            std::string loaiHD = body.value("loaiHopDong", "");
            std::string ngayKy = body.value("ngayKyKet", "");
            double luong = body.value("luongThoaThuan", 0.0);
            std::string maNV = body.value("maNV", "");
            std::string hetHan = body.value("ngayHetHan", "");

            if (maHD.empty()) {
                errorResponse(res, 400, "Mã hợp đồng không được để trống");
                return;
            }

            for (const auto& hd : storage.dsHopDong) {
                if (hd.getMaHopDong() == maHD) {
                    errorResponse(res, 400, "Mã hợp đồng " + maHD + " đã tồn tại");
                    return;
                }
            }

            auto nv = storage.timNhanVien(maNV);
            if (!nv) {
                errorResponse(res, 400, "Nhân viên không hợp lệ hoặc chưa được chọn");
                return;
            }

            {
                std::string loiNgay = kiemTraNgayHopDong(ngayKy, hetHan);
                if (!loiNgay.empty()) {
                    errorResponse(res, 400, loiNgay);
                    return;
                }
                // Constructor cua HopDongLaoDong tu doi luong am thanh 0 nen kiemTraHopLe() khong bat duoc
                if (luong < 0) {
                    errorResponse(res, 400, "Lương thỏa thuận không được là số âm");
                    return;
                }
            }

            HopDongLaoDong hd(maHD, loaiHD, ngayKy, luong, nv);
            if (!hd.kiemTraHopLe()) {
                errorResponse(res, 400, "Thông tin hợp đồng không hợp lệ (kiểm tra mức lương >= 0 và ngày ký)");
                return;
            }

            storage.dsHopDong.push_back(hd);
            if (!hetHan.empty()) storage.hopDongHetHanMap[maHD] = hetHan;
            storage.saveContracts();

            jsonResponse(res, 201, true, JsonAdapter::hopDongToJson(hd, hetHan), "Thêm hợp đồng thành công");
        } catch (const std::exception& e) {
            errorResponse(res, 400, e.what());
        }
    });

    svr.Put(R"(/api/contracts/([A-Za-z0-9_-]+))", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string id = req.matches[1];
        for (auto& hd : storage.dsHopDong) {
            if (hd.getMaHopDong() == id) {
                try {
                    json body = json::parse(req.body);
                    {
                        std::string ky = body.contains("ngayKyKet") ? body["ngayKyKet"].get<std::string>() : hd.getNgayKyKet();
                        std::string het = body.contains("ngayHetHan") ? body["ngayHetHan"].get<std::string>()
                                          : (storage.hopDongHetHanMap.count(id) ? storage.hopDongHetHanMap[id] : std::string());
                        std::string loiNgay = kiemTraNgayHopDong(ky, het);
                        if (!loiNgay.empty()) {
                            errorResponse(res, 400, loiNgay);
                            return;
                        }
                        if (body.contains("luongThoaThuan") && body["luongThoaThuan"].is_number() && body["luongThoaThuan"].get<double>() < 0) {
                            errorResponse(res, 400, "Lương thỏa thuận không được âm");
                            return;
                        }
                    }
                    if (body.contains("loaiHopDong")) hd.setLoaiHopDong(body["loaiHopDong"]);
                    if (body.contains("ngayKyKet")) hd.setNgayKyKet(body["ngayKyKet"]);
                    if (body.contains("luongThoaThuan")) hd.setLuongThoaThuan(body["luongThoaThuan"]);
                    if (body.contains("maNV")) {
                        auto nv = storage.timNhanVien(body["maNV"]);
                        if (nv) hd.setNguoiLaoDong(nv);
                    }
                    if (body.contains("ngayHetHan")) {
                        storage.hopDongHetHanMap[id] = body["ngayHetHan"];
                    }

                    if (!hd.kiemTraHopLe()) {
                        errorResponse(res, 400, "Thông tin hợp đồng không hợp lệ sau khi sửa");
                        return;
                    }

                    storage.saveContracts();
                    jsonResponse(res, 200, true, JsonAdapter::hopDongToJson(hd, storage.hopDongHetHanMap[id]), "Cập nhật hợp đồng thành công");
                    return;
                } catch (const std::exception& e) {
                    errorResponse(res, 400, e.what());
                    return;
                }
            }
        }
        errorResponse(res, 404, "Không tìm thấy hợp đồng " + id);
    });

    svr.Delete(R"(/api/contracts/([A-Za-z0-9_-]+))", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string id = req.matches[1];
        auto it = std::find_if(storage.dsHopDong.begin(), storage.dsHopDong.end(), [&id](const HopDongLaoDong& hd) {
            return hd.getMaHopDong() == id;
        });

        if (it == storage.dsHopDong.end()) {
            errorResponse(res, 404, "Không tìm thấy hợp đồng " + id);
            return;
        }

        storage.dsHopDong.erase(it);
        storage.hopDongHetHanMap.erase(id);
        storage.saveContracts();
        jsonResponse(res, 200, true, nullptr, "Đã xóa hợp đồng " + id);
    });

    // ============================================================
    // 4. API CHẤM CÔNG
    // ============================================================
    svr.Get("/api/attendance", [&storage](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        int thang = 0, nam = 0;
        if (req.has_param("month")) thang = std::stoi(req.get_param_value("month"));
        if (req.has_param("year")) nam = std::stoi(req.get_param_value("year"));

        json arr = json::array();
        for (const auto& cc : storage.dsChamCong) {
            if (thang > 0 && cc.getThang() != thang) continue;
            if (nam > 0 && cc.getNam() != nam) continue;
            auto nv = storage.timNhanVien(cc.getMaNhanVien());
            arr.push_back(JsonAdapter::chamCongToJson(cc, nv ? nv->getHoTen() : ""));
        }
        jsonResponse(res, 200, true, arr);
    });

    svr.Post("/api/attendance", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        try {
            json body = json::parse(req.body);
            std::string maNV = body.value("maNhanVien", "");
            int thang = body.value("thang", 10);
            int nam = body.value("nam", 2026);
            int soNgayCong = body.value("soNgayCong", 0);
            int soNgayNghi = body.value("soNgayNghi", 0);
            std::string trangThai = body.value("trangThai", "Dang cham cong");

            auto nv = storage.timNhanVien(maNV);
            if (!nv) {
                errorResponse(res, 404, "Mã nhân viên " + maNV + " không tồn tại");
                return;
            }

            // Kiểm tra xem đã có bản ghi chấm công tháng này chưa
            auto it = std::find_if(storage.dsChamCong.begin(), storage.dsChamCong.end(), [&](const ChamCong& c) {
                return c.getMaNhanVien() == maNV && c.getThang() == thang && c.getNam() == nam;
            });

            if (it != storage.dsChamCong.end()) {
                // Cập nhật bản ghi có sẵn qua C++ method
                if (it->getTrangThai() == "Da chot cong" && trangThai != "Dang cham cong") {
                    throw LoiChamCong("Công tháng " + std::to_string(thang) + "/" + std::to_string(nam) + " của nhân viên " + maNV + " đã chốt, không thể chỉnh sửa!");
                }
                if (body.contains("trangThai")) it->setTrangThai(trangThai);
                if (!it->capNhatSoNgayCong(soNgayCong)) {
                    throw LoiChamCong("Số ngày công không hợp lệ hoặc tháng đã chốt công!");
                }
                if (!it->capNhatSoNgayNghi(soNgayNghi)) {
                    throw LoiChamCong("Số ngày nghỉ không hợp lệ hoặc tháng đã chốt công!");
                }
            } else {
                ChamCong cc(maNV, thang, nam, soNgayCong, soNgayNghi);
                cc.setTrangThai(trangThai);
                if (!cc.kiemTraHopLe()) {
                    throw LoiChamCong("Bản ghi chấm công không hợp lệ (tổng số ngày công + nghỉ vượt quá số ngày của tháng hoặc âm)");
                }
                storage.dsChamCong.push_back(cc);
            }

            storage.saveAttendance();
            jsonResponse(res, 200, true, nullptr, "Ghi nhận chấm công thành công");
        } catch (const LoiNghiepVu& e) {
            storage.thongKe.ghiNhanLoi(e);
            errorResponse(res, 400, std::string("[") + e.getLoaiLoi() + "] " + e.getThongBao());
        } catch (const std::exception& e) {
            storage.thongKe.ghiNhanLoi(e);
            errorResponse(res, 400, e.what());
        }
    });

    svr.Post(R"(/api/attendance/([A-Za-z0-9_-]+)/lock)", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string maNV = req.matches[1];
        int thang = 10, nam = 2026;
        if (req.has_param("month")) thang = std::stoi(req.get_param_value("month"));
        if (req.has_param("year")) nam = std::stoi(req.get_param_value("year"));

        for (auto& cc : storage.dsChamCong) {
            if (cc.getMaNhanVien() == maNV && cc.getThang() == thang && cc.getNam() == nam) {
                cc.setTrangThai("Da chot cong");
                storage.saveAttendance();
                jsonResponse(res, 200, true, JsonAdapter::chamCongToJson(cc), "Đã chốt công tháng " + std::to_string(thang) + "/" + std::to_string(nam) + " cho " + maNV);
                return;
            }
        }
        errorResponse(res, 404, "Không tìm thấy bản ghi chấm công");
    });

    // ============================================================
    // 5. API KPI & THƯỞNG
    // ============================================================
    svr.Get("/api/kpi", [&storage](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string thang = req.get_param_value("month");
        std::string nam = req.get_param_value("year");

        json arr = json::array();
        for (const auto& kpi : storage.dsKPI) {
            if (!thang.empty() && kpi.getThang() != thang) continue;
            if (!nam.empty() && kpi.getNam() != nam) continue;

            auto nv = storage.timNhanVien(kpi.getMaNV());
            std::string hoTen = nv ? nv->getHoTen() : "";
            std::string loai = nv ? nv->getLoaiNhanVien() : "";
            arr.push_back(JsonAdapter::kpiToJson(kpi, hoTen, loai));
        }
        jsonResponse(res, 200, true, arr);
    });

    svr.Post("/api/kpi", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        try {
            json body = json::parse(req.body);
            std::string maNV = body.value("maNV", "");
            double diem = body.value("diemKPI", 0.0);
            std::string thang = body.value("thang", "10");
            std::string nam = body.value("nam", "2026");

            auto nv = storage.timNhanVien(maNV);
            if (!nv) {
                throw LoiKhongTimThay("Không tìm thấy nhân viên " + maNV);
            }

            auto it = std::find_if(storage.dsKPI.begin(), storage.dsKPI.end(), [&](const HeThongKPI& k) {
                return k.getMaNV() == maNV && k.getThang() == thang && k.getNam() == nam;
            });

            if (it != storage.dsKPI.end()) {
                it->setDiemKPI(diem);
                auto cs = LuongFactory::taoChinhSachThuong(*nv);
                if (cs) it->setChinhSachThuong(cs.release());
            } else {
                HeThongKPI kpi(maNV, diem, thang, nam);
                if (!kpi.kiemTraHopLe()) {
                    throw LoiDuLieuKhongHopLe("Điểm KPI hoặc thông tin kỳ đánh giá không hợp lệ");
                }
                auto cs = LuongFactory::taoChinhSachThuong(*nv);
                if (cs) kpi.setChinhSachThuong(cs.release());
                storage.dsKPI.push_back(kpi);
            }

            storage.saveKPI();
            jsonResponse(res, 200, true, nullptr, "Lưu điểm KPI thành công");
        } catch (const LoiNghiepVu& e) {
            storage.thongKe.ghiNhanLoi(e);
            errorResponse(res, 400, std::string("[") + e.getLoaiLoi() + "] " + e.getThongBao());
        } catch (const std::exception& e) {
            errorResponse(res, 400, e.what());
        }
    });

    // ============================================================
    // 6. API TÍNH LƯƠNG & BẢNG LƯƠNG (QuyTrinhTinhLuong + Factory)
    // ============================================================
    svr.Get("/api/payroll", [&storage](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string thang = req.get_param_value("month");
        std::string nam = req.get_param_value("year");

        json arr = json::array();
        for (const auto& bl : storage.dsBangLuong) {
            if (!thang.empty() && bl.getThang() != thang) continue;
            if (!nam.empty() && bl.getNam() != nam) continue;
            arr.push_back(JsonAdapter::bangLuongToJson(bl));
        }
        jsonResponse(res, 200, true, arr);
    });

    svr.Post("/api/payroll/calculate", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        try {
            json body = json::parse(req.body);
            std::string thang = body.value("thang", "10");
            std::string nam = body.value("nam", "2026");
            std::string targetNV = body.value("maNV", ""); // Nếu có thì tính cho 1 người, rỗng thì tính cho tất cả

            int thangNum = std::stoi(thang);
            int namNum = std::stoi(nam);

            std::vector<BangLuong> tinhDuoc;
            std::vector<std::string> errors;

            auto processOne = [&](const std::shared_ptr<NhanVien>& nv) {
                if (!nv) return;

                // 1. Tìm hợp đồng
                const HopDongLaoDong* hopDongPtr = nullptr;
                for (const auto& hd : storage.dsHopDong) {
                    if (hd.getNguoiLaoDong() && hd.getNguoiLaoDong()->getMaNV() == nv->getMaNV()) {
                        hopDongPtr = &hd;
                        break;
                    }
                }
                if (!hopDongPtr) {
                    errors.push_back("Nhân viên " + nv->getMaNV() + " (" + nv->getHoTen() + ") chưa có hợp đồng lao động");
                    return;
                }

                // 2. Tìm KPI
                const HeThongKPI* kpiPtr = nullptr;
                for (const auto& k : storage.dsKPI) {
                    if (k.getMaNV() == nv->getMaNV() && k.getThang() == thang && k.getNam() == nam) {
                        kpiPtr = &k;
                        break;
                    }
                }
                if (!kpiPtr) {
                    errors.push_back("Nhân viên " + nv->getMaNV() + " (" + nv->getHoTen() + "): chưa có KPI kỳ " + thang + "/" + nam + ", không thể tính lương");
                    return;
                }

                // 3. Tìm Chấm công
                const ChamCong* chamCongPtr = nullptr;
                for (const auto& c : storage.dsChamCong) {
                    if (c.getMaNhanVien() == nv->getMaNV() && c.getThang() == thangNum && c.getNam() == namNum) {
                        chamCongPtr = &c;
                        break;
                    }
                }

                try {
                    auto quyTrinh = LuongFactory::taoQuyTrinh(*nv);
                    if (!quyTrinh) {
                        errors.push_back("Không xác định được quy trình tính lương cho loại: " + nv->getLoaiNhanVien());
                        return;
                    }

                    BangLuong bl = quyTrinh->tinhLuong(*hopDongPtr, *kpiPtr, 0, 0, chamCongPtr);
                    tinhDuoc.push_back(bl);

                    // Cập nhật vào danh sách bảng lương lưu trữ
                    auto it = std::find_if(storage.dsBangLuong.begin(), storage.dsBangLuong.end(), [&](const BangLuong& b) {
                        return b.getMaNV() == nv->getMaNV() && b.getThang() == thang && b.getNam() == nam;
                    });
                    if (it != storage.dsBangLuong.end()) {
                        *it = bl;
                    } else {
                        storage.dsBangLuong.push_back(bl);
                    }
                } catch (const LoiNghiepVu& e) {
                    storage.thongKe.ghiNhanLoi(e);
                    errors.push_back("Nhân viên " + nv->getMaNV() + ": [" + e.getLoaiLoi() + "] " + e.getThongBao());
                }
            };

            if (!targetNV.empty()) {
                auto nv = storage.timNhanVien(targetNV);
                if (!nv) {
                    errorResponse(res, 404, "Không tìm thấy nhân viên " + targetNV);
                    return;
                }
                processOne(nv);
            } else {
                for (const auto& nv : storage.dsNhanVien) {
                    processOne(nv);
                }
            }

            storage.savePayroll();

            json resData;
            json items = json::array();
            for (const auto& bl : tinhDuoc) {
                items.push_back(JsonAdapter::bangLuongToJson(bl));
            }
            resData["items"] = items;
            resData["errors"] = errors;
            resData["tinhThanhCong"] = tinhDuoc.size();

            std::string msg = "Tính lương thành công cho " + std::to_string(tinhDuoc.size()) + " nhân sự.";
            if (!errors.empty()) {
                msg += " (Có " + std::to_string(errors.size()) + " lưu ý/lỗi nghiệp vụ C++ được bắt)";
            }
            jsonResponse(res, 200, true, resData, msg);
        } catch (const std::exception& e) {
            errorResponse(res, 400, e.what());
        }
    });

    svr.Get(R"(/api/payroll/([A-Za-z0-9_-]+)/slip)", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string maNV = req.matches[1];
        std::string thang = req.has_param("month") ? req.get_param_value("month") : "10";
        std::string nam = req.has_param("year") ? req.get_param_value("year") : "2026";

        for (const auto& bl : storage.dsBangLuong) {
            if (bl.getMaNV() == maNV && bl.getThang() == thang && bl.getNam() == nam) {
                jsonResponse(res, 200, true, {
                    {"html", bl.xuatHTML()},
                    {"payroll", JsonAdapter::bangLuongToJson(bl)}
                });
                return;
            }
        }
        errorResponse(res, 404, "Chưa có bảng lương của nhân viên " + maNV + " kỳ " + thang + "/" + nam);
    });

    // ============================================================
    // 7. API THỐNG KÊ (Class ThongKe gốc của C++)
    // ============================================================
    svr.Get("/api/stats/summary", [&storage](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        json j;
        j["tongNhanVien"] = storage.dsNhanVien.size();

        int tt = 0, bt = 0, ql = 0, cg = 0;
        for (const auto& nv : storage.dsNhanVien) {
            if (!nv) continue;
            std::string l = nv->getLoaiNhanVien();
            if (l.find("Toan") != std::string::npos) tt++;
            else if (l.find("Ban") != std::string::npos) bt++;
            else if (l.find("Quan") != std::string::npos) ql++;
            else if (l.find("Chuyen") != std::string::npos) cg++;
        }
        j["theoLoai"] = {
            {"ToanThoiGian", tt},
            {"BanThoiGian", bt},
            {"QuanLy", ql},
            {"ChuyenGia", cg}
        };

        j["tongPhongBan"] = storage.dsPhongBan.size();
        json deptStats = json::array();
        for (const auto& pb : storage.dsPhongBan) {
            deptStats.push_back({
                {"maPhongBan", pb.getMaPhongBan()},
                {"tenPhongBan", pb.getTenPhongBan()},
                {"soLuong", pb.getSoLuongNhanVien()}
            });
        }
        j["theoPhongBan"] = deptStats;

        double tongLuongThucLinh = 0;
        for (const auto& bl : storage.dsBangLuong) {
            tongLuongThucLinh += bl.luongThucLinh();
        }
        j["tongQuyLuong"] = tongLuongThucLinh;
        j["tongBangLuong"] = storage.dsBangLuong.size();

        double tongDiemKPI = 0;
        for (const auto& kpi : storage.dsKPI) {
            tongDiemKPI += kpi.getDiemKPI();
        }
        j["kpiTrungBinh"] = storage.dsKPI.empty() ? 0.0 : (tongDiemKPI / storage.dsKPI.size());

        j["tongLoiGhiNhan"] = storage.thongKe.tongSoLoi();
        j["demLoiChamCong"] = storage.thongKe.demTheoLoai("Loi cham cong");
        j["demLoiDuLieu"] = storage.thongKe.demTheoLoai("Du lieu khong hop le");
        j["demLoiTrangThai"] = storage.thongKe.demTheoLoai("Loi trang thai nhan vien");

        jsonResponse(res, 200, true, j);
    });

    svr.Get("/api/stats/report-raw", [&storage](const httplib::Request&, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        // Dùng std::cout.rdbuf() redirect để chạy trực tiếp ThongKe::baoCaoTongHop của C++!
        std::string rawReport = JsonAdapter::captureCout([&]() {
            storage.thongKe.baoCaoTongHop(
                storage.dsPhongBan,
                storage.dsHopDong,
                storage.dsChamCong,
                storage.dsKPI,
                storage.dsBangLuong
            );
        });

        jsonResponse(res, 200, true, {{"report", rawReport}});
    });

    svr.Get("/api/stats/errors", [&storage](const httplib::Request&, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        json j;
        j["tongSoLoi"] = storage.thongKe.tongSoLoi();
        std::string errText = JsonAdapter::captureCout([&]() {
            storage.thongKe.thongKeLoi(20);
        });
        j["nhatKyLoi"] = errText;
        jsonResponse(res, 200, true, j);
    });

    // ============================================================
    // 8. API QUẢN LÝ DỮ LIỆU (Reset sample data, CSV/JSON)
    // ============================================================
    svr.Post("/api/data/reset-sample", [&storage](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        storage.initSampleData();
        storage.saveAll();
        jsonResponse(res, 200, true, nullptr, "Đã khôi phục dữ liệu mẫu gốc từ main_test.cpp thành công");
    });

    svr.Get("/api/data/export-json", [&storage](const httplib::Request&, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        json root;
        json empArr = json::array();
        for (const auto& nv : storage.dsNhanVien) {
            empArr.push_back(JsonAdapter::nhanVienToJson(nv, storage.getMaPhongBanCuaNV(nv->getMaNV()), storage.getPhongBanCuaNV(nv->getMaNV())));
        }
        root["employees"] = empArr;

        json deptArr = json::array();
        for (const auto& pb : storage.dsPhongBan) {
            deptArr.push_back(JsonAdapter::phongBanToJson(pb));
        }
        root["departments"] = deptArr;

        json hdArr = json::array();
        for (const auto& hd : storage.dsHopDong) {
            root["contracts"].push_back(JsonAdapter::hopDongToJson(hd, storage.hopDongHetHanMap[hd.getMaHopDong()]));
        }

        json ccArr = json::array();
        for (const auto& cc : storage.dsChamCong) {
            auto nv = storage.timNhanVien(cc.getMaNhanVien());
            ccArr.push_back(JsonAdapter::chamCongToJson(cc, nv ? nv->getHoTen() : ""));
        }
        root["attendance"] = ccArr;

        json kpiArr = json::array();
        for (const auto& kpi : storage.dsKPI) {
            auto nv = storage.timNhanVien(kpi.getMaNV());
            kpiArr.push_back(JsonAdapter::kpiToJson(kpi, nv ? nv->getHoTen() : "", nv ? nv->getLoaiNhanVien() : ""));
        }
        root["kpi"] = kpiArr;

        json blArr = json::array();
        for (const auto& bl : storage.dsBangLuong) {
            blArr.push_back(JsonAdapter::bangLuongToJson(bl));
        }
        root["payroll"] = blArr;

        res.set_content(root.dump(2), "application/json; charset=utf-8");
        res.set_header("Content-Disposition", "attachment; filename=\"quan_ly_nhan_su_backup.json\"");
    });

    svr.Post("/api/data/import-json", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        try {
            json root = json::parse(req.body);
            if (root.contains("employees") && root["employees"].is_array()) {
                storage.dsNhanVien.clear();
                for (const auto& item : root["employees"]) {
                    auto nv = JsonAdapter::jsonToNhanVien(item);
                    if (nv) storage.dsNhanVien.push_back(nv);
                }
            }
            if (root.contains("departments") && root["departments"].is_array()) {
                storage.dsPhongBan.clear();
                for (const auto& item : root["departments"]) {
                    PhongBan pb(item.value("maPhongBan", ""), item.value("tenPhongBan", ""));
                    if (item.contains("danhSachNhanVien") && item["danhSachNhanVien"].is_array()) {
                        for (const auto& mem : item["danhSachNhanVien"]) {
                            auto nv = storage.timNhanVien(mem.value("maNV", ""));
                            if (nv) pb.themNhanVien(nv);
                        }
                    }
                    storage.dsPhongBan.push_back(pb);
                }
            }
            storage.saveAll();
            jsonResponse(res, 200, true, nullptr, "Nhập dữ liệu JSON thành công");
        } catch (const std::exception& e) {
            errorResponse(res, 400, std::string("Lỗi nhập dữ liệu: ") + e.what());
        }
    });

    svr.Get("/api/data/export-csv", [&storage](const httplib::Request& req, httplib::Response& res) {

        std::lock_guard<std::recursive_mutex> lock(g_apiMutex);
        std::string type = req.get_param_value("type");
        std::ostringstream csv;
        // UTF-8 BOM cho Excel
        csv << "\xEF\xBB\xBF";

        if (type == "employees") {
            csv << "Mã NV,Họ tên,Ngày sinh,Số điện thoại,Email,Trạng thái,Loại nhân viên,Phòng ban\n";
            for (const auto& nv : storage.dsNhanVien) {
                if (!nv) continue;
                csv << nv->getMaNV() << ",\""
                    << nv->getHoTen() << "\","
                    << nv->getNgaySinh() << ","
                    << nv->getSoDienThoai() << ","
                    << nv->getEmail() << ",\""
                    << nv->getTrangThai() << "\",\""
                    << nv->getLoaiNhanVien() << "\",\""
                    << storage.getPhongBanCuaNV(nv->getMaNV()) << "\"\n";
            }
            res.set_header("Content-Disposition", "attachment; filename=\"danh_sach_nhan_vien.csv\"");
        } else if (type == "payroll") {
            csv << "Mã NV,Họ tên,Loại NV,Kỳ lương,Lương cơ bản,Phụ cấp,Thưởng KPI,Khoản bổ sung,Tổng thu nhập,Bảo hiểm,Thuế,Thực lĩnh\n";
            for (const auto& bl : storage.dsBangLuong) {
                csv << bl.getMaNV() << ",\""
                    << bl.getHoTen() << "\",\""
                    << bl.getLoaiNhanVien() << "\","
                    << bl.getThang() << "/" << bl.getNam() << ","
                    << (long long)bl.getLuongCoBan() << ","
                    << (long long)bl.getPhuCap() << ","
                    << (long long)bl.getThuongKPI() << ","
                    << (long long)bl.getKhoanBoSung() << ","
                    << (long long)bl.tongThuNhap() << ","
                    << (long long)bl.getBaoHiem() << ","
                    << (long long)bl.getThue() << ","
                    << (long long)bl.luongThucLinh() << "\n";
            }
            res.set_header("Content-Disposition", "attachment; filename=\"bang_luong.csv\"");
        } else {
            csv << "Khong ro dinh dang\n";
        }
        res.set_content(csv.str(), "text/csv; charset=utf-8");
    });

    // ============================================================
    // PHỤC VỤ STATIC FILES FRONTEND
    // ============================================================
    svr.set_mount_point("/", publicDir.c_str());

    std::cout << "\n=======================================================\n";
    std::cout << "  HE THONG QUAN LY NHAN SU - C++ REST SERVER\n";
    std::cout << "  May chu dang chay tai: http://localhost:" << port << "\n";
    std::cout << "  Thu muc frontend public: " << publicDir << "\n";
    std::cout << "  Thu muc du lieu data: " << dataDir << "\n";
    std::cout << "=======================================================\n\n";

    svr.listen("0.0.0.0", port);
    return 0;
}
