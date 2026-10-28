# HỆ THỐNG QUẢN LÝ NHÂN SỰ (HRMS) - GIAO DIỆN WEB & C++ OOP ENGINE

Hệ thống Quản lý Nhân sự hiện đại theo phong cách **SaaS Dashboard**, chạy trực tiếp trên nền tảng Web và kết nối trực tiếp vào toàn bộ các lớp nghiệp vụ hướng đối tượng C++17 (Đa hình, Kế thừa, Strategy Pattern, Template Method, Factory Pattern, Exception Handling) **mà không sửa đổi bất kỳ tệp tin nào trong `Include/` và `src/`**.

---

## 1. YÊU CẦU HỆ THỐNG

- **Trình biên dịch C++:** Hỗ trợ C++17 trở lên.
  - **Windows:** MinGW GCC/g++ ≥ 9.0 (hoặc MSVC 2019/2022)
  - **Linux / macOS:** GCC ≥ 9.0 hoặc Clang ≥ 10.0
- **Trình duyệt Web:** Chrome, Edge, Firefox, Safari hiện đại.
- **Không yêu cầu cài đặt Node.js hay npm** (Frontend chạy HTML/CSS/JS thuần, Backend C++ tự phục vụ HTTP tĩnh và REST API).

---

## 2. HƯỚNG DẪN BUILD VÀ CHẠY (3 BƯỚC)

### Cách 1: Khởi động 1 chạm bằng Script (Khuyên dùng)
- **Trên Windows:**
  Nhấp đúp chuột vào file `run.bat` (hoặc mở PowerShell/CMD và gõ `./run.bat`).
  Server sẽ tự động biên dịch (nếu chưa có file thực thi), khởi động máy chủ tại cổng `8080` và tự động mở trình duyệt đến `http://localhost:8080`.
- **Trên Linux / macOS:**
  ```bash
  chmod +x run.sh
  ./run.sh
  ```

---

### Cách 2: Sử dụng Makefile
```bash
# Biên dịch
mingw32-make    # Windows MinGW
make            # Linux/macOS

# Khởi chạy
./server_app 8080
```

---

### Cách 3: Sử dụng CMake
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release

