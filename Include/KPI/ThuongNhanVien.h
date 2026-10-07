#ifndef THUONGNHANVIEN_H
#define THUONGNHANVIEN_H

#include "IChinhSachThuong.h"

// Lớp con thưởng cho Nhân viên
class ThuongNhanVien : public IChinhSachThuong {
public:
    ThuongNhanVien();
    double tinhThuong(double diemKPI) const override;
};

#endif
