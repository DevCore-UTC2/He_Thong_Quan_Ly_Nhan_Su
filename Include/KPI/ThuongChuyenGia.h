#ifndef THUONGCHUYENGIA_H
#define THUONGCHUYENGIA_H

#include "IChinhSachThuong.h"

// Lớp con thưởng cho Chuyên gia
class ThuongChuyenGia : public IChinhSachThuong {
public:
    ThuongChuyenGia();
    double tinhThuong(double diemKPI) const override;
};

#endif
