#include "NhanVienBanThoiGian.h"

NhanVienBanThoiGian::NhanVienBanThoiGian()
{
    soGioLam = 0;
    luongTheoGio = 0;
}

void NhanVienBanThoiGian::nhap()
{
    NhanVien::nhap();

    cout << "Nhap so gio lam: ";
    cin >> soGioLam;

    cout << "Nhap luong theo gio: ";
    cin >> luongTheoGio;

    cin.ignore();
}

void NhanVienBanThoiGian::xuat() const
{
    NhanVien::xuat();

    cout << "So gio lam: " << soGioLam << endl;
    cout << "Luong theo gio: " << luongTheoGio << endl;
}

float NhanVienBanThoiGian::getSoGioLam() const
{
    return soGioLam;
}

float NhanVienBanThoiGian::getLuongTheoGio() const
{
    return luongTheoGio;
}

//Cập nhật thông tin nhân viên bán thời gian
void NhanVienBanThoiGian::capNhatThongTin()
{
    int luaChon;

    do
    {
        cout << "\n===== CAP NHAT THONG TIN NHAN VIEN =====" << endl;
        cout << "1. Cap nhat thong tin chung" << endl;
        cout << "2. Cap nhat thong tin rieng" << endl;
        cout << "0. Thoat" << endl;
        cout << "Lua chon: ";
        cin >> luaChon;
        cin.ignore();

        switch (luaChon)
        {
            case 1:
            {
                NhanVien::capNhatThongTin();
                break;
            }

            case 2:
            {
                int chon;

                do
                {
                    cout << "\n===== THONG TIN RIENG =====" << endl;
                    cout << "1. So gio lam" << endl;
                    cout << "2. Luong theo gio" << endl;
                    cout << "0. Thoat" << endl;
                    cout << "Lua chon: ";
                    cin >> chon;
                    cin.ignore();

                    switch (chon)
                    {
                        case 1:
                        {
                            float gioLamMoi;

                            cout << "Nhap so gio lam moi: ";
                            cin >> gioLamMoi;
                            cin.ignore();

                            setSoGioLam(gioLamMoi);
                            break;
                        }

                        case 2:
                        {
                            float luongMoi;

                            cout << "Nhap luong theo gio moi: ";
                            cin >> luongMoi;
                            cin.ignore();

                            setLuongTheoGio(luongMoi);
                            break;
                        }

                        case 0:
                            break;

                        default:
                            cout << "Lua chon khong hop le!" << endl;
                    }

                } while (chon != 0);

                break;
            }

            case 0:
                break;

            default:
                cout << "Lua chon khong hop le!" << endl;
        }

    } while (luaChon != 0);
}

void NhanVienBanThoiGian::setSoGioLam(float soGioLam)
{
    if (soGioLam >= 0)
    {
        this->soGioLam = soGioLam;
    }
    else
    {
        cout << "So gio lam khong hop le!" << endl;
    }
}

void NhanVienBanThoiGian::setLuongTheoGio(float luongTheoGio)
{
    if (luongTheoGio >= 0)
    {
        this->luongTheoGio = luongTheoGio;
    }
    else
    {
        cout << "Luong theo gio khong hop le!" << endl;
    }
}

bool NhanVienBanThoiGian::kiemTraThongTinRieng() const
{
    if (soGioLam < 0)
        return false;

    if (luongTheoGio < 0)
        return false;

    return true;
}

string NhanVienBanThoiGian::getLoaiNhanVien() const
{
    return "Nhan vien ban thoi gian";
}
