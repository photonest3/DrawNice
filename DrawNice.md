# DrawNice

好的，我将根据搜索结果中提供的开源抠图工具和白板工具的信息，为你生成一个名为 **DrawNice** 的开源项目 `README.md`（中文）和 `README-en.md`（英文）文件。

该项目的定位是：一款拥有AI智能抠图功能的轻量级绘画工具，类似于微软画图（Microsoft Paint）的升级版。

***

### README.md (中文)

````markdown
# DrawNice - 轻量级绘画与AI抠图工具

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub stars](https://img.shields.io/github/stars/your-username/DrawNice)](https://github.com/your-username/DrawNice)

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
    git clone https://github.com/your-username/DrawNice.git
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
````

然后访问 `http://localhost:8080`。

## 📖 使用指南

1. **绘画**：使用左侧工具栏选择画笔、形状或文本工具，在画布上自由创作。
2. **AI 抠图**：点击“上传图片”按钮导入一张图片，然后点击工具栏中的“AI 抠图”按钮，系统将自动识别并移除背景，仅保留主体。
3. **精细调整**：如果自动抠图结果不完美，你可以使用“魔棒”或“橡皮擦”工具进行手动微调。
4. **导出**：编辑完成后，点击“文件”菜单中的“导出为 PNG”，即可保存带透明背景的图片。

## 🛠️ 技术栈

* **前端框架**: React 18
* **开发语言**: TypeScript
* **AI 模型**: ONNX Runtime (Web)，集成 U-2-Net 模型用于抠图&#x20;
* **图像处理**: Canvas API
* **构建工具**: Vite

## 🤝 贡献指南

欢迎任何形式的贡献！如果你发现 Bug 或有新的想法，请先查看 [Issues](https://github.com/your-username/DrawNice/issues) 页面，确认没有人提交过相同的问题，然后提出你的 Issue 或 Pull Request。

请确保你的代码遵循项目现有的编码风格，并包含必要的测试。

## 📄 许可

本项目采用 MIT 协议开源。详情请查看 LICENSE 文件。

## 🙏 致谢

* 本项目使用的 AI 抠图模型基于 [U-2-Net](https://github.com/xuebinqin/U-2-Net) 项目&#x20;
* 感谢所有开源社区贡献者提供的优秀工具和库

: 基于 WebGPU 和 WebAssembly 的抠图工具，参考了开源项目 `remove-bg` 的技术实现。

: PixelCut Pro 智能精细抠图工具的用户交互设计，提供了多种手动微调模式。

: 基于 `u2net.onnx` 模型和 `onnxruntime-web` 的浏览器端智能抠图实现。

: 开源 AI 抠图工具综述中提到的多种模型和实现方案。

````
### README-en.md (英文)

```markdown
# DrawNice - Lightweight Drawing & AI Background Removal Tool

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub stars](https://img.shields.io/github/stars/your-username/DrawNice)](https://github.com/your-username/DrawNice)

**DrawNice** is an open-source, free, and lightweight image editing and drawing tool. It combines the simple and intuitive interface of Microsoft Paint with a powerful AI-powered background removal feature. All image processing is done locally in your browser, ensuring your privacy is fully protected.

## ✨ Features

- **🖌️ Classic Drawing Experience**: Provides brushes, shapes, fill, text, and other basic painting tools for a smooth and easy-to-use experience.
- **🤖 AI-Powered Background Removal**: Remove image backgrounds with a single click. Supports accurate recognition of people, objects, and animals, producing clean and natural edges.[1](@ref)[7](@ref)
- **🔒 Local Privacy Protection**: All AI computations are performed locally (via WebGPU or WebAssembly). Your images never leave your device, eliminating privacy risks.[1](@ref)[5](@ref)
- **🖼️ Rich Editing Tools**: Supports cropping, rotating, resizing, and color adjustments.
- **💾 Multiple Export Formats**: Save your work as PNG (with transparent background) or JPG.
- **💯 Completely Free & Open Source**: Licensed under the MIT license with no feature limitations. Community contributions are welcome.

## 🚀 Quick Start

### Prerequisites

- A modern web browser (Chrome or Edge recommended for best AI performance)
- Node.js 16.x or higher (only required for local development)

### Installation & Setup

1.  **Clone the Repository**
    ```bash
    git clone https://github.com/your-username/DrawNice.git
    cd DrawNice
    ```

2.  **Install Dependencies**
    ```bash
    npm install
    ```

3.  **Start the Development Server**
    ```bash
    npm run start
    ```

4.  **Open the Application**
    Visit `http://localhost:3000` in your web browser.

### Docker Deployment (Optional)

To deploy on a server, you can use Docker:

```bash
docker build -t drawnice .
docker run -d -p 8080:80 --name drawnice drawnice
````

Then visit `http://localhost:8080`.

## 📖 User Guide

1. **Drawing**: Select the brush, shape, or text tool from the left toolbar and start creating on the canvas.
2. **AI Background Removal**: Click the "Upload Image" button to import a picture, then click the "AI Remove BG" button in the toolbar. The system will automatically detect and remove the background.
3. **Fine-Tuning**: If the auto-removal result isn't perfect, you can use the "Magic Wand" or "Eraser" tool for manual adjustments.
4. **Exporting**: Once finished, click "Export as PNG" in the "File" menu to save your image with a transparent background.

## 🛠️ Tech Stack

* **Frontend Framework**: React 18
* **Language**: TypeScript
* **AI Model**: ONNX Runtime (Web), integrating the U-2-Net model for background removal&#x20;
* **Image Processing**: Canvas API
* **Build Tool**: Vite

## 🤝 Contributing

Contributions of all kinds are welcome! If you find a bug or have a new idea, please check the [Issues](https://github.com/your-username/DrawNice/issues) page first to see if it has already been reported. Then, feel free to open an issue or submit a Pull Request.

Please ensure your code follows the project's coding style and includes necessary tests.

## 📄 License

This project is open-sourced under the MIT License. See the LICENSE file for details.

## 🙏 Acknowledgements

* The AI background removal model used in this project is based on [U-2-Net](https://github.com/xuebinqin/U-2-Net)&#x20;
* Thanks to all open-source community contributors for their excellent tools and libraries

: Implementation references the `remove-bg` project for its WebGPU and WebAssembly-based background removal technology.

: User interaction design references the PixelCut Pro tool, offering manual fine-tuning modes.

: Browser-side intelligent matting implementation based on the `u2net.onnx` model and `onnxruntime-web`.

: Overview of various models and implementations from open-source AI matting tool reviews.

```
**请注意：**
1.  请将代码中的 `your-username` 和 `https://github.com/your-username/DrawNice.git` 替换为你实际的 GitHub 用户名和仓库地址。
2.  [1](@ref) 到 [7](@ref) 的引用标记仅用于在 README 中注明信息来源，在实际的 Markdown 文件中，它们是普通文字，不会影响显示。
```

​
