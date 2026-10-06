#ifndef PHONGBAN_H
#define PHONGBAN_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../NhanVien/NhanVien.h"

using namespace std;

// Phòng ban: tập hợp (aggregation) nhiều nhân viên thuộc nhiều ngạch khác nhau.
// - Lưu vector<shared_ptr<NhanVien>> để giữ tính đa hình, tránh object slicing.
// - Dùng shared_ptr vì một nhân viên còn được HopDongLaoDong (và module sau) tham chiếu.
class PhongBan
{
private:
    string maPhongBan;
    string tenPhongBan;
    vector<shared_ptr<NhanVien>> danhSachNhanVien;

public:
    //Constructor mặc định (không cần destructor tự viết: vector + shared_ptr tự dọn)
    PhongBan();
    //Constructor có tham số
    PhongBan(string maPhongBan, string tenPhongBan);

    // Nhập, xuất, cập nhật thông tin phòng ban (không đụng tới danh sách nhân viên)
    void nhap();
    void xuat() const;
    void capNhatThongTin();

    // Lấy thông tin phòng ban
    string getMaPhongBan() const;
    string getTenPhongBan() const;
    int getSoLuongNhanVien() const;

    //Thiết lập cụm hàm set
    void setMaPhongBan(string maPhongBan);
    void setTenPhongBan(string tenPhongBan);

    // Quản lý nhân sự trong phòng ban
    bool themNhanVien(shared_ptr<NhanVien> nv);       // false nếu nullptr, thiếu mã hoặc trùng mã
    bool xoaNhanVien(const string& maNV);             // chỉ gỡ khỏi phòng ban, không hủy nhân viên
    bool coNhanVien(const string& maNV) const;
    shared_ptr<NhanVien> timNhanVien(const string& maNV) const; // nullptr nếu không thấy

    // API cho Người 3 (KPI) và Người 4 (Lương)
    // Trả về tham chiếu hằng: không sao chép mảng và không thể thêm/xóa phần tử từ bên ngoài.
    const vector<shared_ptr<NhanVien>>& getDanhSachNhanVien() const;

    // Lấy các nhân viên thuộc một ngạch cụ thể, đã ép kiểu sẵn (downcast an toàn).
    // Ví dụ: auto ds = pb.layTheoKieu<NhanVienBanThoiGian>();  // phải include header của lớp đó
    template <typename T>
    vector<shared_ptr<T>> layTheoKieu() const
    {
        vector<shared_ptr<T>> ketQua;
        for (const auto& nv : danhSachNhanVien)
        {
            shared_ptr<T> p = dynamic_pointer_cast<T>(nv);
            if (p != nullptr)
            {
                ketQua.push_back(p);
            }
        }
        return ketQua;
    }

    //Kiểm tra hợp lệ thông tin phòng ban
    bool kiemTraHopLe() const;
};

#endif
