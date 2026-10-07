#ifndef ICHINHSACHTHUONG_H
#define ICHINHSACHTHUONG_H

// Interface cho chính sách thưởng (Strategy Pattern)
class IChinhSachThuong {
public:
    virtual ~IChinhSachThuong();
    virtual double tinhThuong(double diemKPI) const = 0;
};

#endif
