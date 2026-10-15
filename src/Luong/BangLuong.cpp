#include "BangLuong.h"
#include <iomanip>
#include <cmath>
#include <sstream>
#include <vector>
#include <algorithm>

BangLuong::BangLuong() {
    luongCoBan = phuCap = thuongKPI = khoanBoSung = baoHiem = thue = 0;
}

BangLuong::BangLuong(string maNV, string hoTen, string loaiNhanVien,
                     string thang, string nam,
                     double luongCoBan, double phuCap, double thuongKPI,
                     double khoanBoSung, double baoHiem, double thue) {
    this->maNV = maNV;
    this->hoTen = hoTen;
    this->loaiNhanVien = loaiNhanVien;
    this->thang = thang;
    this->nam = nam;
    this->luongCoBan = luongCoBan;
    this->phuCap = phuCap;
    this->thuongKPI = thuongKPI;
    this->khoanBoSung = khoanBoSung;
    this->baoHiem = baoHiem;
    this->thue = thue;
}

string BangLuong::getMaNV() const { return maNV; }
string BangLuong::getHoTen() const { return hoTen; }
string BangLuong::getLoaiNhanVien() const { return loaiNhanVien; }
string BangLuong::getThang() const { return thang; }
string BangLuong::getNam() const { return nam; }
double BangLuong::getLuongCoBan() const { return luongCoBan; }
double BangLuong::getPhuCap() const { return phuCap; }
double BangLuong::getThuongKPI() const { return thuongKPI; }
double BangLuong::getKhoanBoSung() const { return khoanBoSung; }
double BangLuong::getBaoHiem() const { return baoHiem; }
double BangLuong::getThue() const { return thue; }

double BangLuong::tongThuNhap() const {
    return luongCoBan + phuCap + thuongKPI + khoanBoSung;
}

double BangLuong::luongThucLinh() const {
    return tongThuNhap() - baoHiem - thue;
}

static string dinhDangTien(double v) {
    long long n = llround(v);
    bool am = n < 0;
    if (am) n = -n;
    string s = to_string(n), kq;
    int dem = 0;
    for (int i = (int)s.size() - 1; i >= 0; i--) {
        kq.insert(kq.begin(), s[i]);
        if (++dem % 3 == 0 && i > 0) kq.insert(kq.begin(), '.');
    }
    return am ? "-" + kq : kq;
}

string dinhDangTienVND(double v) {
    return dinhDangTien(v);
}

static string docBaSo(int n, bool dayDu) {
    static const char* so[] = {"khong", "mot", "hai", "ba", "bon", "nam", "sau", "bay", "tam", "chin"};
    int tram = n / 100, chuc = (n / 10) % 10, dv = n % 10;
    string s;
    if (tram > 0 || dayDu) s += string(so[tram]) + " tram";
    if (chuc > 1) {
        s += string(s.empty() ? "" : " ") + so[chuc] + " muoi";
        if (dv == 1) s += " mot";
        else if (dv == 5) s += " lam";
        else if (dv > 0) s += string(" ") + so[dv];
    } else if (chuc == 1) {
        s += string(s.empty() ? "" : " ") + "muoi";
        if (dv == 5) s += " lam";
        else if (dv > 0) s += string(" ") + so[dv];
    } else if (dv > 0) {
        if (tram > 0 || dayDu) s += string(" linh ") + so[dv];
        else s += so[dv];
    }
    return s;
}

static string docSoThanhChu(double v) {
    long long n = llround(v);
    if (n == 0) return "Khong dong.";
    bool am = n < 0;
    if (am) n = -n;
    static const char* donVi[] = {"", " nghin", " trieu", " ty"};
    int nhom[4];
    for (int i = 0; i < 4; i++) { nhom[i] = (int)(n % 1000); n /= 1000; }
    // n con lai (> 999 ty) bo qua - vuot qua pham vi luong thang
    string kq;
    bool daIn = false;
    for (int i = 3; i >= 0; i--) {
        if (nhom[i] == 0) continue;
        if (daIn) kq += " ";
        kq += docBaSo(nhom[i], daIn) + donVi[i];
        daIn = true;
    }
    if (am) kq = "am " + kq;
    kq[0] = (char)toupper((unsigned char)kq[0]);
    return kq + " dong chan.";
}

static vector<string> ngatDong(const string& s, size_t rong) {
    vector<string> dong;
    istringstream in(s);
    string tu, cur;
    while (in >> tu) {
        if (!cur.empty() && cur.size() + 1 + tu.size() > rong) { dong.push_back(cur); cur.clear(); }
        cur += (cur.empty() ? "" : " ") + tu;
    }
    if (!cur.empty()) dong.push_back(cur);
    return dong;
}

