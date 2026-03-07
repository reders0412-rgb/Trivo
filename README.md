# 🧊 Trivo — 3D File Viewer

> **오픈소스 3D 파일 뷰어** | Open-source 3D File Viewer  
> GLB · FBX · OBJ · GLTF · STL · DAE · 3DS · 30+ formats

[![Build](https://github.com/your-org/trivo/actions/workflows/build.yml/badge.svg)](https://github.com/your-org/trivo/actions/workflows/build.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey)](#)

---

## ✨ 주요 기능 / Features

| 기능 | Feature |
|------|---------|
| 30+ 3D 파일 형식 지원 | 30+ 3D file format support |
| PBR 렌더링 (OpenGL 4.1) | PBR Rendering (OpenGL 4.1 Core) |
| 드래그 앤 드롭 | Drag & Drop |
| 여러 모델을 하나의 씬에 배치 | Multi-model scene |
| 애니메이션 재생 (속도 조절) | Animation playback with speed control |
| 5가지 라이트 프리셋 | 5 Light presets (Studio/Outdoor/Dramatic/Soft/Night) |
| 다크 / 라이트 테마 | Dark & Light theme |
| 한국어 / 영어 UI | Korean & English UI |
| 스크린샷 저장 | Screenshot export (PNG) |
| 파일별 전용 아이콘 | Per-format file icons |

---

## 📦 지원 형식 / Supported Formats

| 카테고리 | 형식 |
|---------|-----|
| 모던 / Modern | `GLB` `GLTF` |
| 게임 엔진 / Game | `FBX` `X` `MD2` `MD3` `MD5` |
| 범용 / General | `OBJ` `DAE` `3DS` `PLY` `STL` |
| DCC 툴 / DCC | `BLEND` `LWO` `LWS` `MS3D` `COB` |
| CAD / Engineering | `STEP` `STP` `IGES` `IGS` `IFC` `DXF` |
| 애니메이션 / Anim | `BVH` `SMD` `VTA` |
| 포인트 클라우드 | `PCD` `XYZ` `PTS` |
| 기타 / Misc | `AC` `OFF` `NFF` `TER` ... |

---

## ⌨️ 조작법 / Controls

| 조작 | 기능 |
|------|------|
| 왼쪽 드래그 | 카메라 회전 / Orbit |
| 가운데 드래그 / Shift+우클릭 | 카메라 이동 / Pan |
| 스크롤 | 줌 / Zoom |
| `R` | 카메라 초기화 |
| `F` | 씬에 맞춤 |
| `Space` | 애니메이션 재생/정지 |
| `Ctrl+O` | 파일 열기 |
| `Ctrl+Shift+O` | 씬에 모델 추가 |
| `Ctrl+P` | 스크린샷 |
| `Ctrl+T` | 다크/라이트 테마 전환 |

---

## 🛠 빌드 / Build

### 요구사항 / Requirements

| 항목 | 버전 |
|------|------|
| C++ 컴파일러 | C++20 지원 (MSVC 2019+, GCC 10+, Clang 12+) |
| CMake | 3.20+ |
| Qt | 6.4+ |
| Assimp | 5.x |

### Linux / macOS (로컬)

```bash
# Ubuntu
sudo apt-get install libassimp-dev qt6-base-dev libqt6opengl6-dev

# macOS
brew install assimp qt6

# 빌드
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/Release/Trivo
```

### Windows (로컬)

```powershell
# 1. Qt6 설치: https://www.qt.io/download (공식 인스톨러)
# 2. vcpkg로 Assimp 설치
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.bat
./vcpkg/vcpkg install assimp:x64-windows

# 3. 빌드
cmake -B build -S . `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_TOOLCHAIN_FILE="vcpkg/scripts/buildsystems/vcpkg.cmake" `
  -DVCPKG_TARGET_TRIPLET="x64-windows"
cmake --build build --config Release
```

---

## 🚀 GitHub Actions 자동 빌드

### 빌드 전략

| 플랫폼 | Qt 설치 방법 | Assimp 설치 방법 |
|--------|------------|----------------|
| Windows | `jurplel/install-qt-action` (공식 바이너리) | vcpkg (직접 클론, builtin-baseline 없음) |
| macOS | `jurplel/install-qt-action` | Homebrew |
| Linux | `jurplel/install-qt-action` | apt (`libassimp-dev`) |

> ⚠️ **주의**: Qt를 vcpkg로 설치하면 `syncqt` 오류가 발생합니다.  
> 반드시 공식 Qt 바이너리 인스톨러 (`jurplel/install-qt-action`)를 사용하세요.

### 릴리즈 방법

```bash
# 태그를 push하면 자동으로 3개 플랫폼 빌드 후 GitHub Release 생성
git tag v1.0.0
git push origin v1.0.0
```

빌드 결과물:
- `Trivo-Windows-x64.zip` — windeployqt 포함 (바로 실행 가능)
- `Trivo-macOS-Universal.dmg` — arm64 + x86_64 Universal Binary
- `Trivo-Linux-x86_64.AppImage` — `chmod +x` 후 바로 실행

---

## 📚 사용 라이브러리 / Libraries

| 라이브러리 | 라이선스 | 용도 |
|-----------|---------|------|
| [Qt 6](https://www.qt.io) | LGPL v3 | GUI, OpenGL |
| [Assimp 5.x](https://github.com/assimp/assimp) | BSD 3-Clause | 3D 파일 로드 |
| OpenGL 4.1 | Vendor | 렌더링 |

전체 라이선스 전문은 [THIRD_PARTY_LICENSES.txt](THIRD_PARTY_LICENSES.txt) 참조.

---

## 📄 라이선스 / License

MIT License — Copyright (c) 2024 Trivo Contributors  
자세한 내용은 [LICENSE](LICENSE) 참조.
