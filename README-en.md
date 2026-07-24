
```markdown
# DrawNice - Lightweight Drawing & AI Background Removal Tool

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub stars](https://img.shields.io/github/stars/photonest3/DrawNice)](https://github.com/photonest3/DrawNice)

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
    git clone https://github.com/photonest3/DrawNice.git
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

Contributions of all kinds are welcome! If you find a bug or have a new idea, please check the [Issues](https://github.com/photonest3/DrawNice/issues) page first to see if it has already been reported. Then, feel free to open an issue or submit a Pull Request.

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