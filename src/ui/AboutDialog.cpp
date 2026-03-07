#include "AboutDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <QFont>
#include <QIcon>

static const char *LICENSE_FULL = R"LICENSE(
══════════════════════════════════════════════════════════════════════════════
  Trivo 사용 라이브러리 및 라이선스 전문
  Third-Party Libraries & Full License Texts
══════════════════════════════════════════════════════════════════════════════

1.  Qt Framework (Qt 6)  —  LGPLv3
    https://www.qt.io

2.  Open Asset Import Library (Assimp) 5.x  —  BSD 3-Clause
    https://github.com/assimp/assimp

3.  zlib 1.3  —  zlib/libpng License
    https://www.zlib.net

4.  pugixml 1.14  —  MIT License
    https://pugixml.org

5.  minizip-ng  —  zlib License
    https://github.com/zlib-ng/minizip-ng

6.  poly2tri  —  BSD 3-Clause License
    https://github.com/jhasse/poly2tri

7.  OpenGL 4.1 Core Profile  —  Vendor (GPU driver)
    https://www.opengl.org

8.  CMake 3.20+  —  BSD 3-Clause  (build system, not bundled)
    https://cmake.org

9.  vcpkg  —  MIT  (CI package manager, not bundled)
    https://github.com/microsoft/vcpkg

──────────────────────────────────────────────────────────────────────────────
  Trivo 자체 라이선스 / Trivo's Own License  —  MIT License
──────────────────────────────────────────────────────────────────────────────

Copyright (c) 2024 Trivo Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.

══════════════════════════════════════════════════════════════════════════════
)LICENSE";

