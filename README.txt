# MonoStudio

MonoStudio是一个功能丰富的桌面工具集，为用户提供多种实用工具，帮助提高工作效率和生活质量。

## 功能特性

### 实用工具
- **Base64编解码器**：用于编码和解码Base64字符串
- **GUID生成器**：生成UUID v4格式的唯一标识符
- **JSON格式化**：格式化和验证JSON数据
- **文件哈希**：计算文件的MD5、SHA-1、SHA-256、SHA-512哈希值
- **密码生成器**：生成安全的随机密码
- **时间工具**：时间戳转换、时间差计算
- **单位转换**：长度、重量、温度、面积、体积单位转换
- **网络工具**：Ping测试、IP地址检测、DNS查询
- **系统信息**：查看系统硬件和软件信息
- **屏幕截图**：截取屏幕内容并保存
- **正则表达式测试**：测试和验证正则表达式

### 个人生活
- **灵感碎片**：捕捉转瞬即逝的想法
- **工作手记**：记录工作中的点滴
- **随想录**：随性的思考与感悟

## 技术栈

- **前端框架**：Vue.js
- **构建工具**：Vite
- **样式**：CSS变量、Flexbox、Grid
- **状态管理**：Vue组件状态
- **存储**：localStorage（本地存储）

## 安装和运行

1. 克隆仓库：
   ```bash
   git clone https://github.com/MonoKelvin/MonoStudio.git
   cd MonoStudio
   ```

2. 安装依赖：
   ```bash
   npm install
   ```

3. 开发模式运行：
   ```bash
   npm run dev
   ```

4. 构建生产版本：
   ```bash
   npm run build
   ```

## 项目结构

```
src/
  ├── components/        # 通用组件
  │   └── base/          # 基础组件
  ├── modules/           # 功能模块
  │   ├── shell/         # 应用外壳
  │   └── tools/         # 工具模块
  ├── assets/            # 静态资源
  └── styles/            # 全局样式
```

## 贡献

欢迎贡献代码和提出建议！请通过GitHub Issues和Pull Requests参与项目。

## 许可证

MIT License

## 联系方式

- GitHub: https://github.com/MonoKelvin