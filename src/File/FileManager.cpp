#include "FileManager.h"
#include "LoiNghiepVu.h"
#include "ChamCong.h"
#include "NhanVienToanThoiGian.h"
#include "NhanVienBanThoiGian.h"
#include "QuanLy.h"
#include "ChuyenGia.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace {

string lamSach(string s) {
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '|' || s[i] == '\n' || s[i] == '\r') s[i] = ' ';
    }
    return s;
}

string soThuc(double v, int doChinhXac) {
    ostringstream o;
    o << setprecision(doChinhXac) << v;
    return o.str();
}

vector<string> tach(const string& dong, char phanCach) {
    vector<string> kq;
    size_t dau = 0;
    while (true) {
        size_t vt = dong.find(phanCach, dau);
        if (vt == string::npos) {
            kq.push_back(dong.substr(dau));
            break;
        }
        kq.push_back(dong.substr(dau, vt - dau));
        dau = vt + 1;
    }
    return kq;
}

string loiDong(int soDong, const string& noiDung) {
    ostringstream o;
    o << "Dong " << soDong << ": " << noiDung;
    return o.str();
}

void kiemTraSoTruong(const vector<string>& f, size_t soTruong, int soDong) {
    if (f.size() != soTruong) {
        ostringstream o;
        o << soTruong;
        throw LoiFile(loiDong(soDong, "sai so truong du lieu (can " + o.str() + ")"));
    }
}

double docSo(const string& s, int soDong) {
    const char* dau = s.c_str();
    char* cuoi = nullptr;
    double v = strtod(dau, &cuoi);
    if (s.empty() || cuoi == dau || *cuoi != '\0') {
        throw LoiFile(loiDong(soDong, "gia tri so khong hop le: '" + s + "'"));
    }
    return v;
}

int docSoNguyen(const string& s, int soDong) {
    return (int)docSo(s, soDong);
}

void moGhi(ofstream& f, const string& duongDan) {
    f.open(duongDan.c_str());
    if (!f.is_open()) throw LoiFile("Khong the mo file de ghi: " + duongDan);
}

void moDoc(ifstream& f, const string& duongDan) {
    f.open(duongDan.c_str());
    if (!f.is_open()) throw LoiFile("Khong the mo file de doc: " + duongDan);
}

bool boQua(string& dong) {
    if (!dong.empty() && dong[dong.size() - 1] == '\r') dong.erase(dong.size() - 1);
    return dong.empty() || dong[0] == '#';
}


// Tam tat cout: setTrangThai() in thong bao "Cap nhat thanh cong" moi lan goi, khong can khi doc file
struct TamTatCout {
    ostringstream nuot;
    streambuf* cu;
    TamTatCout() : cu(cout.rdbuf(nuot.rdbuf())) {}
    ~TamTatCout() { cout.rdbuf(cu); }
};

// File cu khong co cot trang thai -> so truong = soTruongCoBan; file moi them 1 cot cuoi
bool kiemTraSoTruongCoTrangThai(const vector<string>& f, size_t soTruongCoBan, int soDong) {
    if (f.size() == soTruongCoBan) return false;
    if (f.size() == soTruongCoBan + 1) return true;
    ostringstream o;
    o << soTruongCoBan << " hoac " << soTruongCoBan + 1;
    throw LoiFile(loiDong(soDong, "sai so truong du lieu (can " + o.str() + ")"));
}

void datTrangThaiNhanVien(NhanVien& nv, const string& trangThai, int soDong) {
    if (trangThai != "Dang lam" && trangThai != "Nghi phep" &&
        trangThai != "Tam nghi" && trangThai != "Da nghi viec") {
        throw LoiFile(loiDong(soDong, "trang thai nhan vien khong hop le: '" + trangThai + "'"));
    }
    TamTatCout im;
    nv.setTrangThai(trangThai);
}

}