static string dinhDangLoaiNhanVien(const string& loai) {
    string kq = loai;
    if (kq.empty()) return "Khac";

    string lower = kq;
    transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char ch) {
        return static_cast<char>(tolower(ch));
    });

    if (lower.find("toan thoi gian") != string::npos || lower.find("toanthoigian") != string::npos) return "Toan thoi gian";
    if (lower.find("ban thoi gian") != string::npos || lower.find("banthoigian") != string::npos) return "Ban thoi gian";
    if (lower.find("quan ly") != string::npos || lower.find("quanly") != string::npos) return "Quan ly";
    if (lower.find("chuyen gia") != string::npos || lower.find("chuyengia") != string::npos) return "Chuyen gia";

    return kq;
}

static string canTrai(const string& s, size_t w) { return s.size() >= w ? s.substr(0, w) : s + string(w - s.size(), ' '); }
static string canPhai(const string& s, size_t w) { return s.size() >= w ? s.substr(0, w) : string(w - s.size(), ' ') + s; }
static string canGiua(const string& s, size_t w) {
    if (s.size() >= w) return s.substr(0, w);
    size_t t = (w - s.size()) / 2;
    return string(t, ' ') + s + string(w - s.size() - t, ' ');
}

void BangLuong::xuat() const {
    const size_t W1 = 6, W2 = 36, W3 = 24, TONG = W1 + W2 + W3 + 2;   // TONG = do rong giua 2 vach ngoai

    const string dongNgoai = "+" + string(TONG, '-') + "+\n";
    const string dongDoi   = "+" + string(TONG, '=') + "+\n";
    const string dongBang  = "+" + string(W1, '-') + "+" + string(W2, '-') + "+" + string(W3, '-') + "+\n";
    const string dongBangD = "+" + string(W1, '=') + "+" + string(W2, '=') + "+" + string(W3, '=') + "+\n";

    auto dongChu = [&](const string& s) {
        cout << "|" << canGiua(s, TONG) << "|\n";
    };

    auto dongTrai = [&](const string& s) {
        cout << "| " << canTrai(s, TONG - 2) << " |\n";
    };

    auto dongHang = [&](const string& stt, const string& muc, const string& tien) {
        cout << "|" << canGiua(stt, W1) << "| "
             << canTrai(muc, W2 - 2) << " | "
             << canPhai(tien, W3 - 2) << " |\n";
    };

    auto dongHai = [&](const string& trai, const string& phai) {
        cout << "| " << canTrai(trai, W1 + W2) << "| "
             << canTrai(phai, W3 - 2) << " |\n";
    };

    double tongTN = tongThuNhap();
    double tongKT = baoHiem + thue;
    double luongTL = luongThucLinh();

    cout << "\n" << dongDoi;
    dongChu("PHIEU LUONG");
    dongChu("THANG " + thang + "/" + nam);
    cout << dongDoi;
    cout << dongNgoai;

    dongHai("Ma NV: " + maNV, "Ky luong: " + thang + "/" + nam);
    dongTrai("Ho va ten: " + hoTen);
    dongTrai("Loai nhan vien: " + loaiNhanVien);
    cout << dongBangD;

    cout << "|" << canGiua("STT", W1) << "|" << canGiua("KHOAN MUC", W2) << "|"
         << canGiua("SO TIEN (VND)", W3) << "|\n";
    cout << dongBangD;

    dongHang("A", "CAC KHOAN THU NHAP", "");
    cout << dongBang;
    dongHang("1", "Luong co ban", dinhDangTien(luongCoBan));
    dongHang("2", "Phu cap", dinhDangTien(phuCap));
    dongHang("3", "Thuong KPI", dinhDangTien(thuongKPI));
    dongHang("4", "Khoan bo sung", dinhDangTien(khoanBoSung));
    cout << dongBang;
    dongHang("", "Tong thu nhap (A)", dinhDangTien(tongTN));
    cout << dongBang;

    dongHang("B", "CAC KHOAN KHAU TRU", "");
    cout << dongBang;
    dongHang("1", "Bao hiem", dinhDangTien(baoHiem));
    dongHang("2", "Thue thu nhap ca nhan", dinhDangTien(thue));
    cout << dongBang;
    dongHang("", "Tong khau tru (B)", dinhDangTien(tongKT));
    cout << dongBangD;

    dongHang("", "THUC LINH (A - B)", dinhDangTien(luongTL));
    cout << dongBangD;

    vector<string> chu = ngatDong(docSoThanhChu(luongTL), TONG - 2 - 10);
    for (size_t i = 0; i < chu.size(); i++) {
        string prefix = (i == 0 ? "Bang chu: " : "          ");
        dongTrai(prefix + chu[i]);
    }

    cout << dongNgoai << endl;
}

