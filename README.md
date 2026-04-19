# MonoStudio

MonoStudio是一个连开发者都不知道全部功能的神奇工具箱，基于Electron和Vue构建，为用户提供多种实用工具，帮助提高工作效率和生活质量。

**重要说明**：本项目全部代码由AI赛博打工人生成，开发者负责取餐和整合。鼠鼠我呀，真的一行代码都不想写了～ (￣ω￣)

### 使用的AI工具
- [Trae](https://trae.io) - 智能开发助手
- [Cursor](https://cursor.sh) - AI代码编辑器
- [DeepSeek](https://deepseek.com) - 深度求索AI

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
/
  ├── electron/          # Electron主进程代码
  ├── src/               # 前端源代码
  │   ├── components/    # 通用组件
  │   │   └── base/      # 基础组件
  │   ├── modules/       # 功能模块
  │   │   ├── shell/     # 应用外壳
  │   │   └── tools/     # 工具模块
  │   ├── assets/        # 静态资源
  │   └── styles/        # 全局样式
```

## 贡献

欢迎贡献代码和提出建议！请通过GitHub Issues和Pull Requests参与项目。

## 许可证

本项目采用 [MIT License](LICENSE) 开源许可证。

## 联系方式

- 邮箱：[15007083506@qq.com](mailto:15007083506@qq.com)
- GitHub: [https://github.com/MonoKelvin](https://github.com/MonoKelvin)

## 开发计划

### Todo列表

#### 已完成
- ✅ Base64编解码器
- ✅ GUID生成器
- ✅ JSON格式化
- ✅ 文件哈希计算
- ✅ 密码生成器
- ✅ 时间工具
- ✅ 单位转换
- ✅ 网络工具
- ✅ 系统信息查看
- ✅ 屏幕截图功能
- ✅ 正则表达式测试
- ✅ 灵感碎片记录
- ✅ 工作手记功能
- ✅ 随想录功能

#### 待完成
- ⬜ 深色模式支持
- ⬜ 多语言国际化
- ⬜ 插件系统
- ⬜ 云同步功能
- ⬜ 更多实用工具
- ⬜ 性能优化
- ⬜ 文档完善
- ⬜ 测试覆盖

## 赞助支持

如果您觉得MonoStudio对您有帮助，欢迎通过以下方式支持项目发展：

### 赞助方式

| 支付宝 | 微信支付 |
|-------|--------|
| <img src="src/assets/images/alipay_payment_code.jpg" width="200" height="200" alt="支付宝二维码"> | <img src="src/assets/images/wechat_payment_code.jpg" width="200" height="200" alt="微信支付二维码"> |