void FileManager::luuNhanVien(const DanhSachNhanVien& ds, const string& duongDan) {
    ofstream f;
    moGhi(f, duongDan);
    for (size_t i = 0; i < ds.size(); i++) {
        const NhanVien* nv = ds[i].get();
        if (nv == nullptr) continue;

        if (nv->getTrangThai().empty()) {
            throw LoiDuLieuKhongHopLe("Khong luu duoc nhan vien " + nv->getMaNV() + ": trang thai nhan vien chua hop le");
        }

        string chung = lamSach(nv->getMaNV()) + "|" + lamSach(nv->getHoTen()) + "|" +
                       lamSach(nv->getNgaySinh()) + "|" + lamSach(nv->getSoDienThoai()) + "|" +
                       lamSach(nv->getEmail());
        // Trang thai la cot cuoi cung cua moi dong
        string cuoi = "|" + lamSach(nv->getTrangThai()) + "\n";

        if (const NhanVienToanThoiGian* t = dynamic_cast<const NhanVienToanThoiGian*>(nv)) {
            f << "TT|" << chung << "|" << t->getSoNgayCong() << "|" << soThuc(t->getSoGioLam(), 7)
              << "|" << lamSach(t->getCaLamViec()) << cuoi;
        } else if (const NhanVienBanThoiGian* b = dynamic_cast<const NhanVienBanThoiGian*>(nv)) {
            f << "BT|" << chung << "|" << soThuc(b->getSoGioLam(), 7) << "|"
              << soThuc(b->getLuongTheoGio(), 7) << cuoi;
        } else if (const QuanLy* q = dynamic_cast<const QuanLy*>(nv)) {
            f << "QL|" << chung << "|" << lamSach(q->getChucVu()) << "|" << lamSach(q->getCapQuanLy())
              << "|" << lamSach(q->getThongTinDacThu()) << cuoi;
        } else if (const ChuyenGia* c = dynamic_cast<const ChuyenGia*>(nv)) {
            f << "CG|" << chung << "|" << lamSach(c->getChuyenMon()) << "|" << c->getSoNamKinhNghiem() << cuoi;
        } else {
            throw LoiDuLieuKhongHopLe("Khong luu duoc nhan vien " + nv->getMaNV() + ": loai nhan vien chua duoc ho tro");
        }
    }
    if (!f) throw LoiFile("Loi khi ghi file: " + duongDan);
}

FileManager::DanhSachNhanVien FileManager::docNhanVien(const string& duongDan) {
    ifstream f;
    moDoc(f, duongDan);
    DanhSachNhanVien kq;
    string dong;
    int soDong = 0;

    while (getline(f, dong)) {
        soDong++;
        if (boQua(dong)) continue;
        vector<string> t = tach(dong, '|');
        const string& loai = t[0];

        shared_ptr<NhanVien> nv;
        bool coTrangThai = false;
        size_t cotTrangThai = 0;
        if (loai == "TT") {
            coTrangThai = kiemTraSoTruongCoTrangThai(t, 9, soDong);
            cotTrangThai = 9;
            shared_ptr<NhanVienToanThoiGian> p = make_shared<NhanVienToanThoiGian>();
            p->setSoNgayCong(docSoNguyen(t[6], soDong));
            p->setSoGioLam((float)docSo(t[7], soDong));
            p->setCaLamViec(t[8]);
            nv = p;
        } else if (loai == "BT") {
            coTrangThai = kiemTraSoTruongCoTrangThai(t, 8, soDong);
            cotTrangThai = 8;
            shared_ptr<NhanVienBanThoiGian> p = make_shared<NhanVienBanThoiGian>();
            p->setSoGioLam((float)docSo(t[6], soDong));
            p->setLuongTheoGio((float)docSo(t[7], soDong));
            nv = p;
        } else if (loai == "QL") {
            coTrangThai = kiemTraSoTruongCoTrangThai(t, 9, soDong);
            cotTrangThai = 9;
            shared_ptr<QuanLy> p = make_shared<QuanLy>();
            p->setChucVu(t[6]);
            p->setCapQuanLy(t[7]);
            p->setThongTinDacThu(t[8]);
            nv = p;
        } else if (loai == "CG") {
            coTrangThai = kiemTraSoTruongCoTrangThai(t, 8, soDong);
            cotTrangThai = 8;
            shared_ptr<ChuyenGia> p = make_shared<ChuyenGia>();
            p->setChuyenMon(t[6]);
            p->setSoNamKinhNghiem(docSoNguyen(t[7], soDong));
            nv = p;
        } else {
            throw LoiFile(loiDong(soDong, "ma loai nhan vien khong hop le: '" + loai + "'"));
        }

        nv->setMaNV(t[1]);
        nv->setHoTen(t[2]);
        nv->setNgaySinh(t[3]);
        nv->setSoDienThoai(t[4]);
        nv->setEmail(t[5]);
        if (coTrangThai) datTrangThaiNhanVien(*nv, t[cotTrangThai], soDong);   // file cu: giu mac dinh "Dang lam"
        kq.push_back(nv);
    }
    return kq;
}

