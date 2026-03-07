# Trivo 변경 사항 / Changelog

## 이번 업데이트 (2025-03)

### 🐛 버그 수정

#### 1. 다크 모드 글자 가독성 문제 — `AboutDialog.cpp`
- `QTabBar`, `QTextBrowser`, `QLabel` 등 모든 위젯에 테마 기반 색상 명시
- 탭 헤더(정보 / 라이브러리 / 라이선스 전문)가 다크 모드에서 안 보이던 문제 해결
- `setStyleSheet()`로 dialog 전체에 일관된 `color` 적용

#### 2. 3D 포맷 아이콘 생성 안 됨 — `IconProvider.cpp`
- SVG 리소스 파일 없이도 동작하도록 **프로그래매틱 배지 아이콘** 생성으로 변경
- 포맷별 고유 색상: GLB(파랑), FBX(빨강), OBJ(초록), BLEND(분홍), CAD류(보라) 등
- `QPainter`로 둥근 배지 위에 확장자 텍스트를 렌더링 — SVG 파일 의존성 제거

---

### ✨ 새 기능

#### 3. 언어 전환 제거 — `ToolBar.cpp / .h`
- 언어 콤보박스 제거, 한국어/영어 혼합 고정 표기 유지

#### 4. Unity 스타일 씬 오브젝트 조작 — `Viewport.cpp / .h`
- 씬에서 **좌클릭**으로 모델 선택 (ray-sphere picking)
- 툴바의 기즈모 버튼 또는 단축키로 조작 모드 전환:
  - `✖` / `Q` — 선택 해제 (카메라 이동 모드)
  - `↔` / `W` — **이동 (Translate)**
  - `↻` / `E` — **회전 (Rotate)**
  - `⤢` / `R` — **크기 (Scale)**
- 선택된 모델은 황금색으로 하이라이트

#### 5. 모델 선택 시 정보 표시 — `ScenePanel.cpp / .h`
- 씬 패널에 선택된 모델의 상세 정보 표시:
  - 파일명, 확장자, 파일 크기
  - 버텍스 수, 삼각형 수, 메시 수, 재질 수, 본 수
  - 애니메이션 여부 (🎬)
- 뷰포트 클릭 선택과 사이드바 목록 선택 양방향 동기화

#### 6. 스크린샷 씬 뷰만 캡처 — `Viewport.cpp`, `MainWindow.cpp`
- `m_viewport->takeScreenshot()` — `grabFramebuffer()` 사용
- UI 크롬(툴바, 사이드바) 제외하고 OpenGL 씬만 PNG로 저장

#### 7. 조명 밝기 조정 (최대 4배) — `ToolBar.cpp`, `Scene.h/.cpp`, `Renderer.cpp`
- 툴바에 `💡` 슬라이더 추가 (0× ~ 4×, 기본 1×)
- `Scene::lightIntensityMultiplier` 필드로 관리
- `Renderer::bindLights()`에서 `l.intensity * mult` 적용

#### 8. 텍스처 보이기 / 와이어프레임 모드 — `ToolBar.cpp`, `Scene.h/.cpp`, `Renderer.cpp`
- 툴바 `🖼` 버튼 토글
  - **ON** — 기본 PBR 렌더링
  - **OFF** — 중성 회색 솔리드 + 파란 와이어프레임 오버레이 (선택 시 황금색)
- GLSL `uRenderMode` uniform으로 shader 내 분기

---

### 📁 수정된 파일 목록

| 파일 | 변경 내용 |
|------|-----------|
| `src/ui/AboutDialog.cpp` | 다크모드 색상 전체 수정 |
| `src/utils/IconProvider.h/.cpp` | 프로그래매틱 배지 아이콘 |
| `src/ui/ToolBar.h/.cpp` | 기즈모 버튼, 텍스처 토글, 밝기 슬라이더, 언어 전환 제거 |
| `src/core/Scene.h/.cpp` | selectedIndex, textureVisible, lightIntensityMultiplier 추가 |
| `src/core/Renderer.h/.cpp` | uRenderMode, uSelectionHighlight, 밝기 배수, 와이어프레임 셰이더 |
| `src/core/Viewport.h/.cpp` | ray picking, gizmo drag, 씬뷰 스크린샷 |
| `src/ui/ScenePanel.h/.cpp` | 모델 정보 패널, 양방향 선택 동기화 |
| `src/ui/Sidebar.h/.cpp` | syncSelection() 추가 |
| `src/core/MainWindow.h/.cpp` | 모든 새 시그널 연결, 스크린샷 씬뷰 전용 |

---

### ⌨️ 단축키 요약

| 키 | 동작 |
|----|------|
| `Q` | 기즈모 없음 (카메라 모드) |
| `W` | 이동 기즈모 |
| `E` | 회전 기즈모 |
| `R` | 카메라 리셋 |
| `F` | 씬에 맞춤 |
| `Space` | 애니메이션 재생/정지 |
| `Ctrl+O` | 파일 열기 |
| `Ctrl+Shift+O` | 씬에 추가 |
| `Ctrl+P` | 스크린샷 (씬 뷰만) |
| `Ctrl+T` | 다크/라이트 전환 |
