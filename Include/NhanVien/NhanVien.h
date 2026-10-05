#ifndef NHANVIEN_H
#define NHANVIEN_H

#include <iostream>
#include <string>
using namespace std;
//Khai báo Class cha 
class NhanVien
{
    //Dùng protected để các lớp con có thể truy cập(kế thừa) được
    protected:
        string maNV;
        string hoTen;
        string ngaySinh;
        string soDienThoai;
        string email;
    public:
        //Khai báo contructor và destructor ảo
        NhanVien();
        virtual ~NhanVien();

        //Nhập ảo và xuât ảo ctrinh
        virtual void nhap();
        virtual void xuat() const;

        //Lấy thông tin nhân viên để hỗ trợ các module khác
        string getMaNV() const;
        string getHoTen() const;
        string getNgaySinh() const;
        string getSoDienThoai() const;
        string getEmail() const;

        //Cập nhật thông tin nhân viên
        virtual void capNhatThongTin();

        //Kiểm tra hợp lệ
        bool kiemTraHopLe() const;

        //Phân loại nhân viên
        virtual string getLoaiNhanVien() const;

        //Thiết lập cụm hàm set
        void setMaNV(string maNV);
        void setHoTen(string hoTen);
        void setNgaySinh(string ngaySinh);
        void setSoDienThoai(string soDienThoai);
        void setEmail(string email);

};
#endif