AboutDialog::AboutDialog(bool dark, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Trivo 정보 / About Trivo");
    setMinimumSize(660, 600);
    setModal(true);

    const QString textColor  = dark ? "#e8e8ec" : "#1a1a2e";
    const QString mutedColor = dark ? "#aaa"    : "#555";
    const QString bgColor    = dark ? "#1a1a1f" : "#f5f5f8";
    const QString headerBg   = dark ? "#141418" : "#eef2ff";
    const QString tabBg      = dark ? "#1a1a1f" : "#f5f5f8";
    const QString tabSelBg   = dark ? "#2d2d38" : "#ffffff";
    const QString tabBorder  = dark ? "#2a2a32" : "#ddd";
    const QString browserBg  = dark ? "#1a1a1f" : "#ffffff";
    const QString browserFg  = dark ? "#e8e8ec" : "#1a1a2e";
    const QString linkColor  = dark ? "#5aa0ff" : "#3a7bd5";
    const QString monoBg     = dark ? "#141418" : "#f0f0f4";
    const QString monoFg     = dark ? "#d0d0d8" : "#1a1a2e";

    setStyleSheet(QString(
        "QDialog { background: %1; color: %2; }"
        "QTabWidget::pane { border: 1px solid %3; background: %4; }"
        "QTabBar::tab { background: %4; color: %2; border: 1px solid %3;"
        "  border-bottom: none; padding: 6px 14px; margin-right: 2px; border-radius: 4px 4px 0 0; }"
        "QTabBar::tab:selected { background: %5; color: %2; }"
        "QTabBar::tab:hover { background: %5; }"
        "QTextBrowser { background: %6; color: %7; border: none; }"
        "QScrollBar:vertical { background: %4; width: 8px; }"
        "QScrollBar::handle:vertical { background: %3; border-radius: 4px; min-height: 20px; }"
        "QPushButton { background: %4; color: %2; border: 1px solid %3;"
        "  border-radius: 6px; padding: 6px 14px; }"
        "QPushButton:hover { background: #3a7bd5; color: white; border-color: #3a7bd5; }"
        "QPushButton:default { background: #3a7bd5; color: white; border-color: #3a7bd5; }"
        "QLabel { color: %2; }"
    ).arg(bgColor, textColor, tabBorder, tabBg, tabSelBg, browserBg, browserFg));

    const QString browserCSS = QString(
        "body { font-family: sans-serif; font-size: 13px; background: %1; color: %2; margin: 8px; }"
        "h3   { color: #3a7bd5; margin-bottom: 4px; }"
        "li   { margin-bottom: 3px; }"
        "hr   { border: none; border-top: 1px solid %3; margin: 12px 0; }"
        ".fmt { font-family: monospace; background: rgba(58,123,213,0.15);"
        "       padding: 1px 4px; border-radius: 3px; color: %2; }"
        ".lib { background: rgba(58,123,213,0.10); border-radius: 6px;"
        "       padding: 8px 12px; margin-bottom: 8px; }"
        ".badge { display:inline-block; background:#3a7bd5; color:white;"
        "         padding:1px 7px; border-radius:10px; font-size:11px; }"
        ".ico-table td { padding: 4px 8px; color: %2; vertical-align: middle; }"
        ".ico-table .ext { font-family: monospace; font-weight: bold;"
        "   background: rgba(58,123,213,0.15); border-radius:4px; padding:1px 5px; }"
        ".ico-table .src { color: %4; font-size: 11px; }"
        "a { color: %4; }"
        "b { color: %2; }"
        "td { color: %2; }"
    ).arg(browserBg, textColor, tabBorder, linkColor);

    auto *outer = new QVBoxLayout(this);
    outer->setSpacing(12);
    outer->setContentsMargins(16, 16, 16, 12);

    // ── Header ────────────────────────────────────────────────────────────────
    auto *header = new QWidget(this);
    header->setObjectName("about_header");
    header->setStyleSheet(QString("#about_header { background: %1; border-radius: 10px; }").arg(headerBg));
    auto *hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(20, 16, 20, 16);
    auto *iconLabel = new QLabel("🧊", header);
    iconLabel->setStyleSheet("font-size:48px;");
    hLayout->addWidget(iconLabel);
    auto *titleBlock = new QVBoxLayout;
    auto *title = new QLabel("<b style='font-size:22px;color:#3a7bd5;'>Trivo</b>", header);
    title->setTextFormat(Qt::RichText);
    auto *sub = new QLabel("Version 1.0.0  |  MIT License", header);
    sub->setStyleSheet(QString("color:%1;font-size:12px;").arg(mutedColor));
    titleBlock->addWidget(title);
    titleBlock->addWidget(sub);
    hLayout->addLayout(titleBlock);
    hLayout->addStretch();
    outer->addWidget(header);

    auto *tabs = new QTabWidget(this);

    // ── Tab 1: Info ────────────────────────────────────────────────────────
    auto *infoW = new QWidget(tabs);
    auto *infoL = new QVBoxLayout(infoW);
    infoL->setContentsMargins(0,8,0,0);
    auto *infoBrowser = new QTextBrowser(infoW);
    infoBrowser->setOpenExternalLinks(true);
    infoBrowser->setHtml(QString(R"HTML(
<style>%1</style>
<h3>✨ 주요 기능 / Features</h3>
<ul>
  <li>30+ 3D 파일 형식 지원 / 30+ 3D format support</li>
  <li>PBR 물리 기반 렌더링 (OpenGL 4.1 Core) / PBR Rendering</li>
  <li>드래그 앤 드롭 / Drag &amp; Drop</li>
  <li>씬에서 좌클릭으로 모델 선택 / Left-click to select model in scene</li>
  <li>Q/W/E/T 기즈모 — 이동 / 회전 / 크기 (Unity 스타일)</li>
  <li>애니메이션 재생 + 속도 조절 / Animation playback + speed</li>
  <li>조명 프리셋 5종 + 밝기(0.25×~8×) + 그림자 조절 / 5 Light presets + intensity + ambient</li>
  <li>텍스처 PBR ↔ 와이어프레임 토글 / PBR ↔ Wireframe toggle</li>
  <li>다크 / 라이트 테마 / Dark &amp; Light theme</li>
  <li>스크린샷 (씬 뷰만) + 미리보기 다이얼로그 / Screenshot — scene only + preview</li>
  <li>기본 3D 뷰어로 등록 / Register as default 3D viewer</li>
</ul>
<hr>
<h3>📂 지원 형식 / Supported Formats</h3>
<table cellspacing="4">
<tr><td><b>모던</b></td><td><span class="fmt">GLB</span> <span class="fmt">GLTF</span></td></tr>
<tr><td><b>게임</b></td><td><span class="fmt">FBX</span> <span class="fmt">X</span> <span class="fmt">MD2</span> <span class="fmt">MD3</span></td></tr>
<tr><td><b>범용</b></td><td><span class="fmt">OBJ</span> <span class="fmt">DAE</span> <span class="fmt">3DS</span> <span class="fmt">PLY</span> <span class="fmt">STL</span></td></tr>
<tr><td><b>DCC</b></td><td><span class="fmt">BLEND</span> <span class="fmt">LWO</span> <span class="fmt">MS3D</span></td></tr>
<tr><td><b>CAD</b></td><td><span class="fmt">STEP</span> <span class="fmt">IGES</span> <span class="fmt">IFC</span> <span class="fmt">DXF</span></td></tr>
<tr><td><b>애니메이션</b></td><td><span class="fmt">BVH</span> <span class="fmt">SMD</span> <span class="fmt">VTA</span></td></tr>
<tr><td><b>포인트 클라우드</b></td><td><span class="fmt">PCD</span> <span class="fmt">XYZ</span> <span class="fmt">PTS</span></td></tr>
</table>
<hr>
<h3>⌨️ 단축키 / Shortcuts</h3>
<table cellspacing="4">
<tr><td><b>R</b></td><td>카메라 리셋</td></tr>
<tr><td><b>F</b></td><td>씬에 맞춤</td></tr>
<tr><td><b>Q / W / E / T</b></td><td>기즈모: 없음 / 이동 / 회전 / 크기</td></tr>
<tr><td><b>Space</b></td><td>애니메이션 재생/정지</td></tr>
<tr><td><b>Ctrl+O</b></td><td>파일 열기</td></tr>
<tr><td><b>Ctrl+Shift+O</b></td><td>씬에 추가</td></tr>
<tr><td><b>Ctrl+P</b></td><td>스크린샷</td></tr>
<tr><td><b>Ctrl+T</b></td><td>다크/라이트 전환</td></tr>
</table>
<hr>
<p>🐙 <b>GitHub:</b> <a href="https://github.com/reders0412-rgb/Trivo">https://github.com/reders0412-rgb/Trivo</a></p>
)HTML").arg(browserCSS));
    infoL->addWidget(infoBrowser);
    tabs->addTab(infoW, "정보 / Info");

    // ── Tab 2: Libraries ────────────────────────────────────────────────────
    auto *libW = new QWidget(tabs);
    auto *libL = new QVBoxLayout(libW);
    libL->setContentsMargins(0,8,0,0);
    auto *libBrowser = new QTextBrowser(libW);
    libBrowser->setOpenExternalLinks(true);
    libBrowser->setHtml(QString(R"HTML(
<style>%1</style>
<div class="lib"><b>Qt Framework (Qt6)</b> <span class="badge">LGPL v3</span><br>
Core · Widgets · Gui · OpenGL · OpenGLWidgets · Concurrent<br>
<a href="https://www.qt.io">https://www.qt.io</a></div>

<div class="lib"><b>Open Asset Import Library (Assimp) 5.x</b> <span class="badge">BSD 3-Clause</span><br>
30+ 3D 파일 형식 파싱 / Parses 30+ 3D formats<br>
<a href="https://github.com/assimp/assimp">https://github.com/assimp/assimp</a></div>

<div class="lib"><b>zlib 1.3</b> <span class="badge">zlib License</span><br>
Assimp 압축 의존성 / Compression<br>
<a href="https://www.zlib.net">https://www.zlib.net</a></div>

<div class="lib"><b>pugixml 1.14</b> <span class="badge">MIT</span><br>
Assimp XML 파싱 의존성<br>
<a href="https://pugixml.org">https://pugixml.org</a></div>

<div class="lib"><b>minizip-ng</b> <span class="badge">zlib License</span><br>
<a href="https://github.com/zlib-ng/minizip-ng">https://github.com/zlib-ng/minizip-ng</a></div>

<div class="lib"><b>poly2tri</b> <span class="badge">BSD 3-Clause</span><br>
폴리곤 삼각화 / Polygon triangulation<br>
<a href="https://github.com/jhasse/poly2tri">https://github.com/jhasse/poly2tri</a></div>

<div class="lib"><b>OpenGL 4.1 Core Profile</b> <span class="badge">Vendor</span><br>
시스템 GPU 드라이버 제공<br>
<a href="https://www.opengl.org">https://www.opengl.org</a></div>

<div class="lib"><b>CMake 3.20+</b> <span class="badge">BSD 3-Clause</span><br>
빌드 시스템 (앱에 포함 안 됨)<br>
<a href="https://cmake.org">https://cmake.org</a></div>

<div class="lib"><b>vcpkg</b> <span class="badge">MIT</span><br>
Windows CI 패키지 관리 (앱에 포함 안 됨)<br>
<a href="https://github.com/microsoft/vcpkg">https://github.com/microsoft/vcpkg</a></div>
)HTML").arg(browserCSS));
    libL->addWidget(libBrowser);
    tabs->addTab(libW, "라이브러리 / Libraries");

    // ── Tab 3: Icons & Assets ─────────────────────────────────────────────────
    auto *assetsW = new QWidget(tabs);
    auto *assetsL = new QVBoxLayout(assetsW);
    assetsL->setContentsMargins(0,8,0,0);
    auto *assetsBrowser = new QTextBrowser(assetsW);
    assetsBrowser->setOpenExternalLinks(true);
    assetsBrowser->setHtml(QString(R"HTML(
<style>%1</style>
<h3>🎨 아이콘 &amp; 에셋 출처 / Icons &amp; Assets</h3>
<p>Trivo는 아래 아이콘 에셋을 사용합니다. 각 아이콘은 해당 라이선스에 따라 사용됩니다.</p>

<hr>
<h3>📂 3D 파일 형식 아이콘 (resources/icons/ext_*.ico)</h3>
<table class="ico-table" cellspacing="2">
<tr><td><span class="ext">GLB</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">GLTF</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">FBX</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">OBJ</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">DAE</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">3DS</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">STL</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">PLY</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">BLEND</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">BVH</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">X</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">DXF</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">IFC</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">STEP / STP</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">IGES / IGS</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
<tr><td><span class="ext">Generic</span></td><td class="src">Trivo 자체 제작 / Original artwork</td></tr>
</table>

<hr>
<h3>🖼️ UI 아이콘 (resources/icons/*.ico)</h3>
<table class="ico-table" cellspacing="2">
<tr><td><span class="ext">trivo_app.ico</span></td>
    <td class="src">앱 로고 — Trivo 자체 제작 / App logo, original artwork</td></tr>
<tr><td><span class="ext">open.ico, add.ico</span></td>
    <td class="src">파일 열기/추가 — Trivo 자체 제작</td></tr>
<tr><td><span class="ext">reset.ico</span></td>
    <td class="src">카메라 리셋 — Trivo 자체 제작</td></tr>
<tr><td><span class="ext">play.ico, pause.ico</span></td>
    <td class="src">애니메이션 재생/정지 — Trivo 자체 제작</td></tr>
<tr><td><span class="ext">camera.ico</span></td>
    <td class="src">스크린샷 — Trivo 자체 제작</td></tr>
<tr><td><span class="ext">moon.ico, sun.ico</span></td>
    <td class="src">다크/라이트 테마 — Trivo 자체 제작</td></tr>
</table>

<hr>
<h3>📝 폰트 및 기타</h3>
<div class="lib">
  <b>시스템 폰트 / System Font</b><br>
  Qt 기본 시스템 폰트 사용 — 별도 임베디드 폰트 없음<br>
  Qt default system font — no embedded fonts
</div>
<div class="lib">
  <b>이모지 / Emoji</b><br>
  UI 내 이모지는 OS 제공 이모지 폰트를 사용합니다.<br>
  Emoji rendered via OS-provided emoji font (Segoe UI Emoji / Apple Color Emoji / Noto Emoji)
</div>

<hr>
<p style="font-size:11px; color:%2;">
아이콘 교체를 원하면 <code>resources/icons/</code> 폴더의 ICO 파일을 교체하고 재빌드하세요.<br>
To replace icons, swap ICO files in <code>resources/icons/</code> and rebuild.
</p>
)HTML").arg(browserCSS, mutedColor));
    assetsL->addWidget(assetsBrowser);
    tabs->addTab(assetsW, "아이콘 출처 / Assets");

    // ── Tab 4: Full Licenses ──────────────────────────────────────────────────
    auto *licW = new QWidget(tabs);
    auto *licL = new QVBoxLayout(licW);
    licL->setContentsMargins(0,8,0,0);
    auto *licBrowser = new QTextBrowser(licW);
    licBrowser->setStyleSheet(QString(
        "QTextBrowser { background: %1; color: %2; font-family: 'Courier New', monospace; font-size: 10pt; }"
    ).arg(monoBg, monoFg));
    QFont mono("Courier New", 10);
    mono.setStyleHint(QFont::Monospace);
    licBrowser->setFont(mono);
    licBrowser->setPlainText(LICENSE_FULL);
    licL->addWidget(licBrowser);
    tabs->addTab(licW, "라이선스 전문 / Full Licenses");

    outer->addWidget(tabs, 1);

    auto *btnRow = new QHBoxLayout;
    btnRow->addStretch();
    auto *closeBtn = new QPushButton("  닫기 / Close  ", this);
    closeBtn->setDefault(true);
    closeBtn->setMinimumWidth(120);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnRow->addWidget(closeBtn);
    outer->addLayout(btnRow);
}
