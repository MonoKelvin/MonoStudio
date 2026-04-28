# 进程管理工具模块

## 📁 文件结构

```
process-manager/
├── components/
│   ├── ProcessListToolbar.vue    # 工具栏组件（刷新、搜索、列显示等）
│   ├── ProcessListStats.vue      # 统计信息栏组件
│   ├── ProcessListTable.vue      # 表格主组件
│   ├── ProcessListRow.vue        # 表格行组件
│   ├── ProcessDetailDialog.vue   # 进程详情对话框
│   ├── ProcessIcon.vue           # 进程图标组件
│   └── InfoItem.vue              # 信息项组件
├── composables/
│   ├── useProcessSort.js         # 排序逻辑
│   ├── useProcessSearch.js       # 搜索逻辑
│   └── useProcessPagination.js   # 分页逻辑
├── services/
│   └── ProcessService.js         # 进程服务
├── utils/
│   ├── processConstants.js       # 常量定义
│   └── processHelpers.js         # 工具函数
├── index.js                       # 模块导出
├── ProcessManagerToolView.vue    # 主视图组件
└── README.md                      # 说明文档
```

## 🎯 组件职责

### 主组件
- **ProcessManagerToolView.vue**: 主入口组件，协调整个工具的功能

### 功能组件
- **ProcessListToolbar.vue**: 工具栏，包含刷新、搜索、列显示切换等功能
- **ProcessListStats.vue**: 显示系统统计信息（CPU、内存、进程总数）
- **ProcessListTable.vue**: 表格主容器，管理表格的渲染和分页
- **ProcessListRow.vue**: 单个进程行的渲染组件

### 子组件
- **ProcessIcon.vue**: 根据进程名显示对应的图标
- **InfoItem.vue**: 显示键值对信息的UI组件

### 辅助组件
- **ProcessDetailDialog.vue**: 显示进程详细信息的对话框

## 🔄 组合式函数

- **useProcessSort**: 处理进程排序逻辑
- **useProcessSearch**: 处理进程搜索功能
- **useProcessPagination**: 处理分页逻辑

## 🛠️ 工具函数

- **processConstants**: 定义常量（系统进程列表、列配置、状态映射等）
- **processHelpers**: 提供辅助函数（内存格式化、状态判断等）

## 🚀 使用示例

```vue
<script setup>
import ProcessManagerToolView from './components/ProcessManagerToolView.vue'

// 在父组件中使用
<template>
  <ProcessManagerToolView
    tool-name="进程管理"
    tool-description="查看和管理系统进程"
  />
</template>
```

## ✨ 功能特性

- ✅ 进程列表展示（支持排序、搜索、分页）
- ✅ 进程详细信息查看
- ✅ 进程终止功能
- ✅ 进程暂停/恢复功能
- ✅ 系统统计信息显示
- ✅ 列显示/隐藏自定义
- ✅ 自动刷新功能
- ✅ 键盘快捷键支持
- ✅ 动画效果和视觉反馈

## 🎨 动画效果

- 列表项进入/离开动画
- 加载动画（跳动的点）
- 空状态浮动动画
- 悬停高亮效果
- 进度条渐变动画
- 按钮交互反馈

## 📝 注意事项

1. 旧的 `ProcessList.vue` 文件已保留作为备份
2. 新的模块化结构使用 `ProcessListNew.vue` 作为主要组件
3. 所有子组件都可以单独使用或组合使用
4. 组合式函数提供了可复用的业务逻辑
