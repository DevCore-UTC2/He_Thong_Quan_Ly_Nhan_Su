#ifndef HOPDONGLAODONG_H
#define HOPDONGLAODONG_H

#include <iostream>
#include <string>
#include <memory>
#include "../NhanVien/NhanVien.h"

using namespace std;

// Hợp đồng lao động: liên kết (association) tới đúng một nhân viên.
// Không sao chép họ tên/email của nhân viên, chỉ giữ shared_ptr nên luôn thấy dữ liệu mới nhất.
class HopDongLaoDong
{
private:
    string maHopDong;
    string loaiHopDong;      // VD: Thu viec, Chinh thuc, Thoi vu
    string ngayKyKet;        // DD/MM/YYYY
    double luongThoaThuan;   // Lương thỏa thuận cơ bản (VND), đầu vào cho Người 4
    shared_ptr<NhanVien> nguoiLaoDong;

public:
    //Constructor
    HopDongLaoDong();
    HopDongLaoDong(string maHopDong, string loaiHopDong, string ngayKyKet,
                   double luongThoaThuan, shared_ptr<NhanVien> nguoiLaoDong);

    // Nhập, xuất, cập nhật. nhap() KHÔNG gán nhân viên, hãy gọi setNguoiLaoDong() sau đó.
    void nhap();
    void xuat() const;
    void capNhatThongTin();

    // Lấy thông tin hợp đồng
    string getMaHopDong() const;
    string getLoaiHopDong() const;
    string getNgayKyKet() const;
    double getLuongThoaThuan() const;
    shared_ptr<NhanVien> getNguoiLaoDong() const;   // nullptr nếu chưa gán

    //Thiết lập cụm hàm set
    void setMaHopDong(string maHopDong);
    void setLoaiHopDong(string loaiHopDong);
    void setNgayKyKet(string ngayKyKet);
    void setLuongThoaThuan(double luongThoaThuan);
    void setNguoiLaoDong(shared_ptr<NhanVien> nv);

    //Kiểm tra hợp lệ: đủ thông tin, lương >= 0 và đã gán nhân viên
    bool kiemTraHopLe() const;
};

#endif
