# Trivo 변경 사항 / Changelog

## 이번 업데이트 (2025-03 v2)

### 🐛 버그 수정

#### 1. 스크린샷 흰색 깜빡임 — `Viewport.cpp`
- `takeScreenshot()`에서 `makeCurrent()` / `doneCurrent()` 명시적 호출
- GL 컨텍스트 외부에서 `grabFramebuffer()` 호출하던 문제 해결

#### 2. R키 충돌 (카메라 리셋 vs 기즈모 스케일) — `Viewport.cpp`, `ToolBar.cpp`
- Scale 기즈모 단축키를 **R → T** 로 변경
- R = 카메라 리셋 전용, Q/W/E/T = 기즈모 전용

#### 3. 모델 선택 안 됨 (Picking 개선) — `Viewport.cpp`
- `raySphereIntersect`에서 tOut 반환 추가, 가장 가까운 모델 선택
- 구 반지름을 `scale * 1.8f`로 더 넉넉하게 조정

#### 4. 기즈모가 안 보임 — `Viewport.cpp`, `Renderer.cpp`
- 독립 GL 셰이더(`GIZMO_VERT/FRAG`)로 기즈모 선 오버레이 직접 드로우
- `glDisable(GL_DEPTH_TEST)`로 모델 위에 항상 보이게 처리
- 이동/크기 → 3색 화살표 (X=빨강, Y=초록, Z=파랑)
- 회전 → 3색 원형 링 (16세그먼트)

---

### ✨ 새 기능

#### 5. 조명 밝기 + 그림자 → 환경 탭으로 이동 — `Sidebar.cpp`, `ToolBar.cpp`
- 툴바에서 조명 슬라이더 제거
- **환경(Env)탭**에:
  - `조명 밝기 / Light Intensity` — 슬라이더 0.25× ~ 8.0×, 기본 **4.0×**
  - `그림자 밝기 / Ambient` — 슬라이더 0% ~ 100%, 기본 15%
  - `소프트 그림자 / Soft Shadows` — 체크 시 Ambient 40%로 자동 설정

#### 6. 조명 기본값 4배, 범위 1/4 ~ 8배
- `Scene::m_lightMult` 기본값 4.0f
- 슬라이더 범위: 25~800 (= 0.25× ~ 8.0×)

#### 7. 캡처 후 미리보기 + 폴더 열기 — `MainWindow.cpp`
- 스크린샷 저장 즉시 비모달 다이얼로그 팝업
- 썸네일 미리보기 (480×300 fit)
- **이미지 열기** — OS 기본 이미지 뷰어로 직접 오픈
- **폴더 열기** — 저장 폴더를 탐색기/파인더로 열기

#### 8. 기본 3D 뷰어 등록 — `MainWindow.cpp`
- `파일 → 기본 3D 뷰어로 설정` 메뉴 추가
- **Windows**: HKCU 레지스트리에 모든 지원 확장자 등록 + 셸 아이콘 연결
- **Linux**: `xdg-mime` + `~/.local/share/applications/trivo.desktop` 생성
- **macOS**: Finder 우클릭 안내 메시지

#### 9. ICO 파일 아이콘 실제 사용 — `IconProvider.cpp`
- `resources/icons/ext_*.ico` 를 QRC 통해 실제로 로드
- Qt 리소스 시스템 → ICO 파일이 없을 때만 프로그래매틱 배지 fallback

#### 10. 도움말에 아이콘 출처 탭 추가 — `AboutDialog.cpp`
- `아이콘 출처 / Assets` 탭 신설
- 모든 ICO 파일 목록 + 출처 표시 (자체 제작 / 외부 등)
- 이모지·폰트 출처 명시

---

### 🔑 단축키 정리

| 키 | 동작 |
|----|------|
| `R` | 카메라 리셋 (R에서 기즈모 충돌 제거) |
| `Q` | 기즈모 없음 |
| `W` | 이동 기즈모 |
| `E` | 회전 기즈모 |
| `T` | 크기 기즈모 (Scale) — 이전 R에서 변경 |
| `F` | 씬에 맞춤 |
| `Space` | 애니메이션 재생/정지 |
| `Ctrl+O` | 파일 열기 |
| `Ctrl+Shift+O` | 씬에 추가 |
| `Ctrl+P` | 스크린샷 |
| `Ctrl+T` | 다크/라이트 전환 |

---

### 📁 수정된 파일

| 파일 | 변경 |
|------|------|
| `src/core/Viewport.cpp/.h` | 기즈모 GL 오버레이, 픽킹 수정, R키 분리, 깜빡임 수정 |
| `src/core/Scene.h/.cpp` | ambientStrength 추가, 기본 lightMult=4.0 |
| `src/core/MainWindow.cpp/.h` | 스크린샷 미리보기 다이얼로그, 기본앱 등록 메뉴 |
| `src/ui/Sidebar.h/.cpp` | 조명 밝기+그림자 슬라이더 환경탭 추가 |
| `src/ui/ToolBar.h/.cpp` | 조명 슬라이더 제거, Scale 단축키 T로 변경 |
| `src/ui/AboutDialog.cpp` | 아이콘 출처 탭(Assets) 추가 |
| `src/utils/IconProvider.cpp/.h` | ICO 파일 실제 로드 우선, 배지 fallback |
| `src/main.cpp` | ICO 기반 앱 아이콘 |
