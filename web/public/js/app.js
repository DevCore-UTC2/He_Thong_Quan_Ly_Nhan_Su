/**
 * HE THONG QUAN LY NHAN SU - FRONTEND APP ENGINE
 * Giao tiếp trực tiếp với Backend REST API C++
 */

const App = (() => {
  // State
  let state = {
    currentTab: 'dashboard',
    employees: [],
    departments: [],
    contracts: [],
    attendance: [],
    kpi: [],
    payroll: [],
    stats: {},
    currentEditEmpId: null,
    currentSelectedDept: null,
    currentAttendanceData: null,
    deleteCallback: null,
    chartTypeInstance: null,
    chartDeptInstance: null
  };

  const API_BASE = '/api';

  // ============================================================
  // FORMATTERS & UTILITIES
  // ============================================================
  const formatVND = (num) => {
    if (num === null || num === undefined || isNaN(num)) return '0 ₫';
    return Math.round(num).toLocaleString('vi-VN') + ' ₫';
  };

  const showToast = (message, type = 'info', title = '') => {
    const container = document.getElementById('toast-container');
    if (!container) return;

    const toast = document.createElement('div');
    toast.className = `toast toast-${type}`;
    
    let defaultTitle = 'Thông báo';
    let iconName = 'info';
    if (type === 'success') { defaultTitle = 'Thành công'; iconName = 'check-circle'; }
    else if (type === 'danger') { defaultTitle = 'Lỗi Nghiệp vụ C++'; iconName = 'alert-triangle'; }
    else if (type === 'warning') { defaultTitle = 'Cảnh báo'; iconName = 'alert-circle'; }

    toast.innerHTML = `
      <i data-lucide="${iconName}" style="width: 20px; height: 20px; flex-shrink: 0; color: var(--${type === 'danger' ? 'danger' : (type === 'success' ? 'success' : 'primary')})"></i>
      <div class="toast-content">
        <div class="toast-title">${title || defaultTitle}</div>
        <div class="toast-msg">${message}</div>
      </div>
    `;

    container.appendChild(toast);
    if (window.lucide) lucide.createIcons({ root: toast });

    setTimeout(() => {
      toast.style.opacity = '0';
      toast.style.transform = 'translateX(100%)';
      toast.style.transition = 'all 0.3s ease';
      setTimeout(() => toast.remove(), 300);
    }, 4500);
  };

  // ============================================================
  // THEME MANAGEMENT (Light / Dark)
  // ============================================================
  const initTheme = () => {
    let savedTheme = 'light';
    try {
      savedTheme = localStorage.getItem('hrms-theme');
    } catch (e) {}

    if (!savedTheme) {
      if (window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches) {
        savedTheme = 'dark';
      }
    }
    setTheme(savedTheme || 'light');

    document.getElementById('theme-toggle-btn').addEventListener('click', () => {
      const current = document.documentElement.getAttribute('data-theme') || 'light';
      setTheme(current === 'light' ? 'dark' : 'light');
    });
  };

  const setTheme = (theme) => {
    document.documentElement.setAttribute('data-theme', theme);
    try {
      localStorage.setItem('hrms-theme', theme);
    } catch (e) {}

    const icon = document.getElementById('theme-icon');
    if (icon) {
      icon.setAttribute('data-lucide', theme === 'dark' ? 'sun' : 'moon');
      if (window.lucide) lucide.createIcons({ root: document.getElementById('theme-toggle-btn') });
    }

    // Refresh charts colors if rendered
    renderDashboardCharts();
  };

  // ============================================================
  // REALTIME CLOCK & SHORTCUTS
  // ============================================================
  const initClock = () => {
    const updateTime = () => {
      const now = new Date();
      const timeStr = now.toLocaleTimeString('vi-VN', { hour: '2-digit', minute: '2-digit', second: '2-digit' });
      const dateStr = now.toLocaleDateString('vi-VN', { weekday: 'short', day: '2-digit', month: '2-digit', year: 'numeric' });
      const el = document.getElementById('realtime-clock');
      if (el) el.textContent = `${timeStr} - ${dateStr}`;
    };
    updateTime();
    setInterval(updateTime, 1000);
  };

  const initShortcuts = () => {
    window.addEventListener('keydown', (e) => {
      if (e.key === '/' && document.activeElement.tagName !== 'INPUT' && document.activeElement.tagName !== 'TEXTAREA') {
        e.preventDefault();
        const searchInput = document.getElementById('global-search-input');
        if (searchInput) searchInput.focus();
      }
      if (e.key === 'Escape') {
        closeAllModals();
      }
    });

    const searchInput = document.getElementById('global-search-input');
    if (searchInput) {
      searchInput.addEventListener('input', (e) => {
        const query = e.target.value.toLowerCase().trim();
        switchTab('employees');
        const filterSearch = document.getElementById('filter-emp-search');
        if (filterSearch) {
          filterSearch.value = query;
          loadEmployees();
        }
      });
    }

    const toggleSidebarBtn = document.getElementById('toggle-sidebar-btn');
    if (toggleSidebarBtn) {
      toggleSidebarBtn.addEventListener('click', () => {
        const sidebar = document.getElementById('sidebar');
        const main = document.getElementById('main-wrapper');
        sidebar.classList.toggle('collapsed');
        main.classList.toggle('expanded');
      });
    }

    document.getElementById('btn-quick-reset').addEventListener('click', () => {
      resetSampleData();
    });
  };

  // ============================================================
  // TAB NAVIGATION
  // ============================================================
  const initNavigation = () => {
    document.querySelectorAll('.nav-link').forEach(link => {
      link.addEventListener('click', (e) => {
        e.preventDefault();
        const tab = link.getAttribute('data-tab');
        switchTab(tab);
      });
    });
  };

  const switchTab = (tabName) => {
    state.currentTab = tabName;
    document.querySelectorAll('.nav-link').forEach(link => {
      link.classList.toggle('active', link.getAttribute('data-tab') === tabName);
    });

    document.querySelectorAll('.page-section').forEach(sec => {
      sec.classList.remove('active');
    });

    const activeSec = document.getElementById(`sec-${tabName}`);
    if (activeSec) activeSec.classList.add('active');

    // Load data for specific tab
    if (tabName === 'dashboard') loadDashboard();
    else if (tabName === 'employees') loadEmployees();
    else if (tabName === 'departments') loadDepartments();
    else if (tabName === 'contracts') loadContracts();
    else if (tabName === 'attendance') loadAttendance();
    else if (tabName === 'kpi') loadKPI();
    else if (tabName === 'payroll') loadPayroll();
    else if (tabName === 'reports') loadRawReport();

    if (window.lucide) lucide.createIcons();
  };

  // ============================================================
  // MODAL HELPERS
  // ============================================================
  const openModal = (id) => {
    const modal = document.getElementById(id);
    if (modal) {
      modal.classList.add('open');
      if (window.lucide) lucide.createIcons({ root: modal });
    }
  };

  const closeModal = (id) => {
    const modal = document.getElementById(id);
    if (modal) modal.classList.remove('open');
  };

  const closeAllModals = () => {
    document.querySelectorAll('.modal-overlay').forEach(m => m.classList.remove('open'));
  };

  const confirmDelete = (message, onConfirm) => {
    document.getElementById('confirm-delete-msg').textContent = message;
    state.deleteCallback = onConfirm;
    openModal('modal-confirm-delete');
  };

  document.getElementById('btn-confirm-delete-action').addEventListener('click', () => {
    if (state.deleteCallback) {
      state.deleteCallback();
      state.deleteCallback = null;
    }
    closeModal('modal-confirm-delete');
  });

  // ============================================================
  // 1. DASHBOARD
  // ============================================================
  const loadDashboard = async () => {
    try {
      const res = await fetch(`${API_BASE}/stats/summary`);
      const jsonRes = await res.json();
      if (jsonRes.success) {
        state.stats = jsonRes.data;
        document.getElementById('kpi-total-emp').textContent = state.stats.tongNhanVien || 0;
        document.getElementById('kpi-total-salary').textContent = formatVND(state.stats.tongQuyLuong || 0);
        document.getElementById('kpi-avg-score').textContent = (state.stats.kpiTrungBinh || 0).toFixed(1);
        document.getElementById('kpi-total-depts').textContent = state.stats.tongPhongBan || 0;
        document.getElementById('badge-emp-count').textContent = state.stats.tongNhanVien || 0;

        renderDashboardCharts();
      }

      // Load recent employees
      const empRes = await fetch(`${API_BASE}/employees`);
      const empJson = await empRes.json();
      if (empJson.success) {
        state.employees = empJson.data;
        renderDashboardRecentEmployees();
      }
    } catch (e) {
      showToast('Không thể kết nối đến máy chủ C++', 'danger');
    }
  };

  const renderDashboardCharts = () => {
    const isDark = (document.documentElement.getAttribute('data-theme') || 'light') === 'dark';
    const textColor = isDark ? '#cbd5e1' : '#475569';
    const gridColor = isDark ? '#1f293d' : '#e2e8f0';

    // Chart 1: Employee types
    const ctxType = document.getElementById('chart-employee-type');
    if (ctxType) {
      if (state.chartTypeInstance) state.chartTypeInstance.destroy();
      const types = state.stats.theoLoai || {};
      state.chartTypeInstance = new Chart(ctxType, {
        type: 'doughnut',
        data: {
          labels: ['Toàn thời gian', 'Bán thời gian', 'Quản lý', 'Chuyên gia'],
          datasets: [{
            data: [
              types.ToanThoiGian || 0,
              types.BanThoiGian || 0,
              types.QuanLy || 0,
              types.ChuyenGia || 0
            ],
            backgroundColor: ['#4f46e5', '#38bdf8', '#f59e0b', '#10b981'],
            borderWidth: 0
          }]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          plugins: {
            legend: {
              position: 'right',
              labels: { color: textColor, font: { family: 'Be Vietnam Pro', size: 12 } }
            }
          }
        }
      });
    }

    // Chart 2: Departments
    const ctxDept = document.getElementById('chart-department-dist');
    if (ctxDept) {
      if (state.chartDeptInstance) state.chartDeptInstance.destroy();
      const depts = state.stats.theoPhongBan || [];
      state.chartDeptInstance = new Chart(ctxDept, {
        type: 'bar',
        data: {
          labels: depts.map(d => d.tenPhongBan),
          datasets: [{
            label: 'Nhân sự',
            data: depts.map(d => d.soLuong),
            backgroundColor: '#6366f1',
            borderRadius: 8
          }]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          scales: {
            x: { ticks: { color: textColor }, grid: { display: false } },
            y: { ticks: { color: textColor, stepSize: 1 }, grid: { color: gridColor } }
          },
          plugins: {
            legend: { display: false }
          }
        }
      });
    }
  };

  const renderDashboardRecentEmployees = () => {
    const tbody = document.getElementById('dashboard-recent-employees');
    if (!tbody) return;

    if (state.employees.length === 0) {
      tbody.innerHTML = `<tr><td colspan="6" class="text-center" style="padding: 30px; text-align: center; color: var(--text-muted)">Chưa có dữ liệu nhân viên. Nhấn "Thêm nhân viên" để bắt đầu.</td></tr>`;
      return;
    }

    tbody.innerHTML = state.employees.slice(0, 5).map(nv => {
      let badgeClass = 'badge-primary';
      if (nv.loaiId === 'BAN_THOI_GIAN') badgeClass = 'badge-info';
      else if (nv.loaiId === 'QUAN_LY') badgeClass = 'badge-warning';
      else if (nv.loaiId === 'CHUYEN_GIA') badgeClass = 'badge-success';

      return `
        <tr>
          <td><strong style="color: var(--primary)">${nv.maNV}</strong></td>
          <td><div style="font-weight: 600">${nv.hoTen}</div><small style="color:var(--text-muted)">${nv.email}</small></td>
          <td><span class="badge ${badgeClass}">${nv.loaiNhanVien}</span></td>
          <td>${nv.tenPhongBan || 'Chưa phân bổ'}</td>
          <td><span class="badge ${nv.trangThai === 'Dang lam' ? 'badge-success' : 'badge-danger'}">${nv.trangThai}</span></td>
          <td>
            <button class="btn btn-secondary btn-sm" onclick="App.openEditEmployeeModal('${nv.maNV}')">Chi tiết</button>
          </td>
        </tr>
      `;
    }).join('');
  };

  // ============================================================
  // 2. NHÂN VIÊN
  // ============================================================
  const loadEmployees = async () => {
    try {
      const q = document.getElementById('filter-emp-search').value.trim();
      const type = document.getElementById('filter-emp-type').value;
      const dept = document.getElementById('filter-emp-dept').value;
      const status = document.getElementById('filter-emp-status').value;

      const params = new URLSearchParams();
      if (q) params.append('q', q);
      if (type) params.append('type', type);
      if (dept) params.append('dept', dept);
      if (status) params.append('status', status);

      const res = await fetch(`${API_BASE}/employees?${params.toString()}`);
      const jsonRes = await res.json();
      if (jsonRes.success) {
        state.employees = jsonRes.data;
        renderEmployeesTable();
      }
    } catch (e) {
      showToast('Lỗi khi tải danh sách nhân viên', 'danger');
    }
  };

  const renderEmployeesTable = () => {
    const tbody = document.getElementById('employees-table-body');
    if (!tbody) return;

    if (state.employees.length === 0) {
      tbody.innerHTML = `
        <tr>
          <td colspan="8">
            <div class="empty-state">
              <div class="empty-icon"><i data-lucide="users"></i></div>
              <div class="empty-title">Không tìm thấy nhân sự phù hợp</div>
              <div class="empty-desc">Thử điều chỉnh bộ lọc hoặc thêm nhân viên mới vào hệ thống</div>
              <button class="btn btn-primary btn-sm" onclick="App.openAddEmployeeModal()">Thêm nhân viên</button>
            </div>
          </td>
        </tr>
      `;
      if (window.lucide) lucide.createIcons({ root: tbody });
      return;
    }

    tbody.innerHTML = state.employees.map(nv => {
      let badgeClass = 'badge-primary';
      let extraDetail = '';

      if (nv.loaiId === 'TOAN_THOI_GIAN') {
        badgeClass = 'badge-primary';
        extraDetail = `Công: ${nv.soNgayCong || 0} ngày | ${nv.caLamViec || 'Ca sang'}`;
      } else if (nv.loaiId === 'BAN_THOI_GIAN') {
        badgeClass = 'badge-info';
        extraDetail = `Lương giờ: ${formatVND(nv.luongTheoGio || 0)} (${nv.soGioLam || 0}h)`;
      } else if (nv.loaiId === 'QUAN_LY') {
        badgeClass = 'badge-warning';
        extraDetail = `Chức vụ: ${nv.chucVu || 'Quản lý'} (${nv.capQuanLy || 'Trung'})`;
      } else if (nv.loaiId === 'CHUYEN_GIA') {
        badgeClass = 'badge-success';
        extraDetail = `${nv.chuyenMon || 'Chuyên gia'} (${nv.soNamKinhNghiem || 0} năm KN)`;
      }

      return `
        <tr>
          <td><strong style="color: var(--primary)">${nv.maNV}</strong></td>
          <td>
            <div style="font-weight: 600">${nv.hoTen}</div>
            <small style="color: var(--text-muted)">Sinh ngày: ${nv.ngaySinh || '-'}</small>
          </td>
          <td>${nv.tenPhongBan || '<span style="color:var(--text-muted)">Chưa gán</span>'}</td>
          <td><span class="badge ${badgeClass}">${nv.loaiNhanVien}</span></td>
          <td>
            <div>${nv.soDienThoai || '-'}</div>
            <small style="color: var(--text-muted)">${nv.email || '-'}</small>
          </td>
          <td><small style="color: var(--text-secondary)">${extraDetail}</small></td>
          <td>
            <span class="badge ${nv.trangThai === 'Dang lam' ? 'badge-success' : 'badge-danger'}">
              ${nv.trangThai}
            </span>
          </td>
          <td style="text-align: right;">
            <div class="table-actions" style="justify-content: flex-end;">
              <button class="action-btn" title="Chỉnh sửa thông tin" onclick="App.openEditEmployeeModal('${nv.maNV}')">
                <i data-lucide="edit-2"></i>
              </button>
              <button class="action-btn delete" title="Xóa nhân viên" onclick="App.deleteEmployee('${nv.maNV}', '${nv.hoTen}')">
                <i data-lucide="trash-2"></i>
              </button>
            </div>
          </td>
        </tr>
      `;
    }).join('');

    if (window.lucide) lucide.createIcons({ root: tbody });
  };

  const populateDepartmentSelects = () => {
    const filterDept = document.getElementById('filter-emp-dept');
    const formDept = document.getElementById('emp-maPhongBan');

    let options = `<option value="">-- Tất cả phòng ban --</option>`;
    let formOptions = `<option value="">-- Chưa gán phòng ban --</option>`;

    state.departments.forEach(pb => {
      options += `<option value="${pb.maPhongBan}">${pb.tenPhongBan}</option>`;
      formOptions += `<option value="${pb.maPhongBan}">${pb.tenPhongBan} (${pb.maPhongBan})</option>`;
    });

    if (filterDept) filterDept.innerHTML = options;
    if (formDept) formDept.innerHTML = formOptions;
  };

  const selectEmployeeType = (type) => {
    if (state.currentEditEmpId) return; // Khong cho doi loai OOP khi dang sua
    const input = document.getElementById('emp-loaiId');
    if (input) input.value = type;

    document.querySelectorAll('.poly-card').forEach(card => {
      card.classList.toggle('active', card.getAttribute('data-type') === type);
    });

    onEmployeeTypeChanged();
  };

  const setShiftPreset = (shiftName) => {
    const input = document.getElementById('emp-caLamViec');
    if (input) input.value = shiftName;
  };

  const validateField = (fieldId) => {
    const el = document.getElementById(fieldId);
    if (!el) return true;
    const errEl = document.getElementById(`err-${fieldId}`);
    const val = el.value.trim();
    let errorMsg = '';
    let successMsg = '';

    if (fieldId === 'emp-maNV') {
      if (!val) {
        errorMsg = 'Mã nhân viên không được để trống';
      } else if (!state.currentEditEmpId) {
        const existed = state.employees.some(e => e.maNV.toUpperCase() === val.toUpperCase());
        if (existed) errorMsg = `Mã ${val} đã tồn tại trong hệ thống!`;
      }
    } else if (fieldId === 'emp-hoTen') {
      if (!val) {
        errorMsg = 'Họ và tên không được để trống';
      } else if (/\d/.test(val)) {
        errorMsg = 'Họ và tên không được chứa chữ số';
      } else if (val.length < 2) {
        errorMsg = 'Họ và tên quá ngắn (tối thiểu 2 ký tự)';
      }
    } else if (fieldId === 'emp-ngaySinh') {
      if (!val) {
        errorMsg = 'Vui lòng nhập ngày sinh theo định dạng DD/MM/YYYY';
      } else {
        const dateRegex = /^(0[1-9]|[12][0-9]|3[01])\/(0[1-9]|1[0-2])\/(\d{4})$/;
        const match = val.match(dateRegex);
        if (!match) {
          errorMsg = 'Định dạng ngày chưa đúng (yêu cầu DD/MM/YYYY, VD: 15/08/1995)';
        } else {
          const birthYear = parseInt(match[3]);
          const age = 2026 - birthYear;
          if (age < 18) {
            errorMsg = `Chưa đủ tuổi lao động (hiện tại ${age} tuổi, yêu cầu từ đủ 18 tuổi)`;
          } else if (age > 65) {
            errorMsg = `Vượt quá độ tuổi lao động tối đa (hiện tại ${age} tuổi, tối đa 65 tuổi)`;
          } else {
            successMsg = `✓ ${age} tuổi - Đủ tuổi lao động hợp pháp (18 - 65 tuổi)`;
          }
        }
      }
    } else if (fieldId === 'emp-soDienThoai') {
      const phoneRegex = /^0\d{9}$/;
      if (!val) {
        errorMsg = 'Số điện thoại không được để trống';
      } else if (!phoneRegex.test(val)) {
        errorMsg = 'Số điện thoại phải gồm đúng 10 chữ số và bắt đầu bằng số 0 (VD: 0912345678)';
      }
    } else if (fieldId === 'emp-email') {
      const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
      if (!val) {
        errorMsg = 'Email không được để trống';
      } else if (!emailRegex.test(val)) {
        errorMsg = 'Email không hợp lệ (VD: nam@cty.vn hoặc nhanvien@gmail.com)';
      }
    } else if (fieldId === 'emp-soNgayCong') {
      const num = parseInt(val);
      if (isNaN(num) || num < 0 || num > 31) {
        errorMsg = 'Số ngày công trong tháng phải từ 0 đến 31';
      }
    } else if (fieldId === 'emp-soGioLam') {
      const num = parseFloat(val);
      if (isNaN(num) || num < 0) {
        errorMsg = 'Số giờ làm không được âm';
      }
    } else if (fieldId === 'emp-bt-luongTheoGio') {
      const num = parseFloat(val);
      if (isNaN(num) || num <= 0) {
        errorMsg = 'Mức thù lao theo giờ phải lớn hơn 0';
      }
    } else if (fieldId === 'emp-bt-soGioLam') {
      const num = parseFloat(val);
      if (isNaN(num) || num < 0) {
        errorMsg = 'Số giờ làm không được âm';
      }
    }

    if (errorMsg) {
      el.classList.add('is-invalid');
      el.classList.remove('is-valid');
      if (errEl) {
        errEl.innerHTML = `<span class="err-text"><i data-lucide="alert-circle" style="width:13px;height:13px;display:inline-block;vertical-align:-2px"></i> ${errorMsg}</span>`;
        if (window.lucide) lucide.createIcons();
      }
      return false;
    } else {
      el.classList.remove('is-invalid');
      if (val) el.classList.add('is-valid');
      else el.classList.remove('is-valid');
      if (errEl) {
        if (successMsg) {
          errEl.innerHTML = `<span class="success-text">${successMsg}</span>`;
        } else {
          errEl.innerHTML = '';
        }
      }
      return true;
    }
  };

  const clearValidationErrors = () => {
    document.querySelectorAll('.field-error').forEach(el => el.innerHTML = '');
    document.querySelectorAll('.is-invalid, .is-valid').forEach(el => el.classList.remove('is-invalid', 'is-valid'));
  };

  const onBirthDateInput = (val) => {
    validateField('emp-ngaySinh');
  };

  const onEmployeeTypeChanged = () => {
    const type = document.getElementById('emp-loaiId').value || 'TOAN_THOI_GIAN';
    document.querySelectorAll('.poly-card').forEach(card => {
      card.classList.toggle('active', card.getAttribute('data-type') === type);
    });

    document.getElementById('fields-toanthoigian').style.display = type === 'TOAN_THOI_GIAN' ? 'flex' : 'none';
    document.getElementById('fields-banthoigian').style.display = type === 'BAN_THOI_GIAN' ? 'flex' : 'none';
    document.getElementById('fields-quanly').style.display = type === 'QUAN_LY' ? 'flex' : 'none';
    document.getElementById('fields-chuyengia').style.display = type === 'CHUYEN_GIA' ? 'flex' : 'none';
    if (window.lucide) lucide.createIcons();
  };

  const openAddEmployeeModal = () => {
    state.currentEditEmpId = null;
    document.getElementById('modal-employee-title').textContent = 'Thêm nhân sự mới';
    document.getElementById('emp-maNV').disabled = false;
    document.getElementById('form-employee').reset();
    clearValidationErrors();
    populateDepartmentSelects();

    document.querySelectorAll('.poly-card').forEach(card => {
      card.classList.remove('disabled');
    });

    selectEmployeeType('TOAN_THOI_GIAN');
    const caInput = document.getElementById('emp-caLamViec');
    if (caInput) caInput.value = 'Ca sáng (08:00 - 17:00)';
    openModal('modal-employee');
  };

  const openEditEmployeeModal = (maNV) => {
    const nv = state.employees.find(e => e.maNV === maNV);
    if (!nv) return;

    state.currentEditEmpId = maNV;
    document.getElementById('modal-employee-title').textContent = `Cập nhật thông tin: ${nv.hoTen}`;
    clearValidationErrors();
    populateDepartmentSelects();

    document.getElementById('emp-maNV').value = nv.maNV;
    document.getElementById('emp-maNV').disabled = true;
    document.getElementById('emp-hoTen').value = nv.hoTen || '';
    document.getElementById('emp-ngaySinh').value = nv.ngaySinh || '';
    document.getElementById('emp-soDienThoai').value = nv.soDienThoai || '';
    document.getElementById('emp-email').value = nv.email || '';
    document.getElementById('emp-trangThai').value = nv.trangThai || 'Dang lam';
    document.getElementById('emp-maPhongBan').value = nv.maPhongBan || '';
    document.getElementById('emp-loaiId').value = nv.loaiId || 'TOAN_THOI_GIAN';
    document.getElementById('emp-loaiId').disabled = true; // Polymorphic type cannot be changed

    document.querySelectorAll('.poly-card').forEach(card => {
      const isThis = card.getAttribute('data-type') === (nv.loaiId || 'TOAN_THOI_GIAN');
      card.classList.toggle('active', isThis);
      card.classList.toggle('disabled', !isThis);
    });
    onBirthDateInput(nv.ngaySinh || '');
    onEmployeeTypeChanged();

    if (nv.loaiId === 'TOAN_THOI_GIAN') {
      document.getElementById('emp-soNgayCong').value = nv.soNgayCong || 22;
      document.getElementById('emp-soGioLam').value = nv.soGioLam || 176;
      document.getElementById('emp-caLamViec').value = nv.caLamViec || 'Ca sáng (08:00 - 17:00)';
    } else if (nv.loaiId === 'BAN_THOI_GIAN') {
      document.getElementById('emp-bt-soGioLam').value = nv.soGioLam || 80;
      document.getElementById('emp-bt-luongTheoGio').value = nv.luongTheoGio || 50000;
    } else if (nv.loaiId === 'QUAN_LY') {
      document.getElementById('emp-ql-chucVu').value = nv.chucVu || '';
      document.getElementById('emp-ql-capQuanLy').value = nv.capQuanLy || '';
      document.getElementById('emp-ql-thongTinDacThu').value = nv.thongTinDacThu || '';
    } else if (nv.loaiId === 'CHUYEN_GIA') {
      document.getElementById('emp-cg-chuyenMon').value = nv.chuyenMon || '';
      document.getElementById('emp-cg-soNamKinhNghiem').value = nv.soNamKinhNghiem || 0;
    }

    openModal('modal-employee');
  };

  const saveEmployee = async (e) => {
    e.preventDefault();
    const type = document.getElementById('emp-loaiId').value;
    const isEdit = !!state.currentEditEmpId;

    const payload = {
      maNV: document.getElementById('emp-maNV').value.trim(),
      hoTen: document.getElementById('emp-hoTen').value.trim(),
      ngaySinh: document.getElementById('emp-ngaySinh').value.trim(),
      soDienThoai: document.getElementById('emp-soDienThoai').value.trim(),
      email: document.getElementById('emp-email').value.trim(),
      trangThai: document.getElementById('emp-trangThai').value,
      maPhongBan: document.getElementById('emp-maPhongBan').value,
      loaiId: type
    };

    if (type === 'TOAN_THOI_GIAN') {
      payload.soNgayCong = parseInt(document.getElementById('emp-soNgayCong').value) || 0;
      payload.soGioLam = parseFloat(document.getElementById('emp-soGioLam').value) || 0;
      payload.caLamViec = document.getElementById('emp-caLamViec').value.trim();
    } else if (type === 'BAN_THOI_GIAN') {
      payload.soGioLam = parseFloat(document.getElementById('emp-bt-soGioLam').value) || 0;
      payload.luongTheoGio = parseFloat(document.getElementById('emp-bt-luongTheoGio').value) || 0;
    } else if (type === 'QUAN_LY') {
      payload.chucVu = document.getElementById('emp-ql-chucVu').value.trim();
      payload.capQuanLy = document.getElementById('emp-ql-capQuanLy').value.trim();
      payload.thongTinDacThu = document.getElementById('emp-ql-thongTinDacThu').value.trim();
    } else if (type === 'CHUYEN_GIA') {
      payload.chuyenMon = document.getElementById('emp-cg-chuyenMon').value.trim();
      payload.soNamKinhNghiem = parseInt(document.getElementById('emp-cg-soNamKinhNghiem').value) || 0;
    }

    // Kiem tra dinh dang dau vao dong bo truc tiep (Inline Two-tier Validation)
    const fieldsToValidate = ['emp-maNV', 'emp-hoTen', 'emp-ngaySinh', 'emp-soDienThoai', 'emp-email'];
    if (type === 'TOAN_THOI_GIAN') {
      fieldsToValidate.push('emp-soNgayCong', 'emp-soGioLam');
    } else if (type === 'BAN_THOI_GIAN') {
      fieldsToValidate.push('emp-bt-soGioLam', 'emp-bt-luongTheoGio');
    }

    let isValid = true;
    let firstInvalid = null;
    for (const f of fieldsToValidate) {
      const ok = validateField(f);
      if (!ok && isValid) {
        isValid = false;
        firstInvalid = document.getElementById(f);
      }
    }

    if (!isValid) {
      if (firstInvalid) {
        firstInvalid.focus();
        firstInvalid.scrollIntoView({ behavior: 'smooth', block: 'center' });
      }
      showToast('Vui lòng kiểm tra và hoàn thiện các ô thông tin được đánh dấu đỏ.', 'warning', 'Dữ liệu chưa hợp lệ');
      return;
    }

    try {
      const url = isEdit ? `${API_BASE}/employees/${payload.maNV}` : `${API_BASE}/employees`;
      const method = isEdit ? 'PUT' : 'POST';

      const res = await fetch(url, {
        method: method,
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });
      const data = await res.json();

      if (data.success) {
        showToast(isEdit ? 'Cập nhật nhân viên thành công' : 'Thêm nhân viên mới thành công', 'success');
        closeModal('modal-employee');
        loadEmployees();
        loadDashboard();
      } else {
        showToast(data.message || 'Lỗi khi lưu nhân viên', 'danger');
      }
    } catch (err) {
      showToast('Lỗi kết nối máy chủ', 'danger');
    }
  };

  const deleteEmployee = (maNV, hoTen) => {
    confirmDelete(`Bạn có chắc chắn muốn xóa nhân viên ${hoTen} (${maNV})? Dữ liệu hợp đồng, công và lương liên quan cũng sẽ được cập nhật.`, async () => {
      try {
        const res = await fetch(`${API_BASE}/employees/${maNV}`, { method: 'DELETE' });
        const data = await res.json();
        if (data.success) {
          showToast(`Đã xóa nhân viên ${maNV}`, 'success');
          loadEmployees();
          loadDashboard();
        } else {
          showToast(data.message, 'danger');
        }
      } catch (e) {
        showToast('Lỗi khi xóa nhân viên', 'danger');
      }
    });
  };

  // ============================================================
  // 3. PHÒNG BAN
  // ============================================================
  const loadDepartments = async () => {
    try {
      const res = await fetch(`${API_BASE}/departments`);
      const data = await res.json();
      if (data.success) {
        state.departments = data.data;
        populateDepartmentSelects();
        renderDepartmentsGrid();
      }
    } catch (e) {
      showToast('Lỗi khi tải phòng ban', 'danger');
    }
  };

  const renderDepartmentsGrid = () => {
    const grid = document.getElementById('departments-grid');
    if (!grid) return;

    if (state.departments.length === 0) {
      grid.innerHTML = `
        <div class="card" style="grid-column: 1 / -1; text-align: center; padding: 48px;">
          <div class="empty-icon" style="margin: 0 auto 16px;"><i data-lucide="building-2"></i></div>
          <div class="empty-title">Chưa có phòng ban nào</div>
          <button class="btn btn-primary btn-sm" onclick="App.openAddDepartmentModal()">Tạo phòng ban đầu tiên</button>
        </div>
      `;
      if (window.lucide) lucide.createIcons({ root: grid });
      return;
    }

    grid.innerHTML = state.departments.map(pb => {
      return `
        <div class="card stat-card" style="align-items: flex-start; flex-direction: column; gap: 14px;">
          <div style="display: flex; justify-content: space-between; width: 100%; align-items: center;">
            <span class="badge badge-primary">${pb.maPhongBan}</span>
            <div class="table-actions">
              <button class="action-btn delete" title="Xóa phòng ban" onclick="App.deleteDepartment('${pb.maPhongBan}', '${pb.tenPhongBan}')">
                <i data-lucide="trash-2"></i>
              </button>
            </div>
          </div>

          <div>
            <h3 style="font-size: 18px; font-weight: 700; color: var(--text-primary)">${pb.tenPhongBan}</h3>
            <p style="font-size: 13px; color: var(--text-secondary); margin-top: 4px;">
              Trưởng phòng: <strong>${pb.tenTruongPhong || 'Chưa chỉ định'}</strong>
            </p>
          </div>

          <div style="display: flex; justify-content: space-between; width: 100%; align-items: center; border-top: 1px solid var(--border-color); padding-top: 14px; margin-top: 6px;">
            <span style="font-size: 13px; font-weight: 600; color: var(--text-secondary)">
              ${pb.soLuongNhanVien || 0} thành viên
            </span>
            <button class="btn btn-secondary btn-sm" onclick="App.openDeptMembersModal('${pb.maPhongBan}')">
              Quản lý nhân sự
            </button>
          </div>
        </div>
      `;
    }).join('');

    if (window.lucide) lucide.createIcons({ root: grid });
  };

  const openAddDepartmentModal = () => {
    document.getElementById('form-department').reset();
    openModal('modal-department');
  };

  const saveDepartment = async (e) => {
    e.preventDefault();
    const payload = {
      maPhongBan: document.getElementById('dept-maPB').value.trim(),
      tenPhongBan: document.getElementById('dept-tenPB').value.trim()
    };

    try {
      const res = await fetch(`${API_BASE}/departments`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });
      const data = await res.json();
      if (data.success) {
        showToast('Tạo phòng ban thành công', 'success');
        closeModal('modal-department');
        loadDepartments();
      } else {
        showToast(data.message, 'danger');
      }
    } catch (e) {
      showToast('Lỗi khi lưu phòng ban', 'danger');
    }
  };

  const deleteDepartment = (maPB, tenPB) => {
    confirmDelete(`Bạn có chắc muốn xóa phòng ban "${tenPB}" (${maPB})? Nhân viên sẽ không bị xóa mà chuyển sang trạng thái chưa gán phòng.`, async () => {
      try {
        const res = await fetch(`${API_BASE}/departments/${maPB}`, { method: 'DELETE' });
        const data = await res.json();
        if (data.success) {
          showToast(`Đã xóa phòng ban ${tenPB}`, 'success');
          loadDepartments();
          loadEmployees();
        } else {
          showToast(data.message, 'danger');
        }
      } catch (e) {
        showToast('Lỗi khi xóa phòng ban', 'danger');
      }
    });
  };

  const openDeptMembersModal = (maPB) => {
    const pb = state.departments.find(d => d.maPhongBan === maPB);
    if (!pb) return;
    state.currentSelectedDept = pb;

    document.getElementById('modal-dept-members-title').textContent = `Thành viên: ${pb.tenPhongBan} (${pb.maPhongBan})`;

    // Populate dropdown with employees not yet in this department
    const select = document.getElementById('select-add-member');
    const existingMemberIds = (pb.danhSachNhanVien || []).map(m => m.maNV);
    const available = state.employees.filter(e => !existingMemberIds.includes(e.maNV));

    select.innerHTML = `<option value="">-- Chọn nhân sự để thêm vào phòng --</option>` +
      available.map(e => `<option value="${e.maNV}">${e.hoTen} (${e.maNV} - ${e.loaiNhanVien})</option>`).join('');

    // Render table
    const tbody = document.getElementById('dept-members-table-body');
    if (!pb.danhSachNhanVien || pb.danhSachNhanVien.length === 0) {
      tbody.innerHTML = `<tr><td colspan="5" style="text-align:center; padding: 20px; color: var(--text-muted)">Phòng ban hiện chưa có nhân sự nào.</td></tr>`;
    } else {
      tbody.innerHTML = pb.danhSachNhanVien.map(m => {
        let desc = m.chucVu || m.chuyenMon || m.caLamViec || '-';
        return `
          <tr>
            <td><strong>${m.maNV}</strong></td>
            <td>${m.hoTen}</td>
            <td><span class="badge badge-secondary">${m.loaiNhanVien}</span></td>
            <td>${desc}</td>
            <td>
              <button class="action-btn delete" title="Gỡ khỏi phòng" onclick="App.removeMemberFromDept('${m.maNV}')">
                <i data-lucide="user-minus"></i>
              </button>
            </td>
          </tr>
        `;
      }).join('');
    }

    if (window.lucide) lucide.createIcons({ root: document.getElementById('modal-dept-members') });
    openModal('modal-dept-members');
  };

  const addMemberToDept = async () => {
    if (!state.currentSelectedDept) return;
    const select = document.getElementById('select-add-member');
    const maNV = select.value;
    if (!maNV) {
      showToast('Vui lòng chọn nhân viên cần thêm', 'warning');
      return;
    }

    try {
      const res = await fetch(`${API_BASE}/departments/${state.currentSelectedDept.maPhongBan}/members`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ maNV })
      });
      const data = await res.json();
      if (data.success) {
        showToast('Đã thêm nhân viên vào phòng ban', 'success');
        await loadDepartments();
        openDeptMembersModal(state.currentSelectedDept.maPhongBan);
      } else {
        showToast(data.message, 'danger');
      }
    } catch (e) {
      showToast('Lỗi khi thêm nhân sự', 'danger');
    }
  };

  const removeMemberFromDept = async (maNV) => {
    if (!state.currentSelectedDept) return;
    try {
      const res = await fetch(`${API_BASE}/departments/${state.currentSelectedDept.maPhongBan}/members/${maNV}`, {
        method: 'DELETE'
      });
      const data = await res.json();
      if (data.success) {
        showToast('Đã gỡ nhân viên khỏi phòng ban', 'success');
        await loadDepartments();
        openDeptMembersModal(state.currentSelectedDept.maPhongBan);
      } else {
        showToast(data.message, 'danger');
      }
    } catch (e) {
      showToast('Lỗi khi gỡ nhân viên', 'danger');
    }
  };

  // ============================================================
  // 4. HỢP ĐỒNG LAO ĐỘNG
  // ============================================================
  const loadContracts = async () => {
    try {
      const res = await fetch(`${API_BASE}/contracts`);
      const data = await res.json();
      if (data.success) {
        state.contracts = data.data;
        renderContractsTable();
      }
    } catch (e) {
      showToast('Lỗi khi tải hợp đồng', 'danger');
    }
  };

  const renderContractsTable = () => {
    const tbody = document.getElementById('contracts-table-body');
    if (!tbody) return;

    if (state.contracts.length === 0) {
      tbody.innerHTML = `<tr><td colspan="8" style="text-align:center; padding: 30px; color: var(--text-muted)">Chưa có hợp đồng nào. Nhấn "Tạo hợp đồng mới" để thêm.</td></tr>`;
      return;
    }

    tbody.innerHTML = state.contracts.map(hd => {
      // Expiry check logic (Sắp hết hạn trong vòng 30 ngày)
      let statusBadge = `<span class="badge badge-success">Hiệu lực</span>`;
      if (hd.ngayHetHan) {
        const parts = hd.ngayHetHan.split('/');
        if (parts.length === 3) {
          const expDate = new Date(parts[2], parts[1] - 1, parts[0]);
          const now = new Date(2026, 9, 25); // Baseline system date 10/2026
          const diffDays = Math.ceil((expDate - now) / (1000 * 60 * 60 * 24));
          if (diffDays < 0) {
            statusBadge = `<span class="badge badge-danger">Đã hết hạn</span>`;
          } else if (diffDays <= 30) {
            statusBadge = `<span class="badge badge-warning">Sắp hết hạn (${diffDays} ngày)</span>`;
          }
        }
      }

      return `
        <tr>
          <td><strong style="color: var(--primary)">${hd.maHopDong}</strong></td>
          <td>
            <div style="font-weight: 600">${hd.hoTenNV}</div>
            <small style="color: var(--text-muted)">Mã: ${hd.maNV} | ${hd.loaiNhanVien || '-'}</small>
          </td>
          <td><span class="badge badge-secondary">${hd.loaiHopDong}</span></td>
          <td>${hd.ngayKyKet || '-'}</td>
          <td>${hd.ngayHetHan || '<span style="color:var(--text-muted)">Không thời hạn</span>'}</td>
          <td><strong>${formatVND(hd.luongThoaThuan)}</strong></td>
          <td>${statusBadge}</td>
          <td style="text-align: right;">
            <div class="table-actions" style="justify-content: flex-end;">
              <button class="action-btn delete" title="Xóa hợp đồng" onclick="App.deleteContract('${hd.maHopDong}')">
                <i data-lucide="trash-2"></i>
              </button>
            </div>
          </td>
        </tr>
      `;
    }).join('');

    if (window.lucide) lucide.createIcons({ root: tbody });
  };

  const openAddContractModal = () => {
    document.getElementById('form-contract').reset();
    const select = document.getElementById('hd-maNV');
    select.innerHTML = `<option value="">-- Chọn nhân sự --</option>` +
      state.employees.map(e => `<option value="${e.maNV}">${e.hoTen} (${e.maNV})</option>`).join('');
    openModal('modal-contract');
  };

  const saveContract = async (e) => {
    e.preventDefault();
    const payload = {
      maHopDong: document.getElementById('hd-maHD').value.trim(),
      maNV: document.getElementById('hd-maNV').value,
      loaiHopDong: document.getElementById('hd-loaiHD').value,
      ngayKyKet: document.getElementById('hd-ngayKy').value.trim(),
      ngayHetHan: document.getElementById('hd-ngayHetHan').value.trim(),
      luongThoaThuan: parseFloat(document.getElementById('hd-luong').value) || 0
    };

    try {
      const res = await fetch(`${API_BASE}/contracts`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });
      const data = await res.json();
      if (data.success) {
        showToast('Tạo hợp đồng thành công', 'success');
        closeModal('modal-contract');
        loadContracts();
      } else {
        showToast(data.message, 'danger');
      }
    } catch (e) {
      showToast('Lỗi khi lưu hợp đồng', 'danger');
    }
  };

  const deleteContract = (maHD) => {
    confirmDelete(`Bạn có chắc muốn xóa hợp đồng ${maHD}?`, async () => {
      try {
        const res = await fetch(`${API_BASE}/contracts/${maHD}`, { method: 'DELETE' });
        const data = await res.json();
        if (data.success) {
          showToast(`Đã xóa hợp đồng ${maHD}`, 'success');
          loadContracts();
        } else {
          showToast(data.message, 'danger');
        }
      } catch (e) {
        showToast('Lỗi khi xóa hợp đồng', 'danger');
      }
    });
  };

  // ============================================================
  // 5. CHẤM CÔNG
  // ============================================================
  const loadAttendance = async () => {
    const month = document.getElementById('attendance-month').value;
    const year = document.getElementById('attendance-year').value;

    try {
      const res = await fetch(`${API_BASE}/attendance?month=${month}&year=${year}`);
      const data = await res.json();
      if (data.success) {
        state.attendance = data.data;
        renderAttendanceTable();
      }
    } catch (e) {
      showToast('Lỗi khi tải dữ liệu chấm công', 'danger');
    }
  };

  const renderAttendanceTable = () => {
    const tbody = document.getElementById('attendance-table-body');
    if (!tbody) return;

    if (state.attendance.length === 0) {
      tbody.innerHTML = `<tr><td colspan="7" style="text-align:center; padding: 30px; color: var(--text-muted)">Không có dữ liệu chấm công cho kỳ này.</td></tr>`;
      return;
    }

    tbody.innerHTML = state.attendance.map(cc => {
      const isLocked = cc.trangThai === 'Da chot cong';
      return `
        <tr>
          <td><strong style="color: var(--primary)">${cc.maNhanVien}</strong></td>
          <td><div style="font-weight: 600">${cc.hoTen || '-'}</div></td>
          <td>Tháng ${cc.thang}/${cc.nam}</td>
          <td><strong style="color: var(--success)">${cc.soNgayCong} ngày</strong></td>
          <td><strong style="color: var(--danger)">${cc.soNgayNghi} ngày</strong></td>
          <td>
            <span class="badge ${isLocked ? 'badge-success' : 'badge-warning'}">
              ${isLocked ? 'Đã chốt công' : 'Đang chấm công'}
            </span>
          </td>
          <td style="text-align: right;">
            <button class="btn btn-secondary btn-sm" onclick="App.openAttendanceModal('${cc.maNhanVien}', ${cc.thang}, ${cc.nam}, ${cc.soNgayCong}, ${cc.soNgayNghi}, '${cc.trangThai}')">
              ${isLocked ? 'Xem lịch' : 'Điểm danh / Chốt'}
            </button>
          </td>
        </tr>
      `;
    }).join('');
  };

  const openAttendanceModal = (maNV, thang, nam, soCong, soNghi, trangThai) => {
    const nv = state.employees.find(e => e.maNV === maNV);
    state.currentAttendanceData = { maNV, thang, nam, soCong, soNghi, trangThai };

    document.getElementById('att-emp-id').value = maNV;
    document.getElementById('att-month').value = thang;
    document.getElementById('att-year').value = nam;
    document.getElementById('att-emp-name').textContent = nv ? `${nv.hoTen} (${maNV})` : maNV;
    document.getElementById('att-period-desc').textContent = `Kỳ công: Tháng ${thang}/${nam}`;

    const isLocked = trangThai === 'Da chot cong';
    document.getElementById('att-status-badge').innerHTML = isLocked ?
      `<span class="badge badge-success">Đã chốt công</span>` :
      `<span class="badge badge-warning">Đang chấm công</span>`;

    const workInput = document.getElementById('att-work-days');
    const leaveInput = document.getElementById('att-leave-days');
    workInput.value = soCong;
    leaveInput.value = soNghi;

    workInput.disabled = isLocked;
    leaveInput.disabled = isLocked;
    document.getElementById('btn-lock-attendance').style.display = isLocked ? 'none' : 'inline-flex';
    document.getElementById('btn-save-attendance').style.display = isLocked ? 'none' : 'inline-flex';

    renderAttendanceCalendarGrid(thang, nam, soCong, soNghi, isLocked);
    openModal('modal-attendance');
  };

  const renderAttendanceCalendarGrid = (thang, nam, soCong, soNghi, isLocked) => {
    const grid = document.getElementById('attendance-calendar-grid');
    if (!grid) return;

    const daysInMonth = new Date(nam, thang, 0).getDate();
    let daysHtml = ['T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'CN'].map(d => `<div class="calendar-day-header">${d}</div>`).join('');

    for (let day = 1; day <= daysInMonth; day++) {
      let cellClass = 'calendar-cell';
      if (day <= soCong) cellClass += ' worked';
      else if (day <= soCong + soNghi) cellClass += ' off';

      daysHtml += `
        <div class="${cellClass}" data-day="${day}">
          <span>${day}</span>
        </div>
      `;
    }
    grid.innerHTML = daysHtml;
  };

  const saveAttendanceRecord = async (e) => {
    e.preventDefault();
    const payload = {
      maNhanVien: document.getElementById('att-emp-id').value,
      thang: parseInt(document.getElementById('att-month').value),
      nam: parseInt(document.getElementById('att-year').value),
      soNgayCong: parseInt(document.getElementById('att-work-days').value) || 0,
      soNgayNghi: parseInt(document.getElementById('att-leave-days').value) || 0,
      trangThai: state.currentAttendanceData ? state.currentAttendanceData.trangThai : 'Dang cham cong'
    };

    try {
      const res = await fetch(`${API_BASE}/attendance`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });
      const data = await res.json();
      if (data.success) {
        showToast('Cập nhật chấm công thành công', 'success');
        closeModal('modal-attendance');
        loadAttendance();
      } else {
        showToast(data.message, 'danger');
      }
    } catch (e) {
      showToast('Lỗi khi ghi nhận công', 'danger');
    }
  };

  const lockCurrentAttendance = async () => {
    if (!state.currentAttendanceData) return;
    const { maNV, thang, nam } = state.currentAttendanceData;

    try {
      const res = await fetch(`${API_BASE}/attendance/${maNV}/lock?month=${thang}&year=${nam}`, {
        method: 'POST'
      });
      const data = await res.json();
      if (data.success) {
        showToast(`Đã chốt công tháng ${thang}/${nam} cho ${maNV}`, 'success');
        closeModal('modal-attendance');
        loadAttendance();
      } else {
        showToast(data.message, 'danger');
      }
    } catch (e) {
      showToast('Lỗi khi chốt công', 'danger');
    }
  };

  // ============================================================
  // 6. KPI & THƯỞNG
  // ============================================================
  const loadKPI = async () => {
    const month = document.getElementById('kpi-month').value;
    const year = document.getElementById('kpi-year').value;

    try {
      const res = await fetch(`${API_BASE}/kpi?month=${month}&year=${year}`);
      const data = await res.json();
      if (data.success) {
        state.kpi = data.data;
        renderKPITable();
      }
    } catch (e) {
      showToast('Lỗi khi tải điểm KPI', 'danger');
    }
  };

  const renderKPITable = () => {
    const tbody = document.getElementById('kpi-table-body');
    if (!tbody) return;

    if (state.kpi.length === 0) {
      tbody.innerHTML = `<tr><td colspan="8" style="text-align:center; padding: 30px; color: var(--text-muted)">Chưa có điểm đánh giá KPI trong kỳ này.</td></tr>`;
      return;
    }

    tbody.innerHTML = state.kpi.map(item => {
      let rankBadge = `<span class="badge badge-secondary">Chưa đạt</span>`;
      if (item.diemKPI >= 90) rankBadge = `<span class="badge badge-success">Xuất sắc</span>`;
      else if (item.diemKPI >= 70) rankBadge = `<span class="badge badge-primary">Đạt chuẩn</span>`;

      return `
        <tr>
          <td><strong style="color: var(--primary)">${item.maNV}</strong></td>
          <td><div style="font-weight: 600">${item.hoTen || '-'}</div></td>
          <td><span class="badge badge-secondary">${item.loaiNhanVien || '-'}</span></td>
          <td>Tháng ${item.thang}/${item.nam}</td>
          <td><strong style="font-size: 15px;">${item.diemKPI}</strong> / 100</td>
          <td><strong style="color: var(--success); font-size: 15px;">${formatVND(item.tienThuong)}</strong></td>
          <td>${rankBadge}</td>
          <td style="text-align: right;">
            <button class="btn btn-secondary btn-sm" onclick="App.openEditKPIModal('${item.maNV}', ${item.diemKPI})">
              Chấm điểm
            </button>
          </td>
        </tr>
      `;
    }).join('');
  };

  const openEditKPIModal = (maNV, currentScore) => {
    const select = document.getElementById('kpi-input-maNV');
    select.innerHTML = state.employees.map(e => `<option value="${e.maNV}">${e.hoTen} (${e.maNV})</option>`).join('');
    select.value = maNV;
    document.getElementById('kpi-input-score').value = currentScore !== undefined ? currentScore : 80;
    openModal('modal-kpi');
  };

  const saveKPI = async (e) => {
    e.preventDefault();
    const payload = {
      maNV: document.getElementById('kpi-input-maNV').value,
      diemKPI: parseFloat(document.getElementById('kpi-input-score').value) || 0,
      thang: document.getElementById('kpi-month').value,
      nam: document.getElementById('kpi-year').value
    };

    try {
      const res = await fetch(`${API_BASE}/kpi`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });
      const data = await res.json();
      if (data.success) {
        showToast('Lưu điểm KPI thành công', 'success');
        closeModal('modal-kpi');
        loadKPI();
        loadDashboard();
      } else {
        showToast(data.message, 'danger');
      }
    } catch (e) {
      showToast('Lỗi khi lưu KPI', 'danger');
    }
  };

  // ============================================================
  // 7. BẢNG LƯƠNG
  // ============================================================
  const loadPayroll = async () => {
    const month = document.getElementById('payroll-month').value;
    const year = document.getElementById('payroll-year').value;

    try {
      const res = await fetch(`${API_BASE}/payroll?month=${month}&year=${year}`);
      const data = await res.json();
      if (data.success) {
        state.payroll = data.data;
        renderPayrollTable();
      }
    } catch (e) {
      showToast('Lỗi khi tải bảng lương', 'danger');
    }
  };

  const renderPayrollTable = () => {
    const tbody = document.getElementById('payroll-table-body');
    if (!tbody) return;

    if (state.payroll.length === 0) {
      tbody.innerHTML = `<tr><td colspan="12" style="text-align:center; padding: 30px; color: var(--text-muted)">Chưa tính lương kỳ này. Nhấn "Tính lương toàn công ty" để kích hoạt thuật toán C++.</td></tr>`;
      return;
    }

    tbody.innerHTML = state.payroll.map(bl => {
      return `
        <tr>
          <td><strong style="color: var(--primary)">${bl.maNV}</strong></td>
          <td><div style="font-weight: 600">${bl.hoTen}</div></td>
          <td><span class="badge badge-secondary">${bl.loaiNhanVien}</span></td>
          <td>${formatVND(bl.luongCoBan)}</td>
          <td>${formatVND(bl.phuCap)}</td>
          <td><span style="color:var(--success); font-weight:600;">${formatVND(bl.thuongKPI)}</span></td>
          <td>${formatVND(bl.khoanBoSung)}</td>
          <td><strong>${formatVND(bl.tongThuNhap)}</strong></td>
          <td>${formatVND(bl.baoHiem)}</td>
          <td>${formatVND(bl.thue)}</td>
          <td><strong style="color: var(--success); font-size: 15px;">${formatVND(bl.luongThucLinh)}</strong></td>
          <td style="text-align: right;">
            <button class="btn btn-secondary btn-sm" onclick="App.viewPaySlip('${bl.maNV}')">
              <i data-lucide="eye" style="width: 14px"></i>
              <span>Phiếu lương</span>
            </button>
          </td>
        </tr>
      `;
    }).join('');

    if (window.lucide) lucide.createIcons({ root: tbody });
  };

  const recalculateAllPayroll = async () => {
    const month = document.getElementById('payroll-month').value || '10';
    const year = document.getElementById('payroll-year').value || '2026';

    try {
      showToast('Đang gọi C++ QuyTrinhTinhLuong tính lương...', 'info');
      const res = await fetch(`${API_BASE}/payroll/calculate`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ thang: month, nam: year })
      });
      const data = await res.json();
      if (data.success) {
        showToast(data.message, 'success');
        if (data.data.errors && data.data.errors.length > 0) {
          data.data.errors.forEach(err => showToast(err, 'warning', 'Lưu ý Nghiệp vụ'));
        }
        loadPayroll();
        loadDashboard();
      } else {
        showToast(data.message, 'danger');
      }
    } catch (e) {
      showToast('Lỗi khi tính lương', 'danger');
    }
  };

  const viewPaySlip = async (maNV) => {
    const month = document.getElementById('payroll-month').value || '10';
    const year = document.getElementById('payroll-year').value || '2026';

    try {
      const res = await fetch(`${API_BASE}/payroll/${maNV}/slip?month=${month}&year=${year}`);
      const data = await res.json();
      if (data.success) {
        document.getElementById('payslip-render-area').innerHTML = data.data.html;
        openModal('modal-payslip');
      } else {
        showToast(data.message, 'danger');
      }
    } catch (e) {
      showToast('Lỗi khi tải phiếu lương', 'danger');
    }
  };

  // ============================================================
  // 8. BÁO CÁO C++ & THỐNG KÊ
  // ============================================================
  const loadRawReport = async () => {
    try {
      const [resReport, resSummary] = await Promise.all([
        fetch(`${API_BASE}/stats/report-raw`),
        fetch(`${API_BASE}/stats/summary`)
      ]);
      const dataReport = await resReport.json();
      const dataSummary = await resSummary.json();

      if (dataReport.success) {
        document.getElementById('raw-console-report').textContent = dataReport.data.report;
      }
      if (dataSummary.success) {
        const s = dataSummary.data;
        document.getElementById('stats-total-err').textContent = s.tongLoiGhiNhan || 0;
        document.getElementById('stats-err-chamcong').textContent = s.demLoiChamCong || 0;
        document.getElementById('stats-err-dulieu').textContent = s.demLoiDuLieu || 0;
        document.getElementById('stats-err-trangthai').textContent = s.demLoiTrangThai || 0;
      }
    } catch (e) {
      document.getElementById('raw-console-report').textContent = 'Lỗi kết nối máy chủ C++ khi lấy báo cáo.';
    }
  };

  const copyRawReport = () => {
    const text = document.getElementById('raw-console-report').textContent;
    navigator.clipboard.writeText(text).then(() => {
      showToast('Đã sao chép nội dung báo cáo C++ vào clipboard', 'success');
    }).catch(() => {
      showToast('Không thể sao chép văn bản', 'warning');
    });
  };

  // ============================================================
  // 9. QUẢN LÝ DỮ LIỆU
  // ============================================================
  const resetSampleData = async () => {
    confirmDelete('Khôi phục toàn bộ dữ liệu mẫu theo file test gốc main_test.cpp?', async () => {
      try {
        const res = await fetch(`${API_BASE}/data/reset-sample`, { method: 'POST' });
        const data = await res.json();
        if (data.success) {
          showToast(data.message, 'success');
          loadDashboard();
          loadEmployees();
          loadDepartments();
          loadContracts();
          loadAttendance();
          loadKPI();
          loadPayroll();
        }
      } catch (e) {
        showToast('Lỗi khi khôi phục dữ liệu', 'danger');
      }
    });
  };

  const handleImportJSON = (event) => {
    const file = event.target.files[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = async (e) => {
      try {
        const jsonContent = JSON.parse(e.target.result);
        const res = await fetch(`${API_BASE}/data/import-json`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify(jsonContent)
        });
        const data = await res.json();
        if (data.success) {
          showToast('Nhập dữ liệu JSON thành công', 'success');
          loadDashboard();
          loadEmployees();
          loadDepartments();
          loadContracts();
          loadAttendance();
          loadKPI();
          loadPayroll();
        } else {
          showToast(data.message, 'danger');
        }
      } catch (err) {
        showToast('Tệp tin JSON không hợp lệ', 'danger');
      }
    };
    reader.readAsText(file);
    event.target.value = '';
  };

  const exportCSV = (type) => {
    window.location.href = `${API_BASE}/data/export-csv?type=${type}`;
  };

  // ============================================================
  // INITIALIZATION
  // ============================================================
  const init = () => {
    initTheme();
    initClock();
    initShortcuts();
    initNavigation();

    // Filters event listeners
    const filterSearch = document.getElementById('filter-emp-search');
    const filterType = document.getElementById('filter-emp-type');
    const filterDept = document.getElementById('filter-emp-dept');
    const filterStatus = document.getElementById('filter-emp-status');

    if (filterSearch) filterSearch.addEventListener('input', loadEmployees);
    if (filterType) filterType.addEventListener('change', loadEmployees);
    if (filterDept) filterDept.addEventListener('change', loadEmployees);
    if (filterStatus) filterStatus.addEventListener('change', loadEmployees);

    // Initial fetch
    loadDashboard();
    loadDepartments();
    loadEmployees();

    if (window.lucide) lucide.createIcons();
  };

  return {
    init,
    switchTab,
    openModal,
    closeModal,
    openAddEmployeeModal,
    openEditEmployeeModal,
    saveEmployee,
    deleteEmployee,
    onEmployeeTypeChanged,
    selectEmployeeType,
    setShiftPreset,
    onBirthDateInput,
    validateField,
    clearValidationErrors,
    openAddDepartmentModal,
    saveDepartment,
    deleteDepartment,
    openDeptMembersModal,
    addMemberToDept,
    removeMemberFromDept,
    openAddContractModal,
    saveContract,
    deleteContract,
    loadAttendance,
    openAttendanceModal,
    saveAttendanceRecord,
    lockCurrentAttendance,
    loadKPI,
    openEditKPIModal,
    saveKPI,
    loadPayroll,
    recalculateAllPayroll,
    viewPaySlip,
    loadRawReport,
    copyRawReport,
    resetSampleData,
    handleImportJSON,
    exportCSV
  };
})();

document.addEventListener('DOMContentLoaded', () => {
  App.init();
});
