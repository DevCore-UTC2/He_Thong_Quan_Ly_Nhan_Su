#include "../../include/PhongBan/PhongBan.h"

//Khởi tạo constructor
PhongBan::PhongBan()
{
    maPhongBan = "";
    tenPhongBan = "";
}

PhongBan::PhongBan(string maPhongBan, string tenPhongBan)
{
    this->maPhongBan = maPhongBan;
    this->tenPhongBan = tenPhongBan;
}

//Nhập thông tin phòng ban
void PhongBan::nhap()
{
    cout << "Nhap ma phong ban: ";
    getline(cin, maPhongBan);

    cout << "Nhap ten phong ban: ";
    getline(cin, tenPhongBan);
}

//Xuất thông tin phòng ban và toàn bộ nhân viên
void PhongBan::xuat() const
{
    cout << "\n=============================================" << endl;
    cout << "THONG TIN PHONG BAN" << endl;
    cout << "Ma phong ban: " << maPhongBan << endl;
    cout << "Ten phong ban: " << tenPhongBan << endl;
    cout << "Tong so nhan vien: " << danhSachNhanVien.size() << endl;
    cout << "---------------------------------------------" << endl;

    // Đa hình: mỗi nv->xuat() gọi đúng hàm của lớp con (Toàn thời gian, Quản lý...)
    for (const auto& nv : danhSachNhanVien)
    {
        cout << "[" << nv->getLoaiNhanVien() << "]" << endl;
        nv->xuat();
        cout << "---------------------------------------------" << endl;
    }
}

//Cập nhật thông tin phòng ban
void PhongBan::capNhatThongTin()
{
    int luaChon;

    do
    {
        cout << "\n===== CAP NHAT THONG TIN PHONG BAN =====" << endl;
        cout << "1. Cap nhat ma phong ban" << endl;
        cout << "2. Cap nhat ten phong ban" << endl;
        cout << "0. Thoat" << endl;
        cout << "Lua chon: ";
        cin >> luaChon;
        cin.ignore();

        switch (luaChon)
        {
            case 1:
            {
                string maMoi;

                cout << "Nhap ma phong ban moi: ";
                getline(cin, maMoi);

                setMaPhongBan(maMoi);
                break;
            }

            case 2:
            {
                string tenMoi;

                cout << "Nhap ten phong ban moi: ";
                getline(cin, tenMoi);

                setTenPhongBan(tenMoi);
                break;
            }

            case 0:
                break;

            default:
                cout << "Lua chon khong hop le!" << endl;
        }

    } while (luaChon != 0);
}

//Lấy thông tin phòng ban
string PhongBan::getMaPhongBan() const
{
    return maPhongBan;
}

string PhongBan::getTenPhongBan() const
{
    return tenPhongBan;
}

int PhongBan::getSoLuongNhanVien() const
{
    return static_cast<int>(danhSachNhanVien.size());
}

//Thiết lập cụm hàm set kiểm soát thông tin phòng ban
void PhongBan::setMaPhongBan(string maPhongBan)
{
    if (!maPhongBan.empty())
    {
        this->maPhongBan = maPhongBan;
    }
    else
    {
        cout << "Ma phong ban khong duoc de trong!" << endl;
    }
}

void PhongBan::setTenPhongBan(string tenPhongBan)
{
    if (!tenPhongBan.empty())
    {
        this->tenPhongBan = tenPhongBan;
    }
    else
    {
        cout << "Ten phong ban khong duoc de trong!" << endl;
    }
}

//Thêm nhân viên vào phòng ban
bool PhongBan::themNhanVien(shared_ptr<NhanVien> nv)
{
    // Tiền điều kiện: con trỏ hợp lệ
    if (nv == nullptr)
    {
        cout << "Loi: Khong the them nhan vien rong!" << endl;
        return false;
    }

    // Nhân viên phải có mã để còn tìm kiếm / xóa
    if (nv->getMaNV().empty())
    {
        cout << "Loi: Nhan vien chua co ma nen khong the them vao phong ban!" << endl;
        return false;
    }

    // Không cho trùng mã trong cùng một phòng ban
    if (coNhanVien(nv->getMaNV()))
    {
        cout << "Loi: Nhan vien ma " << nv->getMaNV() << " da co trong phong ban!" << endl;
        return false;
    }

    danhSachNhanVien.push_back(nv);
    cout << "Da them nhan vien " << nv->getHoTen() << " vao phong ban " << tenPhongBan << "." << endl;
    return true;
}

//Xóa nhân viên khỏi phòng ban (nhân viên vẫn tồn tại nếu nơi khác còn tham chiếu)
bool PhongBan::xoaNhanVien(const string& maNV)
{
    for (auto it = danhSachNhanVien.begin(); it != danhSachNhanVien.end(); ++it)
    {
        if ((*it)->getMaNV() == maNV)
        {
            danhSachNhanVien.erase(it);
            cout << "Da xoa nhan vien co ma " << maNV << " khoi phong ban." << endl;
            return true;
        }
    }

    cout << "Khong tim thay nhan vien ma " << maNV << " de xoa." << endl;
    return false;
}

//Kiểm tra nhân viên có trong phòng ban hay không
bool PhongBan::coNhanVien(const string& maNV) const
{
    return timNhanVien(maNV) != nullptr;
}

//Tìm nhân viên theo mã
shared_ptr<NhanVien> PhongBan::timNhanVien(const string& maNV) const
{
    for (const auto& nv : danhSachNhanVien)
    {
        if (nv->getMaNV() == maNV)
        {
            return nv;
        }
    }

    return nullptr;
}

//Lấy danh sách nhân viên (chỉ đọc)
const vector<shared_ptr<NhanVien>>& PhongBan::getDanhSachNhanVien() const
{
    return danhSachNhanVien;
}

//Kiểm tra hợp lệ thông tin phòng ban
bool PhongBan::kiemTraHopLe() const
{
    return (!maPhongBan.empty() && !tenPhongBan.empty());
}