void FileManager::luuHopDong(const vector<HopDongLaoDong>& ds, const string& duongDan) {
    ofstream f;
    moGhi(f, duongDan);
    for (size_t i = 0; i < ds.size(); i++) {
        shared_ptr<NhanVien> nv = ds[i].getNguoiLaoDong();
        if (nv == nullptr) {
            throw LoiDuLieuKhongHopLe("Hop dong " + ds[i].getMaHopDong() + " chua gan nhan vien, khong the luu");
        }
        f << lamSach(ds[i].getMaHopDong()) << "|" << lamSach(ds[i].getLoaiHopDong()) << "|"
          << lamSach(ds[i].getNgayKyKet()) << "|" << soThuc(ds[i].getLuongThoaThuan(), 17) << "|"
          << lamSach(nv->getMaNV()) << "\n";
    }
    if (!f) throw LoiFile("Loi khi ghi file: " + duongDan);
}

vector<HopDongLaoDong> FileManager::docHopDong(const string& duongDan, const DanhSachNhanVien& nhanVien) {
    ifstream f;
    moDoc(f, duongDan);
    vector<HopDongLaoDong> kq;
    string dong;
    int soDong = 0;

    while (getline(f, dong)) {
        soDong++;
        if (boQua(dong)) continue;
        vector<string> t = tach(dong, '|');
        kiemTraSoTruong(t, 5, soDong);

        shared_ptr<NhanVien> nv = BoLoc::timTheoMa(nhanVien, t[4]);
        if (nv == nullptr) {
            throw LoiKhongTimThay(loiDong(soDong, "khong tim thay nhan vien ma " + t[4] + " cho hop dong " + t[0]));
        }
        kq.push_back(HopDongLaoDong(t[0], t[1], t[2], docSo(t[3], soDong), nv));
    }
    return kq;
}

void FileManager::luuPhongBan(const vector<PhongBan>& ds, const string& duongDan) {
    ofstream f;
    moGhi(f, duongDan);
    for (size_t i = 0; i < ds.size(); i++) {
        f << lamSach(ds[i].getMaPhongBan()) << "|" << lamSach(ds[i].getTenPhongBan()) << "|";
        const vector<shared_ptr<NhanVien> >& dsNV = ds[i].getDanhSachNhanVien();
        for (size_t j = 0; j < dsNV.size(); j++) {
            if (j > 0) f << ";";
            f << lamSach(dsNV[j]->getMaNV());
        }
        f << "\n";
    }
    if (!f) throw LoiFile("Loi khi ghi file: " + duongDan);
}

vector<PhongBan> FileManager::docPhongBan(const string& duongDan, const DanhSachNhanVien& nhanVien) {
    ifstream f;
    moDoc(f, duongDan);
    vector<PhongBan> kq;
    string dong;
    int soDong = 0;

    while (getline(f, dong)) {
        soDong++;
        if (boQua(dong)) continue;
        vector<string> t = tach(dong, '|');
        kiemTraSoTruong(t, 3, soDong);

        PhongBan pb(t[0], t[1]);
        if (!t[2].empty()) {
            vector<string> dsMa = tach(t[2], ';');
            for (size_t i = 0; i < dsMa.size(); i++) {
                shared_ptr<NhanVien> nv = BoLoc::timTheoMa(nhanVien, dsMa[i]);
                if (nv == nullptr) {
                    throw LoiKhongTimThay(loiDong(soDong, "khong tim thay nhan vien ma " + dsMa[i] + " cua phong ban " + t[0]));
                }
                ostringstream nuot;
                streambuf* cu = cout.rdbuf(nuot.rdbuf());
                pb.themNhanVien(nv);
                cout.rdbuf(cu);
            }
        }
        kq.push_back(pb);
    }
    return kq;
}

void FileManager::luuKPI(const vector<HeThongKPI>& ds, const string& duongDan) {
    ofstream f;
    moGhi(f, duongDan);
    for (size_t i = 0; i < ds.size(); i++) {
        f << lamSach(ds[i].getMaNV()) << "|" << soThuc(ds[i].getDiemKPI(), 17) << "|"
          << lamSach(ds[i].getThang()) << "|" << lamSach(ds[i].getNam()) << "\n";
    }
    if (!f) throw LoiFile("Loi khi ghi file: " + duongDan);
}

