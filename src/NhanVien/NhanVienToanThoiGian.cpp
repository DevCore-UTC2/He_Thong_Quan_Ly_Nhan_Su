#include "NhanVienToanThoiGian.h"

//Khởi tạo contructor
NhanVienToanThoiGian::NhanVienToanThoiGian()
{
    soNgayCong = 0;
    soGioLam = 0.0;
    caLamViec = "";
}

//Nhap thông tin nhân viên toàn thời gian
void NhanVienToanThoiGian::nhap()
{
    NhanVien::nhap(); //Kế thừa thông tin chung của lớp cha

    cout<<"Nhap so ngay cong: ";
    cin>>soNgayCong;

    cout<<"Nhap so gio lam: ";
    cin>>soGioLam;

    cin.ignore(); //Xóa bộ đệm trước khi nhập chuỗi
    cout<<"Nhap ca lam viec: ";
    getline(cin, caLamViec);
}

//Xuat thông tin nhân viên toàn thời gian
void NhanVienToanThoiGian::xuat() const
{
    NhanVien::xuat(); //Kế thừa thông tin chung của lớp cha

    cout<<"So ngay cong: "<<soNgayCong<<endl;
    cout<<"So gio lam: "<<soGioLam<<endl;
    cout<<"Ca lam viec: "<<caLamViec<<endl;
}


//Lấy thông tin nhân viên toàn thời gian
int NhanVienToanThoiGian::getSoNgayCong() const
{
    return soNgayCong;
}

float NhanVienToanThoiGian::getSoGioLam() const
{
    return soGioLam;
}

string NhanVienToanThoiGian::getCaLamViec() const
{
    return caLamViec;
}

//Cập nhật thông tin nhân viên toàn thời gian
void NhanVienToanThoiGian::capNhatThongTin()
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
                    cout << "1. So ngay cong" << endl;
                    cout << "2. So gio lam" << endl;
                    cout << "3. Ca lam viec" << endl;
                    cout << "0. Thoat" << endl;
                    cout << "Lua chon: ";
                    cin >> chon;
                    cin.ignore();

                    switch (chon)
                    {
                        case 1:
                        {
                            int ngayCongMoi;

                            cout << "Nhap so ngay cong moi: ";
                            cin >> ngayCongMoi;
                            cin.ignore();

                            setSoNgayCong(ngayCongMoi);
                            break;
                        }

                        case 2:
                        {
                            float gioLamMoi;

                            cout << "Nhap so gio lam moi: ";
                            cin >> gioLamMoi;
                            cin.ignore();

                            setSoGioLam(gioLamMoi);
                            break;
                        }

                        case 3:
                        {
                            string caLamViecMoi;

                            cout << "Nhap ca lam viec moi: ";
                            getline(cin, caLamViecMoi);

                            setCaLamViec(caLamViecMoi);
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

//Thiết lập cụm hàm set kiểm soát thông tin nhân viên toàn thời gian
void NhanVienToanThoiGian::setSoNgayCong(int soNgayCong)
{
    if (soNgayCong >= 0)
    {
        this->soNgayCong = soNgayCong;
    }
    else
    {
        cout << "So ngay cong khong hop le!" << endl;
    }
}

void NhanVienToanThoiGian::setSoGioLam(float soGioLam)
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

void NhanVienToanThoiGian::setCaLamViec(string caLamViec)
{
    if (!caLamViec.empty())
    {
        this->caLamViec = caLamViec;
    }
    else
    {
        cout << "Ca lam viec khong hop le!" << endl;
    }
}

//Kiểm tra hợp lệ thông tin riêng của nhân viên toàn thời gian
bool NhanVienToanThoiGian::kiemTraThongTinRieng() const
{
    return (soNgayCong >= 0 && soGioLam >= 0 && !caLamViec.empty());
}

//Phân loại nhân viên toàn thời gian
string NhanVienToanThoiGian::getLoaiNhanVien() const
{
    return "Nhan Vien Toan Thoi Gian";
}