# Chạy
./server_app 8080
```

---

## 3. SƠ ĐỒ KIẾN TRÚC HỆ THỐNG

```
                             [ WEB BROWSER / CLIENT ]
                                        │
           HTTP GET / POST / PUT / DELETE (JSON) & Static Assets
                                        │
                                        ▼
                      ┌────────────────────────────────────┐
                      │    C++17 HTTP REST SERVER          │
                      │    (cpp-httplib + nlohmann/json)   │
                      │    Port: 8080                      │
                      └─────────────────┬──────────────────┘
                                        │
                     ┌──────────────────┴──────────────────┐
                     │                                     │
                     ▼                                     ▼
        ┌─────────────────────────┐           ┌─────────────────────────┐
        │     ADAPTER LAYER       │           │     STORAGE LAYER       │
        │ (web/server/adapters)   │           │ (web/server/storage)    │
        │                         │           │ Lưu trữ bền vững        │
        │ - Stream Buffering      │           │ web/data/*.json         │
        │ - C++ <-> JSON Adapter  │           │ Tự khôi phục dữ liệu    │
        └────────────┬────────────┘           └─────────────────────────┘
                     │
    ┌────────────────┴────────────────────────────────────────────────┐
    │                CORE C++ BUSINESS LOGIC (NGUYÊN BẢN)             │
    │                Include/ và src/ (KHÔNG SỬA ĐỔI)                 │
    ├─────────────────────────────────────────────────────────────────┤
    │ • NhanVien (ToanThoiGian, BanThoiGian, QuanLy, ChuyenGia)       │
    │ • PhongBan (Tập hợp nhân sự, tìm kiếm, gỡ, thêm đa hình)        │
    │ • HopDongLaoDong (Liên kết người lao động, mức lương)           │
    │ • ChamCong (Ngày công, ngày nghỉ, trạng thái chốt công)         │
    │ • HeThongKPI + IChinhSachThuong (Strategy Pattern tính thưởng)  │
    │ • QuyTrinhTinhLuong + LuongFactory (Template Method & Factory)  │
    │ • BangLuong (Bảng lương chi tiết, xuatHTML, format tiền VND)    │
    │ • ThongKe (Báo cáo tổng hợp ra cout, lưu vết ngoại lệ)          │
    │ • LoiNghiepVu (LoiChamCong, LoiDuLieuKhongHopLe,...)            │
    └─────────────────────────────────────────────────────────────────┘
```

---

## 4. DANH SÁCH REST API CHUẨN

Tất cả các API trả về JSON chuẩn: `{ "success": boolean, "data": any, "message": string }`.

| Phương thức | Đường dẫn API | Mô tả nghiệp vụ C++ |
|---|---|---|
| `GET` | `/api/employees` | Lấy danh sách nhân viên (hỗ trợ lọc theo loại, phòng ban, trạng thái, tìm kiếm). |
| `GET` | `/api/employees/:id` | Xem chi tiết 1 nhân viên với các trường đặc thù của lớp con. |
| `POST` | `/api/employees` | Tạo nhân viên mới. Kiểm tra hợp lệ qua `kiemTraHopLe()`, `kiemTraThongTinRieng()`. |
| `PUT` | `/api/employees/:id` | Cập nhật thông tin nhân viên và điều chuyển phòng ban. |
| `DELETE` | `/api/employees/:id` | Xóa nhân viên, tự động gỡ khỏi phòng ban (`pb.xoaNhanVien`). |
| `GET` | `/api/departments` | Lấy danh sách phòng ban và số lượng nhân sự. |
| `POST` | `/api/departments` | Tạo phòng ban mới (`PhongBan::kiemTraHopLe`). |
| `POST` | `/api/departments/:id/members` | Thêm nhân viên vào phòng ban (`pb.themNhanVien`). |
| `DELETE`| `/api/departments/:id/members/:empId` | Gỡ nhân viên khỏi phòng ban (`pb.xoaNhanVien`). |
| `GET` | `/api/contracts` | Lấy danh sách hợp đồng, kiểm tra tình trạng hiệu lực / sắp hết hạn. |
| `POST` | `/api/contracts` | Tạo hợp đồng mới liên kết `shared_ptr<NhanVien>` (`hd.kiemTraHopLe`). |
| `DELETE`| `/api/contracts/:id` | Xóa hợp đồng lao động. |
| `GET` | `/api/attendance` | Lấy bản ghi chấm công theo tháng/năm. |
| `POST` | `/api/attendance` | Cập nhật ngày công / ngày nghỉ (`capNhatSoNgayCong`, `capNhatSoNgayNghi`). |
| `POST` | `/api/attendance/:id/lock` | Chốt công tháng (`cc.setTrangThai("Da chot cong")`). |
| `GET` | `/api/kpi` | Lấy điểm KPI và tính thưởng tự động qua `kpi.tinhTienThuong()`. |
| `POST` | `/api/kpi` | Lưu/chấm điểm KPI, gán chính sách qua `LuongFactory::taoChinhSachThuong`. |
| `GET` | `/api/payroll` | Xem danh sách bảng lương đã tính. |
| `POST` | `/api/payroll/calculate` | Kích hoạt tính lương toàn diện qua `QuyTrinhTinhLuong::tinhLuong`, bắt `LoiNghiepVu`. |
| `GET` | `/api/payroll/:empId/slip`| Lấy phiếu lương HTML chuẩn sinh bởi `BangLuong::xuatHTML()`. |
| `GET` | `/api/stats/summary` | Thống kê số lượng theo loại, phòng ban, quỹ lương từ các đối tượng C++. |
| `GET` | `/api/stats/report-raw`| Bắt trực tiếp luồng `std::cout` của `ThongKe::baoCaoTongHop`. |
| `GET` | `/api/stats/errors` | Lấy số lượng và nhật ký ngoại lệ do `ThongKe::ghiNhanLoi` quản lý. |
| `POST` | `/api/data/reset-sample` | Nạp lại dữ liệu mẫu gốc từ `main_test.cpp`. |
| `GET` | `/api/data/export-json` | Xuất file JSON sao lưu hệ thống. |
| `POST` | `/api/data/import-json` | Phục hồi toàn bộ dữ liệu từ file JSON. |
| `GET` | `/api/data/export-csv` | Xuất dữ liệu nhân sự / bảng lương ra CSV chuẩn UTF-8 BOM. |

---

## 5. HƯỚNG DẪN THÊM MỘT TRANG / CHỨC NĂNG MỚI

1. **Thêm Menu Item vào Sidebar trong `public/index.html`:**
   ```html
   <li class="nav-item">
     <a class="nav-link" data-tab="ten-trang">
       <i data-lucide="icon-name"></i>
       <span class="nav-text">Tên Trang Mới</span>
     </a>
   </li>
   ```
2. **Thêm Section giao diện trong `<main class="content-body">`:**
   ```html
   <section class="page-section" id="sec-ten-trang">
     <div class="section-header">
       <div class="section-title">
         <h1>Tiêu Đề Trang</h1>
         <p>Mô tả chức năng</p>
       </div>
     </div>
     <!-- Nội dung bảng/thẻ/form -->
   </section>
   ```
3. **Thêm hàm xử lý tải dữ liệu trong `public/js/app.js`:**
   Trong switchTab(tabName):
   ```javascript
   else if (tabName === 'ten-trang') loadTenTrang();
   ```
4. **Đăng ký Endpoint C++ tương ứng trong `server/main_server.cpp` (nếu cần dữ liệu mới):**
   ```cpp
   svr.Get("/api/ten-trang", [&](const httplib::Request& req, httplib::Response& res) {
       jsonResponse(res, 200, true, myData);
   });
   ```

---

## 6. DANH SÁCH CHỨC NĂNG ĐÃ LÀM & GIẢ ĐỊNH THIẾT KẾ

### Chức năng đã hoàn thành 100% với Code C++:
1. **Dashboard tổng quan:** Thẻ số liệu, biểu đồ Chart.js tự động cập nhật, danh sách nhân sự mới nhất.
2. **Nhân viên (Đa hình):** Tìm kiếm tức thì, lọc theo loại/phòng ban/trạng thái, thêm/sửa/xóa qua modal, validate bằng chính `kiemTraHopLe()` và `kiemTraThongTinRieng()` của C++.
3. **Phòng ban:** Tạo, xóa phòng ban, gán trưởng phòng, xem thành viên, thêm/gỡ nhân viên theo quan hệ tập hợp (Aggregation).
4. **Hợp đồng lao động:** Quản lý hợp đồng liên kết với nhân sự, tính hợp lệ, theo dõi mức lương thỏa thuận.
5. **Chấm công:** Xem theo kỳ tháng/năm, kiểm tra hợp lệ ngày công và ngày nghỉ, chốt công bằng C++ (`setTrangThai("Da chot cong")`).
6. **KPI & Thưởng:** Chấm điểm, tính tiền thưởng đa hình qua Strategy Pattern (`ThuongNhanVien`, `ThuongQuanLy`, `ThuongChuyenGia`).
7. **Bảng lương:** Tính lương theo quy trình đa hình `LuongFactory::taoQuyTrinh`, xuất phiếu lương HTML (`xuatHTML()`), in ấn và xuất CSV.
8. **Thống kê & Ngoại lệ:** Chụp trực tiếp đầu ra của `ThongKe::baoCaoTongHop`, hiển thị nhật ký lỗi bắt được từ `LoiNghiepVu`.
9. **Quản lý Dữ liệu:** Lưu trữ tự động ra file JSON, nút khôi phục dữ liệu chuẩn `main_test.cpp`, nhập/xuất backup JSON/CSV.
10. **UI/UX hiện đại:** 2 giao diện Sáng/Tối mượt mà (WCAG AA), responsive cho Mobile/Tablet/Desktop, phím tắt `/` và `Esc`.

### Chức năng mở rộng & Giả định thiết kế (do C++ nguyên bản chưa có):
- **Cảnh báo ngày hết hạn hợp đồng:** Trong lớp `HopDongLaoDong` của C++ chỉ có `ngayKyKet`, chưa có trường ngày hết hạn. Adapter đã bổ sung trường `ngayHetHan` ở tầng lưu trữ để giao diện web hiển thị cảnh báo hợp đồng sắp hết hạn trong 30 ngày mà **không cần can thiệp sửa file C++**.
- **Lưới lịch chấm công:** Lớp `ChamCong` của C++ lưu trữ tổng số ngày công và ngày nghỉ của tháng. Giao diện cung cấp lưới lịch trực quan 31 ngày để người dùng thao tác bấm chọn ngày làm/nghỉ, sau đó tổng kết và gọi vào hàm `capNhatSoNgayCong()` / `capNhatSoNgayNghi()` của C++.