vector<HeThongKPI> FileManager::docKPI(const string& duongDan) {
    ifstream f;
    moDoc(f, duongDan);
    vector<HeThongKPI> kq;
    string dong;
    int soDong = 0;

    while (getline(f, dong)) {
        soDong++;
        if (boQua(dong)) continue;
        vector<string> t = tach(dong, '|');
        kiemTraSoTruong(t, 4, soDong);
        kq.push_back(HeThongKPI(t[0], docSo(t[1], soDong), t[2], t[3]));
    }
    return kq;
}

void FileManager::luuChamCong(const vector<ChamCong>& ds, const string& duongDan) {
    ofstream f;
    moGhi(f, duongDan);
    for (size_t i = 0; i < ds.size(); i++) {
        const ChamCong& c = ds[i];
        if (!c.kiemTraHopLe()) {
            throw LoiDuLieuKhongHopLe("Cham cong cua nhan vien '" + c.getMaNhanVien() + "' khong hop le, khong the luu");
        }
        f << lamSach(c.getMaNhanVien()) << "|" << c.getThang() << "|" << c.getNam() << "|"
          << c.getSoNgayCong() << "|" << c.getSoNgayNghi() << "|" << lamSach(c.getTrangThai()) << "\n";
    }
    if (!f) throw LoiFile("Loi khi ghi file: " + duongDan);
}

vector<ChamCong> FileManager::docChamCong(const string& duongDan) {
    ifstream f;
    moDoc(f, duongDan);
    vector<ChamCong> kq;
    string dong;
    int soDong = 0;

    while (getline(f, dong)) {
        soDong++;
        if (boQua(dong)) continue;
        vector<string> t = tach(dong, '|');
        kiemTraSoTruong(t, 6, soDong);

        int thang = docSoNguyen(t[1], soDong);
        int nam = docSoNguyen(t[2], soDong);
        int ngayCong = docSoNguyen(t[3], soDong);
        int ngayNghi = docSoNguyen(t[4], soDong);
        const string& trangThai = t[5];

        // Constructor ChamCong tu dong doi so am thanh 0 nen phai chan truoc
        if (ngayCong < 0 || ngayNghi < 0) {
            throw LoiFile(loiDong(soDong, "so ngay cong / ngay nghi khong duoc am"));
        }
        if (trangThai != "Dang cham cong" && trangThai != "Da chot cong") {
            throw LoiFile(loiDong(soDong, "trang thai cham cong khong hop le: '" + trangThai + "'"));
        }

        ChamCong c(t[0], thang, nam, ngayCong, ngayNghi);
        if (trangThai != "Dang cham cong") {
            TamTatCout im;
            c.setTrangThai(trangThai);
        }
        if (!c.kiemTraHopLe()) {
            throw LoiFile(loiDong(soDong, "du lieu cham cong khong hop le (ma NV, thang 1-12, nam 2000-2100, ngay cong + nghi <= so ngay cua thang)"));
        }
        kq.push_back(c);
    }
    return kq;
}

void FileManager::luuBangLuong(const vector<BangLuong>& ds, const string& duongDan) {
    ofstream f;
    moGhi(f, duongDan);
    for (size_t i = 0; i < ds.size(); i++) {
        const BangLuong& b = ds[i];
        f << lamSach(b.getMaNV()) << "|" << lamSach(b.getHoTen()) << "|" << lamSach(b.getLoaiNhanVien()) << "|"
          << lamSach(b.getThang()) << "|" << lamSach(b.getNam()) << "|"
          << soThuc(b.getLuongCoBan(), 17) << "|" << soThuc(b.getPhuCap(), 17) << "|"
          << soThuc(b.getThuongKPI(), 17) << "|" << soThuc(b.getKhoanBoSung(), 17) << "|"
          << soThuc(b.getBaoHiem(), 17) << "|" << soThuc(b.getThue(), 17) << "\n";
    }
    if (!f) throw LoiFile("Loi khi ghi file: " + duongDan);
}

vector<BangLuong> FileManager::docBangLuong(const string& duongDan) {
    ifstream f;
    moDoc(f, duongDan);
    vector<BangLuong> kq;
    string dong;
    int soDong = 0;

    while (getline(f, dong)) {
        soDong++;
        if (boQua(dong)) continue;
        vector<string> t = tach(dong, '|');
        kiemTraSoTruong(t, 11, soDong);
        kq.push_back(BangLuong(t[0], t[1], t[2], t[3], t[4],
                               docSo(t[5], soDong), docSo(t[6], soDong), docSo(t[7], soDong),
                               docSo(t[8], soDong), docSo(t[9], soDong), docSo(t[10], soDong)));
    }
    return kq;
}

