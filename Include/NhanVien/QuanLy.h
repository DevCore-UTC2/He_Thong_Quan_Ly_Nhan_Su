#ifndef QUANLY_H
#define QUANLY_H

#include "NhanVien.h"

class QuanLy : public NhanVien
{
protected:
    string chucVu;
    string capQuanLy;
    string thongTinDacThu;

public:
    //Contructor và không có destructor ảo vì lớp cha đã có destructor ảo   
    QuanLy();

    // Nhập và xuất thông tin riêng khi kế thừa lớp cha
    void nhap() override;
    void xuat() const override;
    void capNhatThongTin() override;

    // Lấy thông tin quản lý
    string getChucVu() const;
    string getCapQuanLy() const;
    string getThongTinDacThu() const;

    //Thiết lập cụm hàm set
    void setChucVu(string chucVu);
    void setCapQuanLy(string capQuanLy);
    void setThongTinDacThu(string thongTinDacThu);

    bool kiemTraThongTinRieng() const;

    string getLoaiNhanVien() const override;
};

#endif