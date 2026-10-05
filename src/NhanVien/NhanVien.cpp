#include "NhanVien.h"

//contructor
NhanVien::NhanVien()
{
    maNV = "";
    hoTen="";
    ngaySinh="";
    soDienThoai="";
    email="";
}

//destructor
NhanVien::~NhanVien()
{
}

//Nhap, dung getline để nhập chuỗi có khoảng trắng
void NhanVien::nhap()
{
    cout<<"Nhap ma nhan vien: ";
    getline(cin, maNV);

    cout<<"Nhap ho ten nhan vien: ";
    getline(cin, hoTen);

    cout<<"Nhap ngay sinh nhan vien: ";
    getline(cin, ngaySinh);

    cout<<"Nhap so dien thoai nhan vien: ";
    getline(cin, soDienThoai);

    cout<<"Nhap email nhan vien: ";
    getline(cin, email);
}

//Xuat, const để không thay đổi giá trị của đối tượng
void NhanVien::xuat() const
{
    cout<<"Ma nhan vien: "<<maNV<<endl;
    cout<<"Ho ten nhan vien: "<<hoTen<<endl;
    cout<<"Ngay sinh nhan vien: "<<ngaySinh<<endl;
    cout<<"So dien thoai nhan vien: "<<soDienThoai<<endl;
    cout<<"Email nhan vien: "<<email<<endl;
}

//Cụm hàm lấy thông tin nhân viên
string NhanVien::getMaNV() const
{
    return maNV;
}

string NhanVien::getHoTen() const
{
    return hoTen;
}

string NhanVien::getNgaySinh() const
{
    return ngaySinh;
}

string NhanVien::getSoDienThoai() const
{
    return soDienThoai;
}

string NhanVien::getEmail() const
{
    return email;
}

//Funcion cập nhật thông tin
void NhanVien::capNhatThongTin()
{
    int luaChon;
    do 
    {
        cout<<"\n======= Cap nhat thong tin nhan vien =======\n";
        cout<<"1. Cap nhat ho ten\n";
        cout<<"2. Cap nhat ngay sinh\n";
        cout<<"3. Cap nhat so dien thoai\n";
        cout<<"4. Cap nhat email\n";
        cout<<"0. Thoat\n";
        cout<<"Nhap lua chon: ";
        cin>>luaChon;
        cin.ignore(); // Xóa ký tự enter còn lại trong bộ đệm sau khi nhập số nguyên
        switch (luaChon)
        {
            case 1:
                cout<<"Nhap ho ten moi: ";
                getline(cin, hoTen);
                cout<<"Cap nhat ho ten thanh cong!\n";
                break;
            case 2:
                cout<<"Nhap ngay sinh moi: ";
                getline(cin, ngaySinh);
                cout<<"Cap nhat ngay sinh thanh cong!\n";
                break;
            case 3:
                cout<<"Nhap so dien thoai moi: ";
                getline(cin, soDienThoai);
                cout<<"Cap nhat so dien thoai thanh cong!\n";
                break;
            case 4:
                cout<<"Nhap email moi: ";
                getline(cin, email);
                cout<<"Cap nhat email thanh cong!\n";
                break;
            case 0:
                cout<<"Thoat cap nhat thong tin.\n";
                break;
            default:
                cout<<"Lua chon khong hop le. Vui long chon lai.\n";
        }
    } while (luaChon != 0);
}

//Hàm kiểm tra hợp lệ của thông tin nhân viên
bool NhanVien::kiemTraHopLe() const
{
    //Hàm empty đã có sẵn trong C++17 để kiểm tra chuỗi rỗng
    if(maNV.empty() || hoTen.empty() || ngaySinh.empty() || soDienThoai.empty() || email.empty())
    {
        return false; // Thông tin không hợp lệ nếu có trường nào trống
    }
    return true; // Thông tin hợp lệ
}

string NhanVien::getLoaiNhanVien() const
{
    return "Nhan Vien"; // Trả về loại nhân viên là "Nhan Vien" cho lớp cơ sở
}

//Cụm hàm set: chặn dữ liệu sai, không cho ai muốn nhập gì thì nhập VD hãy nhìn hàm setEmail
void NhanVien::setMaNV(string maNV)
{
    this->maNV = maNV;
}
void NhanVien::setHoTen(string hoTen)
{
    this->hoTen = hoTen;
}
void NhanVien::setNgaySinh(string ngaySinh)
{
    this->ngaySinh = ngaySinh;
}
void NhanVien::setSoDienThoai(string soDienThoai)
{
    this->soDienThoai = soDienThoai;
}
void NhanVien::setEmail(string email)
{
    //find có sẵn trong thư viện và được dùng để tìm kí tự
    //string::npos là giá trị được trả về khi không tìm thấy
    if (email.find('@') != string::npos && email.find('.') != string::npos)
    {
        this->email = email;
    }
    else
    {
        cout << "Email khong hop le!" << endl;
    }
}