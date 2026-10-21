#include "../../Include/ThongKe/ThongKe.h"
#include <iomanip>
#include <sstream>
#include <set>

// ===== Ham ho tro noi bo =====
namespace
{
    const int DO_RONG = 72;

    struct NhomLuong
    {
        int so = 0;
        double thuNhap = 0;
        double thucLinh = 0;
    };

    struct NhomHopDong
    {
        int so = 0;
        double tongLuong = 0;
    };

    struct NhomKy
    {
        int so = 0;
        int tongCong = 0;
        int tongNghi = 0;
        double tongDiem = 0;
    };

    void inTieuDe(const string& tieuDe)
    {
        cout << "\n" << string(DO_RONG, '=') << "\n";
        cout << "  " << tieuDe << "\n";
        cout << string(DO_RONG, '=') << "\n";
    }

    void inMuc(const string& tieuDe)
    {
        cout << "\n  --- " << tieuDe << " ---\n";
    }

    // In 1 dong "nhan : gia tri"
    void dong(const string& nhan, const string& giaTri)
    {
        cout << "  " << left << setw(36) << nhan << ": " << giaTri << "\n";
    }

    string so2(double v)
    {
        ostringstream os;
        os << fixed << setprecision(2) << v;
        return os.str();
    }

    string tien(double v)
    {
        return dinhDangTienVND(v) + " VND";
    }

    double tyLe(int a, int b)
    {
        return b > 0 ? 100.0 * a / b : 0.0;
    }

    string soVaTyLe(int a, int tong)
    {
        return to_string(a) + " (" + so2(tyLe(a, tong)) + "%)";
    }
}

// ===== Constructor =====
ThongKe::ThongKe()
{
}

// ===== Loi nghiep vu =====
void ThongKe::ghiNhanLoi(const LoiNghiepVu& loi)
{
    string loai = loi.getLoaiLoi();
    demLoi[loai]++;
    nhatKyLoi.push_back("[" + loai + "] " + loi.getThongBao());
}

void ThongKe::ghiNhanLoi(const exception& loi)
{
    demLoi["Loi he thong"]++;
    nhatKyLoi.push_back(string("[Loi he thong] ") + loi.what());
}

int ThongKe::tongSoLoi() const
{
    return static_cast<int>(nhatKyLoi.size());
}

int ThongKe::demTheoLoai(const string& loaiLoi) const
{
    auto it = demLoi.find(loaiLoi);
    return it != demLoi.end() ? it->second : 0;
}

void ThongKe::xoaNhatKyLoi()
{
    demLoi.clear();
    nhatKyLoi.clear();
}

void ThongKe::thongKeLoi(int soDongNhatKyToiDa) const
{
    inTieuDe("THONG KE LOI NGHIEP VU");

    int tong = tongSoLoi();
    if (tong == 0)
    {
        cout << "  Chua ghi nhan loi nao.\n";
        return;
    }

    dong("Tong so loi", to_string(tong));
    dong("So loai loi", to_string(demLoi.size()));

    inMuc("Theo loai loi");
    cout << "  " << left << setw(36) << "Loai loi" << setw(10) << "So lan" << "Ty le" << "\n";
    cout << "  " << string(58, '-') << "\n";

    string loaiNhieuNhat;
    int max = 0;
    for (const auto& x : demLoi)
    {
        cout << "  " << left << setw(36) << x.first << setw(10) << x.second
             << so2(tyLe(x.second, tong)) << "%\n";
        if (x.second > max)
        {
            max = x.second;
            loaiNhieuNhat = x.first;
        }
    }
    dong("\n  Loai loi xay ra nhieu nhat", loaiNhieuNhat + " (" + to_string(max) + " lan)");

    // Nhat ky: chi in cac loi gan day nhat
    int tu = tong > soDongNhatKyToiDa ? tong - soDongNhatKyToiDa : 0;
    inMuc("Nhat ky loi gan day (" + to_string(tong - tu) + "/" + to_string(tong) + ")");
    for (int i = tu; i < tong; i++)
    {
        cout << "  " << (i + 1) << ". " << nhatKyLoi[i] << "\n";
    }
}

