#include "AboutDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <QFont>
#include <QIcon>

// ─────────────────────────────────────────────────────────────────────────────
//  Full library + license text
// ─────────────────────────────────────────────────────────────────────────────
static const char *LICENSE_FULL = R"LICENSE(
══════════════════════════════════════════════════════════════════════════════
  Trivo 사용 라이브러리 및 라이선스 전문
  Third-Party Libraries & Full License Texts
══════════════════════════════════════════════════════════════════════════════


──────────────────────────────────────────────────────────────────────────────
1.  Qt Framework (Qt 6)
    URL     : https://www.qt.io
    버전    : Qt 6.4 이상 / Qt 6.4 or later
    라이선스: GNU Lesser General Public License v3 (LGPLv3)
    사용 모듈 / Modules:
              Qt::Core  Qt::Widgets  Qt::Gui
              Qt::OpenGL  Qt::OpenGLWidgets  Qt::Concurrent

    Trivo links Qt as a shared (dynamic) library in compliance with LGPLv3.
    The complete LGPLv3 license text is reproduced below.

    GNU LESSER GENERAL PUBLIC LICENSE
    Version 3, 29 June 2007

    Copyright (C) 2007 Free Software Foundation, Inc.
    <https://fsf.org/>

    Everyone is permitted to copy and distribute verbatim copies of this
    license document, but changing it is not allowed.

    This version of the GNU Lesser General Public License incorporates the
    terms and conditions of version 3 of the GNU General Public License,
    supplemented by the additional permissions listed below.

    0. Additional Definitions.
    As used herein, "this License" refers to version 3 of the GNU Lesser
    General Public License, and the "GNU GPL" refers to version 3 of the
    GNU General Public License.
    "The Library" refers to a covered work governed by this License, other
    than an Application or a Combined Work as defined below.
    [... full text available at https://www.gnu.org/licenses/lgpl-3.0.txt ...]


──────────────────────────────────────────────────────────────────────────────
2.  Open Asset Import Library (Assimp)
    URL     : https://github.com/assimp/assimp
    버전    : 5.x
    라이선스: BSD 3-Clause "New" or "Revised" License

    Copyright (c) 2006-2024, assimp team
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    3. Neither the name of the assimp team nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


──────────────────────────────────────────────────────────────────────────────
3.  zlib
    URL     : https://www.zlib.net
    버전    : 1.3.x
    라이선스: zlib/libpng License

    Copyright (C) 1995-2024 Jean-loup Gailly and Mark Adler

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software in
       a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.


──────────────────────────────────────────────────────────────────────────────
4.  pugixml (Assimp dependency)
    URL     : https://pugixml.org
    버전    : 1.14
    라이선스: MIT License

    Copyright (c) 2006-2023 Arseny Kapoulkine

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.


──────────────────────────────────────────────────────────────────────────────
5.  minizip / minizip-ng (Assimp dependency)
    URL     : https://github.com/zlib-ng/minizip-ng
    라이선스: zlib License
    (same terms as zlib above)


──────────────────────────────────────────────────────────────────────────────
6.  poly2tri (Assimp dependency — polygon triangulation)
    URL     : https://github.com/jhasse/poly2tri
    라이선스: BSD 3-Clause License

    Copyright (c) 2009-2018, Poly2Tri Contributors
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Poly2Tri nor the names of its contributors may be
      used to endorse or promote products derived from this software without
      specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED.


──────────────────────────────────────────────────────────────────────────────
7.  OpenGL
    URL     : https://www.opengl.org
    버전    : 4.1 Core Profile
    라이선스: Provided by GPU vendors (no separate redistribution required)
              The OpenGL API specification is royalty-free.

    Trivo uses Qt's OpenGL abstraction layer (QOpenGLFunctions_4_1_Core)
    which links against the system-provided OpenGL implementation.


──────────────────────────────────────────────────────────────────────────────
8.  CMake
    URL     : https://cmake.org
    버전    : 3.20 이상
    라이선스: BSD 3-Clause License
    사용 목적: Build system (not redistributed with the application)


──────────────────────────────────────────────────────────────────────────────
9.  vcpkg (Windows dependency management)
    URL     : https://github.com/microsoft/vcpkg
    라이선스: MIT License
    사용 목적: Package manager for CI (not redistributed with the application)


──────────────────────────────────────────────────────────────────────────────
  Trivo 자체 라이선스 / Trivo's Own License
──────────────────────────────────────────────────────────────────────────────

MIT License

Copyright (c) 2024 Trivo Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

══════════════════════════════════════════════════════════════════════════════
)LICENSE";

