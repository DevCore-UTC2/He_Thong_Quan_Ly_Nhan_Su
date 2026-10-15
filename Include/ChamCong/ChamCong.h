#ifndef CHAMCONG_H
#define CHAMCONG_H

#include <iostream>
#include <string>

using namespace std;

// Chấm công: ghi nhận số ngày công và số ngày nghỉ của MỘT nhân viên trong MỘT tháng.
// - Chỉ lưu mã nhân viên (maNhanVien), không lưu cả đối tượng NhanVien nên không phụ thuộc module của Người 1.
// - Module Lương (Người 4) có thể lấy dữ liệu qua getSoNgayCong() và getSoNgayNghi().
// - trangThai có 2 giá trị: "Dang cham cong" (còn được sửa) và "Da chot cong" (không được sửa nữa).
class ChamCong
{
private:
    string maNhanVien;
    int thang;          // 1 -> 12
    int nam;            // VD: 2026
    int soNgayCong;
    int soNgayNghi;
    string trangThai;

    // Số ngày của tháng/năm hiện tại (có xét năm nhuận), trả về 0 nếu tháng sai
    int soNgayTrongThang() const;

public:
    //Constructor mặc định
    ChamCong();
    //Constructor có tham số (trạng thái mặc định là "Dang cham cong")
    ChamCong(string maNhanVien, int thang, int nam, int soNgayCong, int soNgayNghi);

    // Nhập, xuất, cập nhật thông tin chấm công
    void nhap();
    void xuat() const;
    void capNhatThongTin();

    // Cập nhật từng loại, trả về false nếu bị từ chối (đã chốt công hoặc giá trị sai)
    bool capNhatSoNgayCong(int soNgayCongMoi);
    bool capNhatSoNgayNghi(int soNgayNghiMoi);

    // Reset sang tháng mới: ngày công = 0, ngày nghỉ = 0, trạng thái = "Dang cham cong"
    bool resetThang(int thangMoi, int namMoi);

    // Lấy thông tin chấm công
    string getMaNhanVien() const;
    int getThang() const;
    int getNam() const;
    int getSoNgayCong() const;
    int getSoNgayNghi() const;
    string getTrangThai() const;

    //Thiết lập cụm hàm set
    void setMaNhanVien(string maNhanVien);
    void setThang(int thang);
    void setNam(int nam);
    void setTrangThai(string trangThai);

    // Kiểm tra: ngày công, ngày nghỉ >= 0 và tổng không vượt quá số ngày của tháng
    bool kiemTraSoNgayCongHopLe() const;

    // Kiểm tra hợp lệ toàn bộ bản ghi chấm công
    bool kiemTraHopLe() const;
};

#endif
