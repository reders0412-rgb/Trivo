# Trivo — 개발 가이드 / Development Guide

## 아키텍처 / Architecture

```
trivo/
├── src/
│   ├── main.cpp                  # 진입점 (OpenGL format 설정, i18n 초기화) 
│   ├── core/
│   │   ├── MainWindow.{h,cpp}    # 메인 윈도우, 테마/언어, D&D
│   │   ├── Viewport.{h,cpp}      # QOpenGLWidget, 마우스 조작, 60fps 루프
│   │   ├── Scene.{h,cpp}         # 씬 모델 목록 관리 (QObject + signals)
│   │   ├── Camera.{h,cpp}        # Orbit / Pan / Zoom, 투영 행렬
│   │   ├── Renderer.{h,cpp}      # OpenGL 4.1 PBR, 그리드, GPU 업로드
│   │   ├── ModelLoader.{h,cpp}   # Assimp 로드, 메타데이터 추출
│   │   ├── LightSystem.{h,cpp}   # 5가지 라이트 프리셋, PBR 바인딩
│   │   └── AnimationPlayer.{h,cpp} # 시간 기반 애니메이션 타이머
│   ├── ui/
│   │   ├── Sidebar.{h,cpp}       # 우측 패널 탭 컨테이너
│   │   ├── ScenePanel.{h,cpp}    # 씬 모델 트리, Transform 편집
│   │   ├── InfoPanel.{h,cpp}     # 상태바 파일 정보 표시
│   │   ├── ToolBar.{h,cpp}       # 상단 툴바 (열기/추가/재생/언어/테마)
│   │   ├── AboutDialog.{h,cpp}   # 라이브러리 및 라이선스 다이얼로그
│   │   └── DropOverlay.{h,cpp}   # 드래그 드롭 오버레이
│   ├── i18n/
│   │   └── I18n.{h,cpp}          # 한국어/영어 문자열 관리
│   └── utils/
│       ├── FileUtils.{h,cpp}     # 형식 필터, 지원 확인, 용량 표시
│       └── IconProvider.{h,cpp}  # 확장자별 SVG 아이콘 반환
├── resources/
│   ├── resources.qrc             # Qt 리소스 파일
│   └── icons/
│       ├── trivo_app.svg         # 앱 아이콘 (3D 큐브 로고)
│       ├── open/add/reset...svg  # 툴바 아이콘
│       └── ext_*.svg             # 확장자별 아이콘 (glb/fbx/obj 등 16종+)
├── .github/workflows/
│   └── build.yml                 # Windows + macOS + Linux AppImage 빌드
├── CMakeLists.txt
└── README.md
```

## 렌더링 파이프라인 / Rendering Pipeline

```
paintGL() 호출 (60fps)
  └─ Renderer::render()
       ├─ Grid 렌더 (GRID shader, 반투명 블렌딩)
       └─ 각 SceneModel 순회
            ├─ GPU 업로드 여부 확인 (assimpScene 포인터 키)
            ├─ Model matrix 계산 (position/rotation/scale)
            ├─ PBR shader 바인딩
            │   ├─ uModel / uView / uProj / uNormalMat
            │   ├─ uCamPos (스페큘러 계산용)
            │   ├─ uAmbient / uAmbientStr
            │   └─ uLight[0..3] (Direction/Point 4개까지)
            └─ 각 메시 Draw (GL_TRIANGLES, EBO)
```

## 라이트 프리셋 / Light Presets

| 이름 | 특징 |
|------|------|
| Studio | 키/필/림 3점 조명, 스튜디오 감각 |
| Outdoor | 태양광 + 하늘광 |
| Dramatic | 강한 단방향 + 약한 배경 |
| Soft | 4방향 균일 확산광 |
| Night | 달빛 + 포인트 램프 |

## 새 언어 추가 / Adding a Language

`src/i18n/I18n.cpp`의 `loadStrings()`에 새 언어 블록 추가:

```cpp
auto &ja = m_strings["ja"];
ja["file.open"] = "開く…";
// ...
```

툴바 콤보박스에 항목 추가:
```cpp
// ToolBar.cpp
m_langCombo->addItem("日本語", "ja");
```

## 새 형식 추가 / Adding a Format

Assimp가 지원하는 형식이라면 `FileUtils::supportedExtensions()`에 확장자 추가 → 자동 지원.

```cpp
// FileUtils.cpp
return {"glb","gltf","fbx", ..., "새확장자"};
```

아이콘은 `resources/icons/ext_새확장자.svg` 파일 추가 후 `resources.qrc`에 등록.

## GitHub Actions 릴리즈 / Releasing

```bash
# 태그 push → 자동으로 3개 플랫폼 빌드 + GitHub Release 생성
git tag v1.1.0
git push origin v1.1.0
```

빌드 결과물:
- `Trivo-Windows-x64.zip` (windeployqt 포함)
- `Trivo-macOS.dmg` (macdeployqt 포함)
- `Trivo-Linux-x86_64.AppImage` (linuxdeploy 포함)
