#ifndef LOINGHIEPVU_H
#define LOINGHIEPVU_H

#include <exception>
#include <string>

using namespace std;

class LoiNghiepVu : public exception {
protected:
    string thongBao;

public:
    explicit LoiNghiepVu(const string& thongBao);
    virtual ~LoiNghiepVu();

    const char* what() const noexcept override;
    string getThongBao() const;
    virtual string getLoaiLoi() const;
};

class LoiDuLieuKhongHopLe : public LoiNghiepVu {
public:
    explicit LoiDuLieuKhongHopLe(const string& thongBao);
    string getLoaiLoi() const override;
};

class LoiKhongTimThay : public LoiNghiepVu {
public:
    explicit LoiKhongTimThay(const string& thongBao);
    string getLoaiLoi() const override;
};

class LoiFile : public LoiNghiepVu {
public:
    explicit LoiFile(const string& thongBao);
    string getLoaiLoi() const override;
};

#endif
