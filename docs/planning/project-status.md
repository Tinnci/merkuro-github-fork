# Merkuro 解耦重构 - 项目状态

**最后更新**: 2026-01-07
**状态**: 🚀 Phase 4 完成，UI 功能完善

---

## 📊 快速概览

| 指标 | 值 |
|------|-----|
| 总工期 | 10 周 (快速路径) |
| 当前进度 | Week 2/10 |
| 完成度 | 45% |
| 核心库代码 | 4,500+ 行 |
| 单元测试 | 71 个 (100% 通过) |
| KDE 依赖 | 0 个 (App & Core) ✅ |

---

## ✅ 已完成阶段

### Phase 0-3: 核心库 (Core Library)
- **产出**: 纯 C++ 业务逻辑库
- **测试**: 40 个单元测试
- **特点**: 定义了 ICalendarStorage 接口

### Phase 5/5B: 本地后端 (Local Backends)
- **产出**: ICSFileBackend & DirectoryBackend
- **测试**: 31 个单元测试
- **能力**: 支持单文件和多文件日历存储

### Phase 4: UI 层 (Standalone App)
- **产出**: `personal-calendar` 可执行文件
- **技术**: QtQuick Controls 2 (Material)
- **功能**: 
    - ✅ 查看/创建/删除事件
    - ✅ **编辑事件** (New!)
    - ✅ **时间选择器** (New!)
    - ✅ 多日历支持 (后端就绪)

---

## 🔄 进行中 / 计划中

### Phase 2: Akonadi 适配器 (Week 7-9)
- [ ] AkonadiCalendarBackend 实现
- [ ] 数据转换层
- **目标**: 让 Core 库能作为 KDE Plasma 的一部分运行

### Phase 6: 完善与发布
- [ ] 打包 (Flatpak/AppImage)
- [ ] CI/CD 流程

---

## 🚀 下一步

**UI 完善**:
- 增加日历管理 UI (创建/删除日历)
- 增加事件颜色支持

**后端**:
- 开始 Phase 2 (Akonadi) 调研
