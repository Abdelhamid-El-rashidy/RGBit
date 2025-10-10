Absolutely — here’s a **clean, professional full README.md** for your project, with no emojis and fully consistent formatting.
You can directly paste this into your GitHub repository’s `README.md` file.

---

# Image Processing Application

A modern **Image Processing GUI** built using **C++ and the Qt Framework**, featuring tools for filtering, blurring, and image transformations such as brightness, contrast, and color adjustments — similar to photo editors like Pixlr or Photoshop.

---

## Features

### Core Image Processing

* **Effect Filters**

  * Oil Painting Filter
  * Sunlight Filter
  * Purple Filter
  * Gray
  * Infrared Filter
  * Black & White Filter
  * Old TV Filter
  * Invert
  * Brightness
* **Transformation Filters**
  * Skew
  * Rotate (90°, 180°, 270°)
  * Flip (Horizontal / Vertical)
* **Blur Filters**
* **Merge and Crop**
* Edges Detector
* **Resize and Frame Tools**

---

## User Interface

* Built using **Qt Widgets** (`.ui` file designed in Qt Designer)
* Sidebar panel for **image properties** (appears when clicking “Properties” button)
* Central preview area for **original** and **filtered** images using `QLabel`
* Supports **loading and saving** images in `.png`, `.jpg`, `.jpeg`, and `.bmp` formats
* Animated sidebar for smoother transitions between editing tools

---

## Project Structure

```
ImageProcessor/
├── include/
│   ├── image.h               # Image class handling raw pixel data
│   └── mainwindow.h          # GUI logic and event handlers
├── src/
│   ├── main.cpp              # Application entry point
│   ├── mainwindow.cpp        # UI event handlers and image operations
│   ├── image.cpp             # Core image algorithms
├── ui/
│   └── mainwindow.ui         # Designed Qt interface (XML format)
├── assets/
│   └── sample.jpg            # Example test image
├── CMakeLists.txt            # CMake build configuration
└── README.md                 # Project documentation
```

---

## Installation & Setup

### Prerequisites

Before building, ensure the following dependencies are installed:

#### On Linux (Ubuntu / Debian)

```bash
sudo apt update
sudo apt install build-essential cmake qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools
```

For Qt 6:

```bash
sudo apt install qt6-base-dev qt6-base-dev-tools
```

#### On Windows

* Install [Qt](https://www.qt.io/download-open-source) and select the **Qt 6.x (MinGW)** or **Qt 5.x** package.
* Ensure **CMake** and **MinGW** are added to your system PATH.

#### On macOS

```bash
brew install cmake qt
```

---

### Building with CMake

```bash
git clone https://github.com/<your-username>/Qt-ImageProcessor.git
cd Qt-ImageProcessor
mkdir build && cd build
cmake ..
make
./ImageProcessor
```

If `cmake` cannot find Qt, specify its installation path:

```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64 ..
```

---

### Building in CLion or Qt Creator

#### In CLion

1. Open the project folder in **CLion**.
2. Navigate to **File → Settings → Build, Execution, Deployment → CMake**.
3. Click **+** to add a new CMake profile (for example, `Debug`).
4. Press **Reload CMake Project**.
5. Click **Build → Build Project** or press `Ctrl + F9` to compile.

#### In Qt Creator

1. Open the `CMakeLists.txt` file.
2. Configure the project using your preferred **Qt Kit**.
3. Build and run the application directly.

---

### Troubleshooting

If you get this message:

```
Cannot find any CMake profile
```

Go to **Settings → Build, Execution, Deployment → CMake**, and create a new profile (e.g., `Debug`).

If you see:

```
Could not find Qt6::Widgets
```

Run:

```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64 ..
```

---

## Authors

- Abdelhamid Ahmed Abdelhamid ElRashidy 
- Hazem Mahmoud Mohammed 
- Anas Ibrahem Hesham Ali
---

## Authors

- Abdelhamid Ahmed Abdelhamid ElRashidy
- Hazem Mahmoud Mohammed
- Anas Ibrahem Hesham Ali
---

## Resources

* **Project Report:** [Google Document Link](https://docs.google.com/document/d/1uPoAtI0X4FZSFgmnABq6FTRaYfOWz46X1Rf8pQi4_94/edit?tab=t.0)
* **Demo Video:** [Google Drive Link](https://drive.google.com/drive/folders/1oabwI5nQSOdJEY3tQPBP4ow-7c4mLNsm?usp=drive_link)

---

## License

This project is licensed under the **MIT License** — free for personal, educational, and academic use.