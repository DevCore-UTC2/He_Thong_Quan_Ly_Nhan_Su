#include "ThuongQuanLy.h"

ThuongQuanLy::ThuongQuanLy() {
}

double ThuongQuanLy::tinhThuong(double diemKPI) const {
    if (diemKPI >= 90) {
        return 10000000;
    } else if (diemKPI >= 70) {
        return 5000000;
    }
    return 0;
}
