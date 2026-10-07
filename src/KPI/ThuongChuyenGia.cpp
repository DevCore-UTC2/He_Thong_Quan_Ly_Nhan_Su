#include "ThuongChuyenGia.h"

ThuongChuyenGia::ThuongChuyenGia() {
}

double ThuongChuyenGia::tinhThuong(double diemKPI) const {
    if (diemKPI >= 90) {
        return 5000000;
    } else if (diemKPI >= 70) {
        return 2500000;
    }
    return 0;
}
