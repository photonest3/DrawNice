DrawNice
好的，我将根据搜索结果中提供的开源抠图工具和白板工具的信息，为你生成一个名为 DrawNice 的开源项目 README.md（中文）和 README-en.md（英文）文件。

该项目的定位是：一款拥有AI智能抠图功能的轻量级绘画工具，类似于微软画图（Microsoft Paint）的升级版。

README.md (中文)
# DrawNice - 轻量级绘画与AI抠图工具

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub stars](https://img.shields.io/github/stars/photonest3/DrawNice)](https://github.com/photonest3/DrawNice)

**DrawNice** 是一款开源、免费、轻量级的图像编辑与绘画工具。它结合了类似 Microsoft Paint 的简洁直观操作界面，并集成了基于深度学习的 AI 智能抠图功能，所有图像处理均在本地浏览器中完成，无需上传服务器，充分保护你的隐私。

## ✨ 特性

- **🖌️ 经典绘画体验**：提供画笔、形状、填充、文字等基础绘画工具，操作流畅，上手简单。
- **🤖 AI 智能抠图**：一键移除图像背景，支持人像、物品、动物等多种主体识别，效果精细，边缘自然。[1](@ref)[7](@ref)
- **🔒 本地隐私保护**：所有 AI 计算均在本地完成（基于 WebGPU 或 WebAssembly），图像数据不会离开你的设备，杜绝隐私泄露风险。[1](@ref)[5](@ref)
- **🖼️ 丰富编辑功能**：支持图片裁剪、旋转、缩放、调整尺寸以及颜色调整等功能。
- **💾 多种格式导出**：支持保存为 PNG（含透明背景）、JPG 等常见图片格式。
- **💯 完全免费开源**：基于 MIT 协议开源，无任何功能限制，欢迎社区贡献。

## 🚀 快速开始

### 前置要求

- 现代浏览器（推荐使用最新版 Chrome 或 Edge 以获得最佳 AI 性能）
- Node.js 16.x 或更高版本（仅本地开发需要）

### 安装与运行

1.  **克隆仓库**
    ```bash
    git clone https://github.com/photonest3/DrawNice.git
    cd DrawNice
    ```

2.  **安装依赖**
    ```bash
    npm install
    ```

3.  **启动开发服务器**
    ```bash
    npm run start
    ```

4.  **打开应用**
    在浏览器中访问 `http://localhost:3000` 即可开始使用。

### Docker 部署（可选）

如果你想在服务器上部署，可以使用 Docker：

```bash
docker build -t drawnice .
docker run -d -p 8080:80 --name drawnice drawnice
然后访问 http://localhost:8080。

📖 使用指南
绘画：使用左侧工具栏选择画笔、形状或文本工具，在画布上自由创作。
AI 抠图：点击“上传图片”按钮导入一张图片，然后点击工具栏中的“AI 抠图”按钮，系统将自动识别并移除背景，仅保留主体。
精细调整：如果自动抠图结果不完美，你可以使用“魔棒”或“橡皮擦”工具进行手动微调。
导出：编辑完成后，点击“文件”菜单中的“导出为 PNG”，即可保存带透明背景的图片。
🛠️ 技术栈
前端框架: React 18
开发语言: TypeScript
AI 模型: ONNX Runtime (Web)，集成 U-2-Net 模型用于抠图
图像处理: Canvas API
构建工具: Vite
🤝 贡献指南
欢迎任何形式的贡献！如果你发现 Bug 或有新的想法，请先查看 Issues 页面，确认没有人提交过相同的问题，然后提出你的 Issue 或 Pull Request。

请确保你的代码遵循项目现有的编码风格，并包含必要的测试。

📄 许可
本项目采用 MIT 协议开源。详情请查看 LICENSE 文件。

🙏 致谢
本项目使用的 AI 抠图模型基于 U-2-Net 项目
感谢所有开源社区贡献者提供的优秀工具和库
: 基于 WebGPU 和 WebAssembly 的抠图工具，参考了开源项目 remove-bg 的技术实现。

: PixelCut Pro 智能精细抠图工具的用户交互设计，提供了多种手动微调模式。

: 基于 u2net.onnx 模型和 onnxruntime-web 的浏览器端智能抠图实现。

: 开源 AI 抠图工具综述中提到的多种模型和实现方案。

