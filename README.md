# Carbon Engine Client (`carbon-engine-client`)

A desktop compliance workbench built with **C++20**, **Qt 6 Quick / QML**, and **Qt WebEngine** for corporate ESG disclosures and statutory SEBI BRSR analytics. Connects to local daemons or remote serverless deployments (**Google Cloud Run**).

---

## Key Capabilities

| Capability | Summary |
|---|---|
| **SEBI BRSR P1–P9 Extraction** | Coordinates extraction across statutory Environmental, Social, and Governance disclosure indicators. |
| **Remote PDF Ingestion** | In-flight base64 streaming of multi-page compliance documents to remote Cloud Run or local instances. |
| **Human-in-the-Loop Audit** | Interactive review, correction, and sign-off of extracted metrics mapped to source PDF coordinates. |
| **Pipeline Telemetry** | Real-time tracking of PDF parsing, OCR fallback passes, vector pacing, and Gemini rate limits. |
| **Dynamic Server Targeting** | Runtime switching and latency verification between local daemons and production Cloud Run instances. |

---

## Connection Configuration

| Setting | Details |
|---|---|
| **Endpoint URL** | Backend server address (`http://localhost:8080` or remote Cloud Run URL). |
| **Presets** | One-click presets for local development or remote Cloud Run deployment. |
| **Latency Verification** | Measures roundtrip network latency and backend health before applying. |
| **Persistence** | Saved automatically across sessions via system settings (`QSettings`). |

---

## Build Prerequisites & Tooling

| Component | Requirement | Scope |
|---|---|---|
| **Compiler** | MSVC 2022 (v143+) / GCC 13+ / Clang 17+ | C++20 standard compliance |
| **Build System** | CMake 3.20+ and Ninja | Native build orchestration |
| **Qt Framework** | Qt 6.10+ (Qt 6.10.2) | `Core`, `Gui`, `Qml`, `Quick`, `QuickControls2`, `Network`, `WebEngineQuick` |

### Build Commands (Windows Developer Command Prompt)

```cmd
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\6.10.2\msvc2022_64"
cmake --build build --config Release
windeployqt --qmldir src\qml --no-translations --compiler-runtime build\bin\carbon-engine-client.exe
```

The compiled standalone binary is placed at `build/bin/carbon-engine-client.exe`.

---

## High-Level Architecture

| Subsystem | Location | Description |
|---|---|---|
| **REST Clients & Pollers** | [`src/api/`](file:///E:/Code/carbonservices/carbon-engine-client/src/api) | Asynchronous HTTP clients (`UploadClient`, `ExtractionClient`, `ReportClient`) and telemetry pollers. |
| **QML UI & Workspaces** | [`src/qml/`](file:///E:/Code/carbonservices/carbon-engine-client/src/qml) | Main window, metric review panels, document dropzones, and settings dialog. |
| **Design Token System** | [`src/qml/Theme.qml`](file:///E:/Code/carbonservices/carbon-engine-client/src/qml/Theme.qml) | Centralized colors, typography, spacing tokens, and light/dark theme definitions. |
| **Diagnostic Logging** | [`src/logging/`](file:///E:/Code/carbonservices/carbon-engine-client/src/logging) | Thread-safe rotating logger writing diagnostics to `%USERPROFILE%\Documents\CarbonESG\logs\`. |
| **Common Constants** | [`src/common/`](file:///E:/Code/carbonservices/carbon-engine-client/src/common) | Shared API route definitions, processing statuses, and timeout configurations. |

---

## Policies & Governance

| Document | Description |
|---|---|
| [`SECURITY.md`](file:///E:/Code/carbonservices/carbon-engine-client/SECURITY.md) | Client-side BYOK credential isolation, transport encryption, and vulnerability disclosure SLAs. |
| [`CONTRIBUTING.md`](file:///E:/Code/carbonservices/carbon-engine-client/CONTRIBUTING.md) | C++20 and Qt 6 coding standards, non-blocking asynchronous guidelines, and PR workflow. |
| [`CODE_OF_CONDUCT.md`](file:///E:/Code/carbonservices/carbon-engine-client/CODE_OF_CONDUCT.md) | Contributor Covenant v2.1 community standards. |
| [`LICENSE`](file:///E:/Code/carbonservices/carbon-engine-client/LICENSE) | GNU General Public License v3.0. |