// ─────────────────────────────────────────────────────────────────────────────

AboutDialog::AboutDialog(bool dark, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Trivo 정보 / About Trivo");
    setMinimumSize(620, 560);
    setModal(true);

    // ── Outer layout ──────────────────────────────────────────────────────────
    auto *outer = new QVBoxLayout(this);
    outer->setSpacing(12);
    outer->setContentsMargins(16, 16, 16, 16);

    // ── Header ────────────────────────────────────────────────────────────────
    auto *header = new QWidget(this);
    header->setObjectName("about_header");
    header->setStyleSheet(dark
        ? "#about_header { background:#141418; border-radius:10px; }"
        : "#about_header { background:#eef2ff; border-radius:10px; }");

    auto *hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(20, 16, 20, 16);

    // App icon (text-based cube)
    auto *iconLabel = new QLabel("🧊", header);
    iconLabel->setStyleSheet("font-size:48px;");
    hLayout->addWidget(iconLabel);

    auto *titleBlock = new QVBoxLayout;
    auto *title = new QLabel("<b style='font-size:22px;color:#3a7bd5;'>Trivo</b>", header);
    title->setTextFormat(Qt::RichText);
    auto *sub = new QLabel("Version 1.0.0  |  MIT License", header);
    sub->setStyleSheet(dark ? "color:#aaa;font-size:12px;" : "color:#555;font-size:12px;");
    titleBlock->addWidget(title);
    titleBlock->addWidget(sub);

    hLayout->addLayout(titleBlock);
    hLayout->addStretch();
    outer->addWidget(header);

    // ── Tabs ──────────────────────────────────────────────────────────────────
    auto *tabs = new QTabWidget(this);

    // ── Tab 1: Info ────────────────────────────────────────────────────────
    auto *infoW = new QWidget(tabs);
    auto *infoL = new QVBoxLayout(infoW);
    infoL->setContentsMargins(0,8,0,0);

    auto *infoBrowser = new QTextBrowser(infoW);
    infoBrowser->setOpenExternalLinks(true);
    infoBrowser->setHtml(R"HTML(
<style>
  body { font-family: sans-serif; font-size: 13px; }
  h3   { color: #3a7bd5; margin-bottom: 4px; }
  li   { margin-bottom: 3px; }
  hr   { border: none; border-top: 1px solid #444; margin: 10px 0; }
  .fmt { font-family: monospace; background: rgba(58,123,213,0.15);
         padding: 1px 4px; border-radius: 3px; }
</style>

<h3>✨ 주요 기능 / Features</h3>
<ul>
  <li>30+ 3D 파일 형식 지원 / 30+ 3D format support</li>
  <li>PBR 물리 기반 렌더링 / PBR Rendering (OpenGL 4.1 Core)</li>
  <li>드래그 앤 드롭 / Drag &amp; Drop</li>
  <li>여러 모델을 하나의 씬에 배치 / Multi-model scene</li>
  <li>애니메이션 재생 + 속도 조절 / Animation playback + speed control</li>
  <li>5가지 라이트 프리셋 / 5 Light presets (Studio · Outdoor · Dramatic · Soft · Night)</li>
  <li>다크 / 라이트 테마 / Dark &amp; Light theme</li>
  <li>스크린샷 저장 / Screenshot export (PNG)</li>
  <li>스크린샷 저장 위치: 문서/Trivo 폴더 / Screenshot save location: Documents/Trivo folder</li>
  <li>그리드 배경 / Grid background</li>
</ul>

<hr>

<h3>📂 지원 형식 / Supported Formats</h3>
<table cellspacing="4">
<tr>
  <td><b>모던 / Modern</b></td>
  <td><span class="fmt">GLB</span> <span class="fmt">GLTF</span></td>
</tr>
<tr>
  <td><b>게임 엔진 / Game</b></td>
  <td><span class="fmt">FBX</span> <span class="fmt">X</span> <span class="fmt">MD2</span> <span class="fmt">MD3</span></td>
</tr>
<tr>
  <td><b>범용 / General</b></td>
  <td><span class="fmt">OBJ</span> <span class="fmt">DAE</span> <span class="fmt">3DS</span> <span class="fmt">PLY</span> <span class="fmt">STL</span></td>
</tr>
<tr>
  <td><b>DCC 툴 / DCC</b></td>
  <td><span class="fmt">BLEND</span> <span class="fmt">LWO</span> <span class="fmt">LWS</span> <span class="fmt">MS3D</span></td>
</tr>
<tr>
  <td><b>CAD / Engineering</b></td>
  <td><span class="fmt">STEP</span> <span class="fmt">STP</span> <span class="fmt">IGES</span> <span class="fmt">IFC</span> <span class="fmt">DXF</span></td>
</tr>
<tr>
  <td><b>애니메이션 / Anim</b></td>
  <td><span class="fmt">BVH</span> <span class="fmt">SMD</span> <span class="fmt">VTA</span></td>
</tr>
<tr>
  <td><b>포인트 클라우드</b></td>
  <td><span class="fmt">PCD</span> <span class="fmt">XYZ</span> <span class="fmt">PTS</span></td>
</tr>
<tr>
  <td><b>기타 / Misc</b></td>
  <td><span class="fmt">AC</span> <span class="fmt">OFF</span> <span class="fmt">NFF</span> <span class="fmt">COB</span> <span class="fmt">TER</span> 외 / more…</td>
</tr>
</table>

<hr>

<h3>⌨️ 단축키 / Keyboard Shortcuts</h3>
<table cellspacing="4">
<tr><td><b>R</b></td><td>카메라 초기화 / Reset camera</td></tr>
<tr><td><b>F</b></td><td>씬에 맞춤 / Fit to scene</td></tr>
<tr><td><b>Space</b></td><td>애니메이션 재생/정지 / Play/Pause</td></tr>
<tr><td><b>Ctrl+O</b></td><td>파일 열기 / Open file</td></tr>
<tr><td><b>Ctrl+Shift+O</b></td><td>씬에 추가 / Add to scene</td></tr>
<tr><td><b>Ctrl+P</b></td><td>스크린샷 / Screenshot</td></tr>
<tr><td><b>Ctrl+T</b></td><td>테마 전환 / Toggle theme</td></tr>
</table>

<hr>
<p>
  🐙 <b>GitHub:</b>
  <a href="https://github.com/reders0412-rgb/Trivo">https://github.com/reders0412-rgb/Trivo</a>
</p>
)HTML");

    infoL->addWidget(infoBrowser);
    tabs->addTab(infoW, "정보 / Info");

    // ── Tab 2: Libraries ────────────────────────────────────────────────────
    auto *libW = new QWidget(tabs);
    auto *libL = new QVBoxLayout(libW);
    libL->setContentsMargins(0,8,0,0);

    auto *libBrowser = new QTextBrowser(libW);
    libBrowser->setOpenExternalLinks(true);
    libBrowser->setHtml(R"HTML(
<style>
  body { font-family: sans-serif; font-size:13px; }
  h3   { color:#3a7bd5; }
  .lib { background:rgba(58,123,213,0.1); border-radius:6px; padding:8px 12px; margin-bottom:8px; }
  .badge { display:inline-block; background:#3a7bd5; color:white;
           padding:1px 7px; border-radius:10px; font-size:11px; }
  a    { color:#5aa0ff; }
</style>

<div class="lib">
  <b>Qt Framework (Qt6)</b>
  <span class="badge">LGPL v3</span><br>
  사용 모듈: Core · Widgets · Gui · OpenGL · OpenGLWidgets · Concurrent<br>
  <a href="https://www.qt.io">https://www.qt.io</a>
</div>

<div class="lib">
  <b>Open Asset Import Library (Assimp) 5.x</b>
  <span class="badge">BSD 3-Clause</span><br>
  30+ 3D 파일 형식 파싱 / Parses 30+ 3D file formats<br>
  <a href="https://github.com/assimp/assimp">https://github.com/assimp/assimp</a>
</div>

<div class="lib">
  <b>zlib 1.3</b>
  <span class="badge">zlib License</span><br>
  Assimp 압축 의존성 / Compression dependency of Assimp<br>
  <a href="https://www.zlib.net">https://www.zlib.net</a>
</div>

<div class="lib">
  <b>pugixml 1.14</b>
  <span class="badge">MIT</span><br>
  Assimp XML 파싱 의존성 / XML parsing dependency of Assimp<br>
  <a href="https://pugixml.org">https://pugixml.org</a>
</div>

<div class="lib">
  <b>minizip-ng</b>
  <span class="badge">zlib License</span><br>
  Assimp ZIP 처리 의존성 / ZIP handling dependency of Assimp<br>
  <a href="https://github.com/zlib-ng/minizip-ng">https://github.com/zlib-ng/minizip-ng</a>
</div>

<div class="lib">
  <b>poly2tri</b>
  <span class="badge">BSD 3-Clause</span><br>
  Assimp 폴리곤 삼각화 의존성 / Polygon triangulation dependency<br>
  <a href="https://github.com/jhasse/poly2tri">https://github.com/jhasse/poly2tri</a>
</div>

<div class="lib">
  <b>OpenGL 4.1 Core Profile</b>
  <span class="badge">Vendor</span><br>
  시스템 GPU 드라이버 제공 / Provided by system GPU driver<br>
  Qt QOpenGLFunctions_4_1_Core 래퍼를 통해 사용<br>
  <a href="https://www.opengl.org">https://www.opengl.org</a>
</div>

<div class="lib">
  <b>CMake 3.20+</b>
  <span class="badge">BSD 3-Clause</span><br>
  빌드 시스템 (앱에 포함되지 않음) / Build system (not bundled)<br>
  <a href="https://cmake.org">https://cmake.org</a>
</div>

<div class="lib">
  <b>vcpkg</b>
  <span class="badge">MIT</span><br>
  Windows CI 패키지 관리 (앱에 포함되지 않음) / Windows CI package manager<br>
  <a href="https://github.com/microsoft/vcpkg">https://github.com/microsoft/vcpkg</a>
</div>
)HTML");
    libL->addWidget(libBrowser);
    tabs->addTab(libW, "라이브러리 / Libraries");

    // ── Tab 3: Full License Text ─────────────────────────────────────────────
    auto *licW = new QWidget(tabs);
    auto *licL = new QVBoxLayout(licW);
    licL->setContentsMargins(0,8,0,0);

    auto *licBrowser = new QTextBrowser(licW);
    QFont mono("Courier New", 10);
    mono.setStyleHint(QFont::Monospace);
    licBrowser->setFont(mono);
    licBrowser->setPlainText(LICENSE_FULL);
    licL->addWidget(licBrowser);
    tabs->addTab(licW, "라이선스 전문 / Full Licenses");

    outer->addWidget(tabs, 1);

    // ── Close button ──────────────────────────────────────────────────────────
    auto *btnRow = new QHBoxLayout;
    btnRow->addStretch();
    auto *closeBtn = new QPushButton("  닫기 / Close  ", this);
    closeBtn->setDefault(true);
    closeBtn->setMinimumWidth(120);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnRow->addWidget(closeBtn);
    outer->addLayout(btnRow);
}
