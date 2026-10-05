#ifndef NHANVIENBANTHOIGIAN_H
#define NHANVIENBANTHOIGIAN_H

#include "NhanVien.h"
// Lớp con NhanVienBanThoiGian kế thừa từ lớp cha NhanVien
class NhanVienBanThoiGian : public NhanVien
{
    protected:
        float soGioLam;
        float luongTheoGio;
    public:
        // Lớp con NhanVienBanThoiGian kế thừa từ lớp cha NhanVien
        NhanVienBanThoiGian();

        void nhap() override;
        void xuat() const override;

        // Lấy thông tin nhân viên bán thời gian
        float getSoGioLam() const;
        float getLuongTheoGio() const;

        // Cập nhật thông tin nhân viên bán thời gian
        void capNhatThongTin() override;

        //Thiết lập cụm hàm set
        void setSoGioLam(float soGioLam);
        void setLuongTheoGio(float luongTheoGio);

        //Kiểm tra hợp lệ thông tin riêng
        bool kiemTraThongTinRieng() const;

        //Phân loại nhân viên
        string getLoaiNhanVien() const override;
};

#endif