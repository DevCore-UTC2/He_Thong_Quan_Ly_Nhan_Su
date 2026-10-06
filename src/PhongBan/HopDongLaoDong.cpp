#include "../../include/PhongBan/HopDongLaoDong.h"
#include <iomanip>
#include <limits>

//Khởi tạo constructor
HopDongLaoDong::HopDongLaoDong()
{
    maHopDong = "";
    loaiHopDong = "";
    ngayKyKet = "";
    luongThoaThuan = 0.0;
    nguoiLaoDong = nullptr;
}

HopDongLaoDong::HopDongLaoDong(string maHopDong, string loaiHopDong, string ngayKyKet,
                               double luongThoaThuan, shared_ptr<NhanVien> nguoiLaoDong)
{
    this->maHopDong = maHopDong;
    this->loaiHopDong = loaiHopDong;
    this->ngayKyKet = ngayKyKet;
    this->luongThoaThuan = (luongThoaThuan >= 0) ? luongThoaThuan : 0.0;
    this->nguoiLaoDong = nguoiLaoDong;
}

//Nhập thông tin hợp đồng
void HopDongLaoDong::nhap()
{
    cout << "Nhap ma hop dong: ";
    getline(cin, maHopDong);

    cout << "Nhap loai hop dong (VD: Thu viec, Chinh thuc, Thoi vu): ";
    getline(cin, loaiHopDong);

    cout << "Nhap ngay ky ket (DD/MM/YYYY): ";
    getline(cin, ngayKyKet);

    // Nhập lại nếu gõ sai kiểu hoặc số âm
    double luong;
    while (true)
    {
        cout << "Nhap muc luong thoa thuan (VND): ";
        if (cin >> luong && luong >= 0)
        {
            luongThoaThuan = luong;
            break;
        }

        if (cin.eof())
        {
            break;
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Luong thoa thuan khong hop le!" << endl;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); //Xóa bộ đệm
}

//Xuất thông tin hợp đồng
void HopDongLaoDong::xuat() const
{
    cout << "\n--- HOP DONG LAO DONG ---" << endl;
    cout << "Ma hop dong: " << maHopDong << endl;
    cout << "Loai hop dong: " << loaiHopDong << endl;
    cout << "Ngay ky ket: " << ngayKyKet << endl;

    // In lương không dùng ký hiệu khoa học, sau đó trả lại định dạng cũ cho cout
    ios_base::fmtflags cu = cout.flags();
    streamsize doChinhXac = cout.precision();
    cout << fixed << setprecision(0);
    cout << "Luong thoa thuan: " << luongThoaThuan << " VND" << endl;
    cout.flags(cu);
    cout.precision(doChinhXac);

    cout << "Nguoi lao dong:" << endl;
    if (nguoiLaoDong != nullptr)
    {
        // Đọc trực tiếp từ nhân viên nên luôn là thông tin mới nhất
        cout << "  - Ma NV: " << nguoiLaoDong->getMaNV() << endl;
        cout << "  - Ho ten: " << nguoiLaoDong->getHoTen() << endl;
        cout << "  - Loai: " << nguoiLaoDong->getLoaiNhanVien() << endl;
    }
    else
    {
        cout << "  - [Hop dong chua duoc gan cho nhan vien nao]" << endl;
    }
}

//Cập nhật thông tin hợp đồng
void HopDongLaoDong::capNhatThongTin()
{
    int luaChon;

    do
    {
        cout << "\n===== CAP NHAT THONG TIN HOP DONG =====" << endl;
        cout << "1. Cap nhat loai hop dong" << endl;
        cout << "2. Cap nhat ngay ky ket" << endl;
        cout << "3. Cap nhat luong thoa thuan" << endl;
        cout << "0. Thoat" << endl;
        cout << "Lua chon: ";
        cin >> luaChon;
        cin.ignore();

        switch (luaChon)
        {
            case 1:
            {
                string loaiMoi;

                cout << "Nhap loai hop dong moi: ";
                getline(cin, loaiMoi);

                setLoaiHopDong(loaiMoi);
                break;
            }

            case 2:
            {
                string ngayMoi;

                cout << "Nhap ngay ky ket moi (DD/MM/YYYY): ";
                getline(cin, ngayMoi);

                setNgayKyKet(ngayMoi);
                break;
            }

            case 3:
            {
                double luongMoi;

                cout << "Nhap luong thoa thuan moi: ";
                cin >> luongMoi;
                cin.ignore();

                setLuongThoaThuan(luongMoi);
                break;
            }

            case 0:
                break;

            default:
                cout << "Lua chon khong hop le!" << endl;
        }

    } while (luaChon != 0);
}

//Lấy thông tin hợp đồng
string HopDongLaoDong::getMaHopDong() const
{
    return maHopDong;
}

string HopDongLaoDong::getLoaiHopDong() const
{
    return loaiHopDong;
}

string HopDongLaoDong::getNgayKyKet() const
{
    return ngayKyKet;
}

double HopDongLaoDong::getLuongThoaThuan() const
{
    return luongThoaThuan;
}

shared_ptr<NhanVien> HopDongLaoDong::getNguoiLaoDong() const
{
    return nguoiLaoDong;
}

//Thiết lập cụm hàm set kiểm soát thông tin hợp đồng
void HopDongLaoDong::setMaHopDong(string maHopDong)
{
    if (!maHopDong.empty())
    {
        this->maHopDong = maHopDong;
    }
    else
    {
        cout << "Ma hop dong khong duoc de trong!" << endl;
    }
}

void HopDongLaoDong::setLoaiHopDong(string loaiHopDong)
{
    if (!loaiHopDong.empty())
    {
        this->loaiHopDong = loaiHopDong;
    }
    else
    {
        cout << "Loai hop dong khong duoc de trong!" << endl;
    }
}

void HopDongLaoDong::setNgayKyKet(string ngayKyKet)
{
    if (!ngayKyKet.empty())
    {
        this->ngayKyKet = ngayKyKet;
    }
    else
    {
        cout << "Ngay ky ket khong duoc de trong!" << endl;
    }
}

void HopDongLaoDong::setLuongThoaThuan(double luongThoaThuan)
{
    if (luongThoaThuan >= 0)
    {
        this->luongThoaThuan = luongThoaThuan;
    }
    else
    {
        cout << "Luong thoa thuan khong hop le!" << endl;
    }
}

void HopDongLaoDong::setNguoiLaoDong(shared_ptr<NhanVien> nv)
{
    if (nv != nullptr)
    {
        this->nguoiLaoDong = nv;
    }
    else
    {
        cout << "Loi: Khong the gan hop dong cho nhan vien khong ton tai!" << endl;
    }
}

//Kiểm tra hợp lệ hợp đồng
bool HopDongLaoDong::kiemTraHopLe() const
{
    if (maHopDong.empty() || loaiHopDong.empty() || ngayKyKet.empty())
        return false;

    if (luongThoaThuan < 0)
        return false;

    if (nguoiLaoDong == nullptr)
        return false;

    return true;
}