// ===== Nhan vien =====
void ThongKe::thongKeNhanVien(const vector<shared_ptr<NhanVien>>& dsNhanVien)
{
    inTieuDe("THONG KE NHAN VIEN");

    map<string, int> theoLoai, theoTrangThai;
    int tong = 0, hopLe = 0;

    for (const auto& nv : dsNhanVien)
    {
        if (nv == nullptr)
            continue;

        tong++;
        theoLoai[nv->getLoaiNhanVien()]++;
        theoTrangThai[nv->getTrangThai().empty() ? "(rong)" : nv->getTrangThai()]++;
        if (nv->kiemTraHopLe())
            hopLe++;
    }

    if (tong == 0)
    {
        cout << "  Chua co nhan vien nao.\n";
        return;
    }

    dong("Tong so nhan vien", to_string(tong));
    dong("Thong tin day du / hop le", soVaTyLe(hopLe, tong));
    dong("Thong tin thieu / khong hop le", soVaTyLe(tong - hopLe, tong));

    inMuc("Theo loai nhan vien");
    for (const auto& x : theoLoai)
        dong(x.first, soVaTyLe(x.second, tong));

    inMuc("Theo trang thai");
    for (const auto& x : theoTrangThai)
        dong(x.first, soVaTyLe(x.second, tong));
}

// ===== Phong ban =====
void ThongKe::thongKePhongBan(const vector<PhongBan>& dsPhongBan)
{
    inTieuDe("THONG KE PHONG BAN");

    if (dsPhongBan.empty())
    {
        cout << "  Chua co phong ban nao.\n";
        return;
    }

    int tongNV = 0, khongHopLe = 0, phongTrong = 0;
    size_t iMax = 0;

    for (size_t i = 0; i < dsPhongBan.size(); i++)
    {
        const PhongBan& pb = dsPhongBan[i];
        tongNV += pb.getSoLuongNhanVien();

        if (!pb.kiemTraHopLe())
            khongHopLe++;
        if (pb.getSoLuongNhanVien() == 0)
            phongTrong++;
        if (pb.getSoLuongNhanVien() > dsPhongBan[iMax].getSoLuongNhanVien())
            iMax = i;
    }

    dong("Tong so phong ban", to_string(dsPhongBan.size()));
    dong("Tong so nhan vien (cong don)", to_string(tongNV));
    dong("Trung binh nhan vien / phong", so2((double)tongNV / dsPhongBan.size()));
    dong("Phong chua co nhan vien", to_string(phongTrong));
    dong("Phong thong tin khong hop le", to_string(khongHopLe));
    dong("Phong dong nhan vien nhat",
         dsPhongBan[iMax].getTenPhongBan() + " (" + to_string(dsPhongBan[iMax].getSoLuongNhanVien()) + " NV)");

    inMuc("Chi tiet tung phong");
    for (const auto& pb : dsPhongBan)
    {
        map<string, int> theoLoai;
        for (const auto& nv : pb.getDanhSachNhanVien())
        {
            if (nv != nullptr)
                theoLoai[nv->getLoaiNhanVien()]++;
        }

        cout << "  [" << pb.getMaPhongBan() << "] " << pb.getTenPhongBan()
             << " - " << pb.getSoLuongNhanVien() << " nhan vien\n";
        for (const auto& x : theoLoai)
            cout << "      + " << left << setw(30) << x.first << x.second << "\n";
    }
}

