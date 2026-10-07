#include "HeThongKPI.h"

// contructor
HeThongKPI::HeThongKPI() {
    maNV = "";
    diemKPI = 0;
    thang = "";
    nam = "";
    chinhSachThuong = nullptr;
}

HeThongKPI::HeThongKPI(string maNV, double diemKPI, string thang, string nam) {
    this->maNV = maNV;
    this->diemKPI = diemKPI;
    this->thang = thang;
    this->nam = nam;
    this->chinhSachThuong = nullptr;
}

// destructor
HeThongKPI::~HeThongKPI() {
}

// Nhap
void HeThongKPI::nhap() {
    cout << "Nhap ma nhan vien: ";
    getline(cin, maNV);
    
    cout << "Nhap diem KPI: ";
    cin >> diemKPI;
    cin.ignore();
    
    cout << "Nhap thang: ";
    getline(cin, thang);
    
    cout << "Nhap nam: ";
    getline(cin, nam);
}

// Xuat
void HeThongKPI::xuat() const {
    cout << "Ma nhan vien: " << maNV << endl;
    cout << "Diem KPI: " << diemKPI << endl;
    cout << "Thoi gian: " << thang << "/" << nam << endl;
    if (chinhSachThuong != nullptr) {
        cout << "Tien thuong: " << (long long)tinhTienThuong() << " VND" << endl;
    }
}

// Cụm hàm lấy thông tin
string HeThongKPI::getMaNV() const {
    return maNV;
}

double HeThongKPI::getDiemKPI() const {
    return diemKPI;
}

string HeThongKPI::getThang() const {
    return thang;
}

string HeThongKPI::getNam() const {
    return nam;
}

// Function cập nhật thông tin
void HeThongKPI::capNhatThongTin() {
    int luaChon;
    do {
        cout << "\n======= Cap nhat thong tin KPI =======\n";
        cout << "1. Cap nhat diem KPI\n";
        cout << "2. Cap nhat thang\n";
        cout << "3. Cap nhat nam\n";
        cout << "0. Thoat\n";
        cout << "Nhap lua chon: ";
        cin >> luaChon;
        cin.ignore();
        
        switch (luaChon) {
            case 1:
                cout << "Nhap diem KPI moi: ";
                cin >> diemKPI;
                cin.ignore();
                cout << "Cap nhat diem KPI thanh cong!\n";
                break;
            case 2:
                cout << "Nhap thang moi: ";
                getline(cin, thang);
                cout << "Cap nhat thang thanh cong!\n";
                break;
            case 3:
                cout << "Nhap nam moi: ";
                getline(cin, nam);
                cout << "Cap nhat nam thanh cong!\n";
                break;
            case 0:
                cout << "Thoat cap nhat thong tin KPI.\n";
                break;
            default:
                cout << "Lua chon khong hop le. Vui long chon lai.\n";
        }
    } while (luaChon != 0);
}

// Hàm kiểm tra hợp lệ
bool HeThongKPI::kiemTraHopLe() const {
    if (maNV.empty() || thang.empty() || nam.empty() || diemKPI < 0) {
        return false;
    }
    return true;
}

// Cụm hàm set
void HeThongKPI::setMaNV(string maNV) {
    this->maNV = maNV;
}

void HeThongKPI::setDiemKPI(double diemKPI) {
    this->diemKPI = diemKPI;
}

void HeThongKPI::setThang(string thang) {
    this->thang = thang;
}

void HeThongKPI::setNam(string nam) {
    this->nam = nam;
}

// Strategy Pattern
void HeThongKPI::setChinhSachThuong(IChinhSachThuong* chinhSach) {
    this->chinhSachThuong = chinhSach;
}

double HeThongKPI::tinhTienThuong() const {
    if (chinhSachThuong != nullptr) {
        return chinhSachThuong->tinhThuong(diemKPI);
    }
    return 0;
}
