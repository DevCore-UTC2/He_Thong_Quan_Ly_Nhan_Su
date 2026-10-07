#include "ThuongNhanVien.h"

ThuongNhanVien::ThuongNhanVien() {
}

double ThuongNhanVien::tinhThuong(double diemKPI) const {
    if (diemKPI >= 90) {
        return 2000000;
    } else if (diemKPI >= 70) {
        return 1000000;
    }
    return 0;
}