// ===== Hop dong =====
void ThongKe::thongKeHopDong(const vector<HopDongLaoDong>& dsHopDong)
{
    inTieuDe("THONG KE HOP DONG LAO DONG");

    if (dsHopDong.empty())
    {
        cout << "  Chua co hop dong nao.\n";
        return;
    }

    int tong = (int)dsHopDong.size();
    int khongHopLe = 0, chuaGan = 0, nvDaNghi = 0;
    double tongLuong = 0;
    size_t iMax = 0, iMin = 0;
    map<string, NhomHopDong> theoLoaiHD;
    map<string, int> theoLoaiNV, demMa;

    for (size_t i = 0; i < dsHopDong.size(); i++)
    {
        const HopDongLaoDong& hd = dsHopDong[i];

        if (!hd.kiemTraHopLe())
            khongHopLe++;

        tongLuong += hd.getLuongThoaThuan();
        if (hd.getLuongThoaThuan() > dsHopDong[iMax].getLuongThoaThuan())
            iMax = i;
        if (hd.getLuongThoaThuan() < dsHopDong[iMin].getLuongThoaThuan())
            iMin = i;

        NhomHopDong& nhom = theoLoaiHD[hd.getLoaiHopDong().empty() ? "(chua ghi loai)" : hd.getLoaiHopDong()];
        nhom.so++;
        nhom.tongLuong += hd.getLuongThoaThuan();

        demMa[hd.getMaHopDong()]++;

        shared_ptr<NhanVien> nv = hd.getNguoiLaoDong();
        if (nv == nullptr)
        {
            chuaGan++;
        }
        else
        {
            theoLoaiNV[nv->getLoaiNhanVien()]++;
            if (nv->getTrangThai() == "Da nghi viec")
                nvDaNghi++;
        }
    }

    int maTrung = 0;
    for (const auto& x : demMa)
    {
        if (x.second > 1)
            maTrung += x.second - 1;
    }

    dong("Tong so hop dong", to_string(tong));
    dong("Hop dong hop le", soVaTyLe(tong - khongHopLe, tong));
    dong("Hop dong khong hop le", soVaTyLe(khongHopLe, tong));
    dong("Hop dong chua gan nhan vien", to_string(chuaGan));
    dong("Hop dong cua NV da nghi viec", to_string(nvDaNghi));
    dong("Hop dong bi trung ma", to_string(maTrung));

    inMuc("Luong thoa thuan");
    dong("Tong luong thoa thuan", tien(tongLuong));
    dong("Luong thoa thuan trung binh", tien(tongLuong / tong));
    dong("Cao nhat", tien(dsHopDong[iMax].getLuongThoaThuan()) + " (HD " + dsHopDong[iMax].getMaHopDong() + ")");
    dong("Thap nhat", tien(dsHopDong[iMin].getLuongThoaThuan()) + " (HD " + dsHopDong[iMin].getMaHopDong() + ")");

    inMuc("Theo loai hop dong");
    cout << "  " << left << setw(24) << "Loai" << setw(8) << "So HD" << "Luong TB (VND)\n";
    cout << "  " << string(58, '-') << "\n";
    for (const auto& x : theoLoaiHD)
    {
        cout << "  " << left << setw(24) << x.first << setw(8) << x.second.so
             << dinhDangTienVND(x.second.tongLuong / x.second.so) << "\n";
    }

    inMuc("Theo loai nhan vien");
    for (const auto& x : theoLoaiNV)
        dong(x.first, soVaTyLe(x.second, tong - chuaGan));
}

