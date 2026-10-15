#include "../../Include/ChamCong/ChamCong.h"
#include <limits>

// Hàm hỗ trợ nội bộ đọc 1 số nguyên, nhập sai thì bắt nhập lại
static int docSoNguyen()
{
    int x;
    while (!(cin >> x))
    {
        if (cin.eof())
        {
            return 0;
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Nhap sai. Vui long nhap so: ";
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); //Xóa bộ đệm
    return x;
}

//Khởi tạo constructor
ChamCong::ChamCong()
{
    maNhanVien = "";
    thang = 1;
    nam = 2026;
    soNgayCong = 0;
    soNgayNghi = 0;
    trangThai = "Dang cham cong";
}

ChamCong::ChamCong(string maNhanVien, int thang, int nam, int soNgayCong, int soNgayNghi)
{
    this->maNhanVien = maNhanVien;
    this->thang = thang;
    this->nam = nam;
    this->soNgayCong = (soNgayCong >= 0) ? soNgayCong : 0;
    this->soNgayNghi = (soNgayNghi >= 0) ? soNgayNghi : 0;
    this->trangThai = "Dang cham cong";
}

//Tính số ngày trong tháng (có xét năm nhuận)
int ChamCong::soNgayTrongThang() const
{
    if (thang < 1 || thang > 12)
    {
        return 0;
    }

    if (thang == 4 || thang == 6 || thang == 9 || thang == 11)
    {
        return 30;
    }

    if (thang == 2)
    {
        bool namNhuan = (nam % 400 == 0) || (nam % 4 == 0 && nam % 100 != 0);
        return namNhuan ? 29 : 28;
    }

    return 31;
}

//Nhập thông tin chấm công
void ChamCong::nhap()
{
    // Mã nhân viên không được để trống
    while (true)
    {
        cout << "Nhap ma nhan vien: ";
        getline(cin, maNhanVien);

        if (!maNhanVien.empty() || cin.eof())
        {
            break;
        }
        cout << "Ma nhan vien khong duoc de trong!" << endl;
    }

    // Tháng 1 -> 12
    while (true)
    {
        cout << "Nhap thang (1-12): ";
        thang = docSoNguyen();

        if ((thang >= 1 && thang <= 12) || cin.eof())
        {
            break;
        }
        cout << "Thang khong hop le!" << endl;
    }

    // Năm
    while (true)
    {
        cout << "Nhap nam: ";
        nam = docSoNguyen();

        if ((nam >= 2000 && nam <= 2100) || cin.eof())
        {
            break;
        }
        cout << "Nam khong hop le (2000 - 2100)!" << endl;
    }

    // Ngày công + ngày nghỉ: nhập cả hai rồi mới kiểm tra tổng
    while (true)
    {
        cout << "Nhap so ngay cong: ";
        soNgayCong = docSoNguyen();

        cout << "Nhap so ngay nghi: ";
        soNgayNghi = docSoNguyen();

        if (kiemTraSoNgayCongHopLe() || cin.eof())
        {
            break;
        }
        cout << "So ngay cong/nghi khong hop le! Tong khong duoc am va khong vuot qua "
             << soNgayTrongThang() << " ngay cua thang. Vui long nhap lai." << endl;
    }

    trangThai = "Dang cham cong";
}

//Xuất thông tin chấm công
void ChamCong::xuat() const
{
    cout << "Nhan vien " << maNhanVien << endl;
    cout << "Thang " << thang << "/" << nam << endl;
    cout << "Ngay cong: " << soNgayCong << endl;
    cout << "Ngay nghi: " << soNgayNghi << endl;
    cout << "Trang thai: " << trangThai << endl;
}

//Cập nhật số ngày công
bool ChamCong::capNhatSoNgayCong(int soNgayCongMoi)
{
    if (trangThai == "Da chot cong")
    {
        cout << "Cong thang nay da chot, khong the sua!" << endl;
        return false;
    }

    if (soNgayCongMoi < 0 || soNgayCongMoi + soNgayNghi > soNgayTrongThang())
    {
        cout << "So ngay cong khong hop le! (khong am, cong + nghi toi da "
             << soNgayTrongThang() << " ngay)" << endl;
        return false;
    }

    soNgayCong = soNgayCongMoi;
    cout << "Cap nhat so ngay cong thanh cong!" << endl;
    return true;
}

//Cập nhật số ngày nghỉ
bool ChamCong::capNhatSoNgayNghi(int soNgayNghiMoi)
{
    if (trangThai == "Da chot cong")
    {
        cout << "Cong thang nay da chot, khong the sua!" << endl;
        return false;
    }

    if (soNgayNghiMoi < 0 || soNgayCong + soNgayNghiMoi > soNgayTrongThang())
    {
        cout << "So ngay nghi khong hop le! (khong am, cong + nghi toi da "
             << soNgayTrongThang() << " ngay)" << endl;
        return false;
    }

    soNgayNghi = soNgayNghiMoi;
    cout << "Cap nhat so ngay nghi thanh cong!" << endl;
    return true;
}

//Reset chấm công sang tháng mới
bool ChamCong::resetThang(int thangMoi, int namMoi)
{
    if (thangMoi < 1 || thangMoi > 12)
    {
        cout << "Thang khong hop le!" << endl;
        return false;
    }

    if (namMoi < 2000 || namMoi > 2100)
    {
        cout << "Nam khong hop le (2000 - 2100)!" << endl;
        return false;
    }

    thang = thangMoi;
    nam = namMoi;
    soNgayCong = 0;
    soNgayNghi = 0;
    trangThai = "Dang cham cong";
    cout << "Da reset cham cong sang thang " << thang << "/" << nam << "." << endl;
    return true;
}

//Cập nhật thông tin chấm công (menu)
void ChamCong::capNhatThongTin()
{
    int luaChon;

    do
    {
        cout << "\n===== CAP NHAT THONG TIN CHAM CONG =====" << endl;
        cout << "1. Cap nhat so ngay cong" << endl;
        cout << "2. Cap nhat so ngay nghi" << endl;
        cout << "3. Cap nhat trang thai" << endl;
        cout << "4. Reset sang thang moi" << endl;
        cout << "0. Thoat" << endl;
        cout << "Lua chon: ";
        luaChon = docSoNguyen();

        switch (luaChon)
        {
            case 1:
            {
                cout << "Nhap so ngay cong moi: ";
                int congMoi = docSoNguyen();
                capNhatSoNgayCong(congMoi);
                break;
            }

            case 2:
            {
                cout << "Nhap so ngay nghi moi: ";
                int nghiMoi = docSoNguyen();
                capNhatSoNgayNghi(nghiMoi);
                break;
            }

            case 3:
            {
                cout << "1. Dang cham cong" << endl;
                cout << "2. Da chot cong" << endl;
                cout << "Lua chon: ";
                int chon = docSoNguyen();

                if (chon == 1)
                {
                    setTrangThai("Dang cham cong");
                }
                else if (chon == 2)
                {
                    setTrangThai("Da chot cong");
                }
                else
                {
                    cout << "Lua chon khong hop le!" << endl;
                }
                break;
            }

            case 4:
            {
                cout << "Nhap thang moi (1-12): ";
                int thangMoi = docSoNguyen();
                cout << "Nhap nam moi: ";
                int namMoi = docSoNguyen();
                resetThang(thangMoi, namMoi);
                break;
            }

            case 0:
                break;

            default:
                cout << "Lua chon khong hop le!" << endl;
        }

    } while (luaChon != 0 && !cin.eof());
}

//Lấy thông tin chấm công
string ChamCong::getMaNhanVien() const
{
    return maNhanVien;
}

int ChamCong::getThang() const
{
    return thang;
}

int ChamCong::getNam() const
{
    return nam;
}

int ChamCong::getSoNgayCong() const
{
    return soNgayCong;
}

int ChamCong::getSoNgayNghi() const
{
    return soNgayNghi;
}

string ChamCong::getTrangThai() const
{
    return trangThai;
}

//Thiết lập cụm hàm set kiểm soát thông tin chấm công (sai thì giữ nguyên giá trị cũ)
void ChamCong::setMaNhanVien(string maNhanVien)
{
    if (!maNhanVien.empty())
    {
        this->maNhanVien = maNhanVien;
    }
    else
    {
        cout << "Ma nhan vien khong duoc de trong!" << endl;
    }
}

void ChamCong::setThang(int thang)
{
    if (thang >= 1 && thang <= 12)
    {
        this->thang = thang;
    }
    else
    {
        cout << "Thang khong hop le!" << endl;
    }
}

void ChamCong::setNam(int nam)
{
    if (nam >= 2000 && nam <= 2100)
    {
        this->nam = nam;
    }
    else
    {
        cout << "Nam khong hop le (2000 - 2100)!" << endl;
    }
}

void ChamCong::setTrangThai(string trangThai)
{
    if (trangThai == "Dang cham cong" || trangThai == "Da chot cong")
    {
        this->trangThai = trangThai;
        cout << "Cap nhat trang thai thanh cong!" << endl;
    }
    else
    {
        cout << "Trang thai khong hop le!" << endl;
    }
}

//Kiểm tra số ngày công hợp lệ
bool ChamCong::kiemTraSoNgayCongHopLe() const
{
    if (soNgayCong < 0 || soNgayNghi < 0)
        return false;

    if (soNgayCong + soNgayNghi > soNgayTrongThang())
        return false;

    return true;
}

//Kiểm tra hợp lệ toàn bộ chấm công
bool ChamCong::kiemTraHopLe() const
{
    if (maNhanVien.empty())
        return false;

    if (thang < 1 || thang > 12 || nam < 2000 || nam > 2100)
        return false;

    if (trangThai != "Dang cham cong" && trangThai != "Da chot cong")
        return false;

    return kiemTraSoNgayCongHopLe();
}
