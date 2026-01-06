# Merkuro 解耦重构 - 项目状态

**最后更新**: 2026-01-07
**状态**: 🚀 Phase 4.2 完成，Phase 2 就绪 (等待依赖)

---

## 📊 快速概览

| 指标 | 值 |
|------|-----|
| 总工期 | 10 周 (快速路径) |
| 当前进度 | Week 2/10 |
| 完成度 | 55% |
| 核心库代码 | 4,900+ 行 |
| 单元测试 | 71 个 (100% 通过) |
| KDE 依赖 | 0 个 (当前 App & Core) |

---

## ✅ 已完成阶段

### Phase 0-3: 核心库 (Core Library)
- **产出**: 纯 C++ 业务逻辑库
- **测试**: 40 个单元测试

### Phase 5/5B: 本地后端 (Local Backends)
- **产出**: ICSFileBackend & DirectoryBackend
- **新增**: JSON 元数据存储 (颜色、可见性)

### Phase 4: UI 层 (Standalone App)
- **产出**: `personal-calendar`
- **功能**: 
    - ✅ 增删改查事件 (CRUD)
    - ✅ **日历管理** (Color, Visibility)
    - ✅ **独立运行** (无 KDE 依赖)

---

## 🚧 暂停中 / 就绪 (Ready)

### Phase 2: Akonadi 适配器
- **状态**: 代码已就绪，等待依赖
- **进展**:
    - [x] 构建系统配置 (自动检测)
    - [x] DataConverter 逻辑实现
    - [ ] 实际编译验证 (需 KF5 环境)
- **下一步**: 在具备 KDE 开发环境的机器上编译即可启用。

---

## 🚀 下一步

**打包与发布 (Phase 6)**
- [ ] 准备 AppImage 构建脚本
- [ ] 编写用户手册