// ===== Cham cong =====
void ThongKe::thongKeChamCong(const vector<ChamCong>& dsChamCong)
{
    inTieuDe("THONG KE CHAM CONG");

    if (dsChamCong.empty())
    {
        cout << "  Chua co ban ghi cham cong nao.\n";
        return;
    }

    int tong = (int)dsChamCong.size();
    int daChot = 0, dangCham = 0, khongHopLe = 0, chuyenCan = 0;
    int tongCong = 0, tongNghi = 0;
    map<string, NhomKy> theoKy;
    map<string, pair<int, int>> theoNV;    // maNV -> (tong cong, tong nghi)

    for (const auto& cc : dsChamCong)
    {
        if (cc.getTrangThai() == "Da chot cong")
            daChot++;
        else if (cc.getTrangThai() == "Dang cham cong")
            dangCham++;

        if (!cc.kiemTraHopLe())
            khongHopLe++;
        if (cc.getSoNgayNghi() == 0)
            chuyenCan++;

        tongCong += cc.getSoNgayCong();
        tongNghi += cc.getSoNgayNghi();

        NhomKy& ky = theoKy[to_string(cc.getThang()) + "/" + to_string(cc.getNam())];
        ky.so++;
        ky.tongCong += cc.getSoNgayCong();
        ky.tongNghi += cc.getSoNgayNghi();

        pair<int, int>& nv = theoNV[cc.getMaNhanVien()];
        nv.first += cc.getSoNgayCong();
        nv.second += cc.getSoNgayNghi();
    }

    dong("Tong so ban ghi", to_string(tong));
    dong("So nhan vien co cham cong", to_string(theoNV.size()));
    dong("Da chot cong", soVaTyLe(daChot, tong));
    dong("Dang cham cong", soVaTyLe(dangCham, tong));
    dong("Ban ghi khong hop le", soVaTyLe(khongHopLe, tong));

    inMuc("Ngay cong / ngay nghi");
    dong("Tong ngay cong", to_string(tongCong));
    dong("Tong ngay nghi", to_string(tongNghi));
    dong("Ngay cong trung binh / ban ghi", so2((double)tongCong / tong));
    dong("Ngay nghi trung binh / ban ghi", so2((double)tongNghi / tong));
    dong("Ty le nghi", so2(tyLe(tongNghi, tongCong + tongNghi)) + "%");
    dong("Ban ghi khong nghi ngay nao", soVaTyLe(chuyenCan, tong));

    // Nhan vien nghi nhieu nhat / lam nhieu nhat
    string maNghiNhieu, maLamNhieu;
    int nghiMax = -1, congMax = -1;
    for (const auto& x : theoNV)
    {
        if (x.second.second > nghiMax)
        {
            nghiMax = x.second.second;
            maNghiNhieu = x.first;
        }
        if (x.second.first > congMax)
        {
            congMax = x.second.first;
            maLamNhieu = x.first;
        }
    }
    dong("NV nghi nhieu nhat", maNghiNhieu + " (" + to_string(nghiMax) + " ngay)");
    dong("NV nhieu ngay cong nhat", maLamNhieu + " (" + to_string(congMax) + " ngay)");

    inMuc("Theo ky (thang/nam)");
    cout << "  " << left << setw(12) << "Ky" << setw(10) << "So BG" << setw(14) << "Tong cong" << "Tong nghi\n";
    cout << "  " << string(48, '-') << "\n";
    for (const auto& x : theoKy)
    {
        cout << "  " << left << setw(12) << x.first << setw(10) << x.second.so
             << setw(14) << x.second.tongCong << x.second.tongNghi << "\n";
    }
}

