#include "ChuyenGia.h"
//Khởi tạo contructor
ChuyenGia::ChuyenGia()
{
    chuyenMon = "";
    soNamKinhNghiem = 0;
}

//Nhập thông tin chuyên gia
void ChuyenGia::nhap()
{
    NhanVien::nhap();

    cout << "Nhap chuyen mon: ";
    getline(cin, chuyenMon);

    cout << "Nhap so nam kinh nghiem: ";
    cin >> soNamKinhNghiem;

    cin.ignore();
}

//Xuat thông tin chuyên gia
void ChuyenGia::xuat() const
{
    NhanVien::xuat();

    cout << "Chuyen mon: " << chuyenMon << endl;
    cout << "So nam kinh nghiem: " << soNamKinhNghiem << endl;
}

void ChuyenGia::capNhatThongTin()
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
                    cout << "1. Chuyen mon" << endl;
                    cout << "2. So nam kinh nghiem" << endl;
                    cout << "0. Thoat" << endl;
                    cout << "Lua chon: ";
                    cin >> chon;
                    cin.ignore();

                    switch (chon)
                    {
                        case 1:
                        {
                            string chuyenMonMoi;

                            cout << "Nhap chuyen mon moi: ";
                            getline(cin, chuyenMonMoi);

                            setChuyenMon(chuyenMonMoi);
                            break;
                        }

                        case 2:
                        {
                            int soNamMoi;

                            cout << "Nhap so nam kinh nghiem moi: ";
                            cin >> soNamMoi;
                            cin.ignore();

                            setSoNamKinhNghiem(soNamMoi);
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

//Lấy thông tin chuyên gia
string ChuyenGia::getChuyenMon() const
{
    return chuyenMon;
}

int ChuyenGia::getSoNamKinhNghiem() const
{
    return soNamKinhNghiem;
}

//Thiết lập cụm hàm set kiểm soát thông tin chuyên gia
void ChuyenGia::setChuyenMon(string chuyenMon)
{
    if (!chuyenMon.empty())
    {
        this->chuyenMon = chuyenMon;
    }
    else
    {
        cout << "Chuyen mon khong duoc de trong!" << endl;
    }
}

void ChuyenGia::setSoNamKinhNghiem(int soNamKinhNghiem)
{
    if (soNamKinhNghiem >= 0)
    {
        this->soNamKinhNghiem = soNamKinhNghiem;
    }
    else
    {
        cout << "So nam kinh nghiem khong hop le!" << endl;
    }
}

bool ChuyenGia::kiemTraThongTinRieng() const
{
    if (chuyenMon.empty())
        return false;

    if (soNamKinhNghiem < 0)
        return false;

    return true;
}

string ChuyenGia::getLoaiNhanVien() const
{
    return "Chuyen gia";
}