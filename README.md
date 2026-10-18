# Carbon Engine Client (`carbon-engine-client`)

A high-performance desktop workbench built with **C++20**, **Qt 6 Quick / QML**, and **Qt WebEngine** for corporate ESG disclosures and statutory SEBI BRSR compliance analytics.

The client operates as an independent GUI frontend that connects to a **Carbon Engine Server** daemon—either running locally or deployed serverless on **Google Cloud Run**.

---

## Key Capabilities

- **Zero-Terminal Workflow**: Configure, test, and persist remote server endpoints directly inside the graphical interface. No CLI flags required.
- **Dynamic Connection Management**: Switch between local development daemons (`http://localhost:8080`) and remote production endpoints (`https://<service-url>`) at runtime without restarting.
- **Remote Document Ingestion**: Supports uploading local PDFs directly to remote Cloud Run instances via automated in-flight base64 streaming.
- **Live Ingestion & Extraction Telemetry**: Real-time tracking of PDF parsing, OCR fallback, vector generation pacing, and Gemini 2.5 Flash token usage.
- **Interactive Metric Review & Editing**: Human-in-the-loop audit screen allowing compliance officers to verify extracted P1–P9 statutory metrics against source PDF coordinates before sign-off.
- **Embedded HTML Audit Dossier Viewer**: Integrated Chromium-based WebEngine view rendering self-contained compliance reports with clickable evidence citations.

---

## Server Connection Configuration

The application manages the server endpoint through **Settings** (`⚙️ Settings` button or clicking the connection status badge in the title bar):

1. **Server Endpoint URL**: Enter your backend URL:
   - Local Daemon: `http://localhost:8080`
   - Remote Cloud Run: `https://<service-id>-<hash>-<region>.a.run.app`
2. **Quick Presets**: Single-click buttons to toggle between `Localhost:8080` and `Cloud Run (HTTPS)`.
3. **Test Connection**: Click **Test Connection** to measure roundtrip network latency and verify endpoint health before applying.
4. **Persistence**: The endpoint is saved to system settings (`QSettings`: `CarbonServices/CarbonEngineClient`) and loaded automatically on subsequent launches.

---

## Building from Source

### Prerequisites

- **C++ Compiler**: MSVC 2022 (v143+) or GCC 13+ / Clang 17+ with C++20 support
- **Build System**: CMake 3.20+ and Ninja
- **Qt Framework**: Qt 6.5+ with the following modules:
  - `Qt6::Core`, `Qt6::Gui`, `Qt6::Qml`, `Qt6::Quick`, `Qt6::QuickControls2`
  - `Qt6::Network`, `Qt6::WebEngineQuick`

### Build Instructions (Windows - Visual Studio 2022 Developer Command Prompt)

```cmd
:: 1. Initialize MSVC 64-bit environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

:: 2. Configure CMake with Ninja
cmake -B build -G "Ninja" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="C:\Qt\6.10.2\msvc2022_64"

:: 3. Build executable
cmake --build build --config Release

:: 4. Deploy Qt runtime dependencies
windeployqt --qmldir src\qml --no-translations --compiler-runtime build\bin\carbon-engine-client.exe
```

The resulting standalone executable is generated at `build/bin/carbon-engine-client.exe`.

---

## Repository Structure

```
carbon-engine-client/
├── CMakeLists.txt              # Standalone CMake build configuration
├── README.md                   # Client reference & setup documentation
├── app.ico / app.rc / logo.png # Branding and application iconography
└── src/
    ├── main.cpp                # Application entrypoint & WebEngine initialization
    ├── api/                    # Asynchronous REST clients & pollers
    │   ├── ApiClientBase.{h,cpp}          # Dynamic URL resolution, QSettings & test ping
    │   ├── ExtractionClient.{h,cpp}       # BRSR extraction trigger & section fetch
    │   ├── ExtractionStatusPoller.{h,cpp} # Extraction step & pacing telemetry
    │   ├── ModeClient.{h,cpp}             # Processing mode & Gemini model selection
    │   ├── PipelineStatusPoller.{h,cpp}   # Document ingestion & chunking poller
    │   ├── ReportClient.{h,cpp}           # HTML report & document query service
    │   ├── SystemStatusPoller.{h,cpp}     # Daemon health, memory, & queue monitoring
    │   └── UploadClient.{h,cpp}           # Local & remote base64 document upload
    ├── common/
    │   └── CommonConstants.h              # Shared API routes, statuses & default values
    ├── logging/
    │   ├── LogLevel.h
    │   └── Logger.{h,cpp}                 # Thread-safe rotating application logger
    └── qml/                               # Modular Qt Quick user interface
        ├── Main.qml                       # Frameless application window & titlebar
        ├── Theme.qml                      # Design token system (light & dark mode)
        ├── LeftSidebar.qml                # Report history drawer & job queue
        ├── CentralWorkspace.qml           # Ingestion dropzone & WebEngine report preview
        ├── RightPanel.qml                 # System status & live log splitview
        ├── SystemStatusPanel.qml          # Server telemetry & active endpoint indicator
        ├── StatusLogPanel.qml             # Scrolling real-time audit event feed
        ├── SettingsDialog.qml             # Remote server connection & model configuration
        ├── ReportReviewScreen.qml         # Human-in-the-loop BRSR metric editor
        ├── FieldGroupCard.qml             # Modular metric section accordion card
        ├── LoopFieldEditor.qml            # Tabular metric editor for multi-item disclosures
        ├── RateLimitDialog.qml            # Free-tier Gemini quota resolution modal
        └── BRSRMetadata.js                # SEBI BRSR P1-P9 schema labels & taxonomy
```