// ===== KPI =====
void ThongKe::thongKeKPI(const vector<HeThongKPI>& dsKPI)
{
    inTieuDe("THONG KE KPI");

    if (dsKPI.empty())
    {
        cout << "  Chua co du lieu KPI nao.\n";
        return;
    }

    int tong = (int)dsKPI.size();
    int khongHopLe = 0, xuatSac = 0, dat = 0, chuaDat = 0;
    double tongDiem = 0, tongThuong = 0;
    size_t iMax = 0, iMin = 0;
    map<string, NhomKy> theoKy;

    for (size_t i = 0; i < dsKPI.size(); i++)
    {
        const HeThongKPI& k = dsKPI[i];

        if (!k.kiemTraHopLe())
            khongHopLe++;

        if (k.getDiemKPI() >= 90)
            xuatSac++;
        else if (k.getDiemKPI() >= 70)
            dat++;
        else
            chuaDat++;

        tongDiem += k.getDiemKPI();
        tongThuong += k.tinhTienThuong();   // = 0 neu chua gan chinh sach thuong

        if (k.getDiemKPI() > dsKPI[iMax].getDiemKPI())
            iMax = i;
        if (k.getDiemKPI() < dsKPI[iMin].getDiemKPI())
            iMin = i;

        NhomKy& ky = theoKy[k.getThang() + "/" + k.getNam()];
        ky.so++;
        ky.tongDiem += k.getDiemKPI();
    }

    dong("Tong so ban ghi KPI", to_string(tong));
    dong("Ban ghi khong hop le", soVaTyLe(khongHopLe, tong));
    dong("Diem KPI trung binh", so2(tongDiem / tong));
    dong("Diem cao nhat", so2(dsKPI[iMax].getDiemKPI()) + " (NV " + dsKPI[iMax].getMaNV() + ")");
    dong("Diem thap nhat", so2(dsKPI[iMin].getDiemKPI()) + " (NV " + dsKPI[iMin].getMaNV() + ")");
    dong("Tong tien thuong KPI", tien(tongThuong));

    inMuc("Phan loai (theo moc thuong)");
    dong("Xuat sac (>= 90)", soVaTyLe(xuatSac, tong));
    dong("Dat (70 - duoi 90)", soVaTyLe(dat, tong));
    dong("Chua dat (< 70, khong thuong)", soVaTyLe(chuaDat, tong));

    inMuc("Theo ky (thang/nam)");
    cout << "  " << left << setw(12) << "Ky" << setw(10) << "So BG" << "Diem TB\n";
    cout << "  " << string(36, '-') << "\n";
    for (const auto& x : theoKy)
    {
        cout << "  " << left << setw(12) << x.first << setw(10) << x.second.so
             << so2(x.second.tongDiem / x.second.so) << "\n";
    }
}

