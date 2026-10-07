#ifndef THUONGQUANLY_H
#define THUONGQUANLY_H

#include "IChinhSachThuong.h"

// Lớp con thưởng cho Quản lý
class ThuongQuanLy : public IChinhSachThuong {
public:
    ThuongQuanLy();
    double tinhThuong(double diemKPI) const override;
};

#endif
