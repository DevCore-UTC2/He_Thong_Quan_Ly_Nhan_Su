#include "QuanLy.h"

//Khởi tạo contructor
QuanLy::QuanLy()
{
    chucVu = "";
    capQuanLy = "";
    thongTinDacThu = "";
}


//Nhập thông tin quản lý
void QuanLy::nhap()
{
    NhanVien::nhap();

    cout << "Nhap chuc vu: ";
    getline(cin, chucVu);

    cout << "Nhap cap quan ly: ";
    getline(cin, capQuanLy);

    cout << "Nhap thong tin dac thu: ";
    getline(cin, thongTinDacThu);
}

//Xuat thông tin quản lý
void QuanLy::xuat() const
{
    NhanVien::xuat();

    cout << "Chuc vu: " << chucVu << endl;
    cout << "Cap quan ly: " << capQuanLy << endl;
    cout << "Thong tin dac thu: " << thongTinDacThu << endl;
}


//Cập nhật thông tin quản lý
void QuanLy::capNhatThongTin()
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
                    cout << "1. Chuc vu" << endl;
                    cout << "2. Cap quan ly" << endl;
                    cout << "3. Thong tin dac thu" << endl;
                    cout << "0. Thoat" << endl;
                    cout << "Lua chon: ";
                    cin >> chon;
                    cin.ignore();

                    switch (chon)
                    {
                        case 1:
                        {
                            string chucVuMoi;

                            cout << "Nhap chuc vu moi: ";
                            getline(cin, chucVuMoi);

                            setChucVu(chucVuMoi);
                            break;
                        }

                        case 2:
                        {
                            string capQuanLyMoi;

                            cout << "Nhap cap quan ly moi: ";
                            getline(cin, capQuanLyMoi);

                            setCapQuanLy(capQuanLyMoi);
                            break;
                        }

                        case 3:
                        {
                            string thongTinMoi;

                            cout << "Nhap thong tin dac thu moi: ";
                            getline(cin, thongTinMoi);

                            setThongTinDacThu(thongTinMoi);
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

//Lấy thông tin quản lý
string QuanLy::getChucVu() const
{
    return chucVu;
}

string QuanLy::getCapQuanLy() const
{
    return capQuanLy;
}

string QuanLy::getThongTinDacThu() const
{
    return thongTinDacThu;
}


//Thiết lập cụm hàm set kiểm soát thông tin quản lý
void QuanLy::setChucVu(string chucVu)
{
    if (!chucVu.empty())
    {
        this->chucVu = chucVu;
    }
    else
    {
        cout << "Chuc vu khong duoc de trong!" << endl;
    }
}

void QuanLy::setCapQuanLy(string capQuanLy)
{
    if (!capQuanLy.empty())
    {
        this->capQuanLy = capQuanLy;
    }
    else
    {
        cout << "Cap quan ly khong duoc de trong!" << endl;
    }
}

void QuanLy::setThongTinDacThu(string thongTinDacThu)
{
    if (!thongTinDacThu.empty())
    {
        this->thongTinDacThu = thongTinDacThu;
    }
    else
    {
        cout << "Thong tin dac thu khong duoc de trong!" << endl;
    }
}

//Kiểm tra hợp lệ thông tin riêng
bool QuanLy::kiemTraThongTinRieng() const
{
    if (chucVu.empty())
        return false;

    if (capQuanLy.empty())
        return false;

    if (thongTinDacThu.empty())
        return false;

    return true;
}


//Phân loại nhân viên
string QuanLy::getLoaiNhanVien() const
{
    return "Quan ly";
}