// ===== Bang luong =====
void ThongKe::thongKeBangLuong(const vector<BangLuong>& dsLuong)
{
    inTieuDe("THONG KE BANG LUONG");

    if (dsLuong.empty())
    {
        cout << "  Chua co bang luong nao.\n";
        return;
    }

    int tong = (int)dsLuong.size();
    double tongLCB = 0, tongPC = 0, tongThuong = 0, tongBS = 0;
    double tongThuNhap = 0, tongBH = 0, tongThue = 0, tongThucLinh = 0;
    size_t iMax = 0, iMin = 0;
    map<string, NhomLuong> theoLoai, theoKy;
    set<string> dsMaNV;

    for (size_t i = 0; i < dsLuong.size(); i++)
    {
        const BangLuong& b = dsLuong[i];

        tongLCB += b.getLuongCoBan();
        tongPC += b.getPhuCap();
        tongThuong += b.getThuongKPI();
        tongBS += b.getKhoanBoSung();
        tongThuNhap += b.tongThuNhap();
        tongBH += b.getBaoHiem();
        tongThue += b.getThue();
        tongThucLinh += b.luongThucLinh();
        dsMaNV.insert(b.getMaNV());

        if (b.luongThucLinh() > dsLuong[iMax].luongThucLinh())
            iMax = i;
        if (b.luongThucLinh() < dsLuong[iMin].luongThucLinh())
            iMin = i;

        NhomLuong& l = theoLoai[b.getLoaiNhanVien()];
        l.so++;
        l.thuNhap += b.tongThuNhap();
        l.thucLinh += b.luongThucLinh();

        NhomLuong& k = theoKy[b.getThang() + "/" + b.getNam()];
        k.so++;
        k.thuNhap += b.tongThuNhap();
        k.thucLinh += b.luongThucLinh();
    }

    dong("Tong so bang luong", to_string(tong));
    dong("So nhan vien duoc tra luong", to_string(dsMaNV.size()));

    inMuc("Tong hop chi phi luong");
    dong("Tong luong co ban", tien(tongLCB));
    dong("Tong phu cap", tien(tongPC));
    dong("Tong thuong KPI", tien(tongThuong));
    dong("Tong khoan bo sung", tien(tongBS));
    dong("TONG THU NHAP", tien(tongThuNhap));
    dong("Tong bao hiem", tien(tongBH));
    dong("Tong thue thu nhap ca nhan", tien(tongThue));
    dong("TONG THUC LINH", tien(tongThucLinh));

    inMuc("Muc luong");
    dong("Thuc linh trung binh", tien(tongThucLinh / tong));
    dong("Thuc linh cao nhat", tien(dsLuong[iMax].luongThucLinh()) + " (" +
         dsLuong[iMax].getMaNV() + " - " + dsLuong[iMax].getHoTen() + ")");
    dong("Thuc linh thap nhat", tien(dsLuong[iMin].luongThucLinh()) + " (" +
         dsLuong[iMin].getMaNV() + " - " + dsLuong[iMin].getHoTen() + ")");
    dong("Khau tru / tong thu nhap", so2(tongThuNhap > 0 ? 100.0 * (tongBH + tongThue) / tongThuNhap : 0.0) + "%");

    inMuc("Theo loai nhan vien");
    cout << "  " << left << setw(28) << "Loai" << setw(8) << "So" << setw(18) << "Tong thuc linh" << "Thuc linh TB\n";
    cout << "  " << string(68, '-') << "\n";
    for (const auto& x : theoLoai)
    {
        cout << "  " << left << setw(28) << x.first << setw(8) << x.second.so
             << setw(18) << dinhDangTienVND(x.second.thucLinh)
             << dinhDangTienVND(x.second.thucLinh / x.second.so) << "\n";
    }

    inMuc("Theo ky luong");
    cout << "  " << left << setw(12) << "Ky" << setw(8) << "So" << setw(18) << "Tong thu nhap" << "Tong thuc linh\n";
    cout << "  " << string(58, '-') << "\n";
    for (const auto& x : theoKy)
    {
        cout << "  " << left << setw(12) << x.first << setw(8) << x.second.so
             << setw(18) << dinhDangTienVND(x.second.thuNhap)
             << dinhDangTienVND(x.second.thucLinh) << "\n";
    }
}

// ===== Bao cao tong hop =====
void ThongKe::baoCaoTongHop(const vector<PhongBan>& dsPhongBan,
                            const vector<HopDongLaoDong>& dsHopDong,
                            const vector<ChamCong>& dsChamCong,
                            const vector<HeThongKPI>& dsKPI,
                            const vector<BangLuong>& dsLuong) const
{
    cout << "\n" << string(DO_RONG, '#') << "\n";
    cout << "  BAO CAO THONG KE TONG HOP\n";
    cout << string(DO_RONG, '#') << "\n";

    // Gom nhan vien tu cac phong ban, bo trung ma
    vector<shared_ptr<NhanVien>> dsNhanVien;
    set<string> daCo;
    for (const auto& pb : dsPhongBan)
    {
        for (const auto& nv : pb.getDanhSachNhanVien())
        {
            if (nv != nullptr && daCo.insert(nv->getMaNV()).second)
                dsNhanVien.push_back(nv);
        }
    }

    thongKeNhanVien(dsNhanVien);
    thongKePhongBan(dsPhongBan);
    thongKeHopDong(dsHopDong);
    thongKeChamCong(dsChamCong);
    thongKeKPI(dsKPI);
    thongKeBangLuong(dsLuong);
    thongKeLoi();

    cout << "\n" << string(DO_RONG, '#') << "\n";
    cout << "  HET BAO CAO\n";
    cout << string(DO_RONG, '#') << "\n";
}
