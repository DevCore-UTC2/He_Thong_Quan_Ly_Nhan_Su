#ifndef NHANVIENTOANTHOIGIAN_H
#define NHANVIENTOANTHOIGIAN_H

#include "NhanVien.h"

// Lớp con NhanVienToanThoiGian kế thừa từ lớp cha NhanVien
class NhanVienToanThoiGian : public NhanVien
{
protected:
    int soNgayCong;
    float soGioLam;
    string caLamViec;

public:
    //Contructor và không có destructor ảo vì lớp cha đã có destructor ảo
    NhanVienToanThoiGian();

    // Nhập và xuất thông tin riêng khi kế thừa lớp cha
    void nhap() override;
    void xuat() const override;

    // Lấy thông tin nhân viên toàn thời gian
    int getSoNgayCong() const;
    float getSoGioLam() const;
    string getCaLamViec() const;

    // Kế thừa Cập nhật thông tin nhân viên toàn thời gian
    void capNhatThongTin() override;

    //Thiết lập cụm hàm set
    void setSoNgayCong(int soNgayCong);
    void setSoGioLam(float soGioLam);
    void setCaLamViec(string caLamViec);

    //Kiểm tra hợp lệ thông tin riêng
    bool kiemTraThongTinRieng() const;

    //Phân loại nhân viên
    string getLoaiNhanVien() const override;
};
#endif