void BangLuong::xuatTongHop(const vector<BangLuong>& dsLuong) {
    const int wMa = 12;
    const int wTen = 24;
    const int wLoai = 20;
    const int wThang = 12;
    const int wThuc = 20;

    cout << "\n" << string(96, '=') << "\n";
    cout << left
         << setw(wMa) << "Ma NV"
         << setw(wTen) << "Ho ten"
         << setw(wLoai) << "Loai NV"
         << setw(wThang) << "Thang"
         << setw(wThuc) << "Thuc linh (VND)"
         << "\n";
    cout << string(96, '-') << "\n";

    for (const auto& x : dsLuong) {
        string loai = dinhDangLoaiNhanVien(x.getLoaiNhanVien());

        cout << left
             << setw(wMa) << x.getMaNV()
             << setw(wTen) << x.getHoTen()
             << setw(wLoai) << loai
             << setw(wThang) << (x.getThang() + "/" + x.getNam())
             << setw(wThuc) << dinhDangTien(x.luongThucLinh())
             << "\n";
    }

    cout << string(96, '=') << "\n";
}

string BangLuong::xuatHTML() const {
    auto hang = [](const string& ten, double v, bool dam = false) {
        string a = dam ? "<b>" : "", b = dam ? "</b>" : "";
        return "<tr><td style='padding:5px 10px'>" + a + ten + b +
               "</td><td align='right' style='padding:5px 10px'>" + a + dinhDangTien(v) + b + "</td></tr>";
    };
    auto muc = [](const string& ten) {
        return "<tr><td colspan='2' bgcolor='#EEF2FF' style='padding:5px 10px; color:#1E3A8A'><b>" + ten + "</b></td></tr>";
    };
    string h;
    h += "<div style='font-family:Segoe UI,Arial; width:460px; border:1px solid #CBD5E1; padding:12px'>";
    h += "<div align='center' style='font-size:17px'><b>PHIEU LUONG THANG " + thang + "/" + nam + "</b></div><br>";
    h += "<table width='100%' cellspacing='0'>"
         "<tr><td>Ma nhan vien:</td><td><b>" + maNV + "</b></td><td>Loai:</td><td><b>" + loaiNhanVien + "</b></td></tr>"
         "<tr><td>Ho va ten:</td><td colspan='3'><b>" + hoTen + "</b></td></tr></table><br>";
    h += "<table width='100%' cellspacing='0' border='1' style='border-color:#CBD5E1'>";
    h += "<tr bgcolor='#1E3A8A'><td style='padding:5px 10px;color:white'><b>KHOAN MUC</b></td>"
         "<td align='right' style='padding:5px 10px;color:white'><b>SO TIEN (VND)</b></td></tr>";
    h += muc("A. CAC KHOAN THU NHAP");
    h += hang("Luong co ban", luongCoBan) + hang("Phu cap", phuCap) +
         hang("Thuong KPI", thuongKPI) + hang("Khoan bo sung", khoanBoSung);
    h += hang("Tong thu nhap (A)", tongThuNhap(), true);
    h += muc("B. CAC KHOAN KHAU TRU");
    h += hang("Bao hiem (BHXH, BHYT, BHTN)", baoHiem) + hang("Thue thu nhap ca nhan", thue);
    h += hang("Tong khau tru (B)", baoHiem + thue, true);
    h += "<tr bgcolor='#DCFCE7'><td style='padding:7px 10px'><b>THUC LINH (A - B)</b></td>"
         "<td align='right' style='padding:7px 10px;color:#15803D;font-size:15px'><b>" +
         dinhDangTien(luongThucLinh()) + "</b></td></tr></table>";
    h += "<p><i>Bang chu: " + docSoThanhChu(luongThucLinh()) + "</i></p>";
    h += "<table width='100%'><tr><td align='center'><b>Nguoi lap phieu</b><br><small>(Ky, ghi ro ho ten)</small></td>"
         "<td align='center'><b>Nguoi nhan luong</b><br><small>(Ky, ghi ro ho ten)</small></td></tr></table></div>";
    return h;
}
