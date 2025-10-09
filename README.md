```markdown
# Image Processing Application

A modern **Image Processing GUI** built using **C++ and Qt Framework**, featuring tools for filtering, blurring, and image adjustments such as brightness, contrast, and color controls — similar to photo editors like Pixlr or Photoshop.

---

## 🚀 Features

### 🧠 Core Image Processing
- EFfect Filter
  - Oil Painting Filter
  - Sunlight Filter
  - Purple Filter
  - Infrared Filter
  - Black & White Filter
- Transformation Filter
  - Skew
  - Rotate (90,180,270)
  - Flip (horizontally, Vertically)
- Blur Filter
- Merge

### 🪟 User Interface
- Built using **Qt Widgets** (`.ui` file designed in Qt Designer).
- Sidebar panel for **image properties** (appears when clicking “Properties” button).
- Central area for **image preview** using `QLabel`.
- Supports loading and saving images (`.png`, `.jpg`, `.jpeg`, `.bmp`).

---

## 🧩 Project Structure

```

ImageProcessor/
├── include/
│   ├── image.h               # Image class handling raw pixel data
│   └── mainwindow.h          # GUI logic and slots
├── src/
│   ├── main.cpp              # Entry point
│   ├── mainwindow.cpp        # UI event handlers and filters
│   ├── image.cpp             # Core image algorithms
├── ui/
│   └── mainwindow.ui         # Designed Qt interface (XML format)
├── assets/
│   └── sample.jpg            # Example test image
├── CMakeLists.txt            # CMake build file
└── README.md                 # Project documentation

````

---

## ⚙️ Installation & Build

### 🧱 Requirements
- **Qt 6.x** or **Qt 5.x** (with Qt Widgets module)
- **CMake ≥ 3.16**
- **C++17** compatible compiler (GCC, Clang, MSVC)
- **stb_image / stb_image_write** for image I/O (included in repo)

---

### 🧰 Build Steps

#### 🖥️ Using CMake
```bash
git clone https://github.com/<your-username>/Qt-ImageProcessor.git
cd Qt-ImageProcessor
mkdir build && cd build
cmake ..
make
./ImageProcessor
````

#### 🪟 Using Qt Creator

1. Open `CMakeLists.txt` in Qt Creator.
2. Configure with your preferred Qt kit.
3. Build and run the project directly.

---


## Author

**Abdelhamid Ahmed Abdelhamid ElRashidy**
**Hazem Mohamed**
**Anas Ibrahim**

---

## License

This project is licensed under the **MIT License** – free for personal and academic use.

---

## Screenshot

![App Screenshot](assets/sample_screenshot.png)
*(Example layout inspired by Pixlr’s property editor design.)*
