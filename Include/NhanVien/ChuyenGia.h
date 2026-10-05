#ifndef CHUYENGIA_H
#define CHUYENGIA_H

#include "NhanVien.h"
// Lớp con ChuyenGia kế thừa từ lớp cha NhanVien
class ChuyenGia : public NhanVien
{
protected:
    string chuyenMon;
    int soNamKinhNghiem;

public:
    ChuyenGia();

    void nhap() override;
    void xuat() const override;
    void capNhatThongTin() override;

    // Lấy thông tin chuyên gia
    string getChuyenMon() const;
    int getSoNamKinhNghiem() const;

    //Thiết lập cụm hàm set
    void setChuyenMon(string chuyenMon);
    void setSoNamKinhNghiem(int soNamKinhNghiem);
    
    bool kiemTraThongTinRieng() const;

    string getLoaiNhanVien() const override;
};

#endif