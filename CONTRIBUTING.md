# Contributing to Carbon Engine Client

Thank you for your interest in contributing to **Carbon Engine Client**! We welcome contributions from desktop systems engineers, Qt/QML developers, UI/UX designers, and ESG/sustainability domain specialists.

This repository hosts the standalone C++20 and Qt 6 desktop application that serves as the graphical compliance workbench for **Carbon Engine**.

---

## 💡 Contribution Philosophy: Discuss Before Coding

To ensure your time and effort are well spent, **please open an issue to discuss your proposal before submitting a major Pull Request (PR)**.

- **New Features & UI Enhancements**: Open an issue detailing your use case, mockups or design concept, and implementation strategy. This allows maintainers and the community to align on user experience consistency and architectural patterns.
- **REST API Integration Changes**: If your change requires modifications to backend endpoints or contracts, coordinate with the [Carbon Engine Server](https://github.com/carbonservices/carbon-engine-server) repository first.
- **Small Fixes**: Minor bug fixes, documentation typos, or small CMake/QML styling tweaks do not require prior discussion and can be submitted directly as PRs.

---

## 🐛 Reporting Bugs

When filing a bug report, please include the following details:

### 1. Observed Behavior
- Exact error messages, visual glitches, or stack traces.
- Relevant rotating application log files located in `%USERPROFILE%\Documents\CarbonESG\logs\` (or terminal console output).
- Screenshots or short screen recordings illustrating UI issues.

### 2. Expected Behavior
- Describe what you expected to occur based on statutory SEBI BRSR guidelines and client documentation.

### 3. Environment Profile
- **Operating System**: Windows 10/11 (64-bit), Ubuntu 22.04+, macOS 13+.
- **Display Configuration**: Screen resolution and DPI scaling factor (e.g., 4K at 150%).
- **Software Toolchain**:
  - Compiler: MSVC 2022 (v143+), GCC 13+, or Clang 17+
  - Qt Framework: Qt 6.5+ (specifically `Qt6::Core`, `Qt6::Gui`, `Qt6::Qml`, `Qt6::Quick`, `Qt6::WebEngineQuick`)
  - CMake: Version 3.20+
  - Build Generator: Ninja

### 4. Steps to Reproduce
- Numbered sequence of steps from application launch to the encountered failure.

---

## 🛠️ C++20, Qt & QML Standards

All client code in `src/` follows modern C++20 and Qt 6 best practices. Please adhere to the following standards:

### 1. Resource Management & RAII
- Rely on RAII (Resource Acquisition Is Initialization) for all resource lifetimes.
- Never use raw `new` and `delete`. Use `std::make_unique` or Qt object parent-child hierarchies (`new QObject(parent)` where ownership is clearly managed by Qt's object tree).
- Use `QPointer` or `QScopedPointer` when managing references to dynamically created QML-accessible objects.

### 2. Non-Blocking GUI & Asynchronous Networking
- **Never block the main GUI thread**. Long network operations, file reading, and polling cycles must execute asynchronously.
- Network communication must leverage `QNetworkAccessManager` asynchronously via signals/slots or event-driven poller workers (`ExtractionStatusPoller`, `PipelineStatusPoller`, `SystemStatusPoller`).
- Dynamic server endpoint resolution and health checking must extend `ApiClientBase`.

### 3. QML Design Tokens & Responsive Layouts
- All colors, fonts, margins, and animation timings must adhere to the design token system defined in [`src/qml/Theme.qml`](src/qml/Theme.qml). Avoid hardcoded hex colors or arbitrary pixel margins.
- Support both **Light Mode** and **Dark Mode** seamlessly via the theme token bindings.
- Layouts must be responsive and adapt gracefully to window resizing using `Layout` attached properties (`RowLayout`, `ColumnLayout`, `Layout.fillWidth`).

### 4. Logging & Diagnostic Hygiene
- Use the central thread-safe `Logger` subsystem (`LOG_INFO`, `LOG_WARN`, `LOG_ERROR`, `LOG_DEBUG`) for all operational logging.
- Do not use raw `std::cout` or `printf`.
- **Zero Credential Exposure**: Never log API keys, authentication tokens, or raw confidential document text to logs or terminal streams.

### 5. API Routes & Shared Constants
- Centralize all API routes, status strings, and default timeouts in [`src/common/CommonConstants.h`](src/common/CommonConstants.h) rather than using string literals scattered across client classes.

---

## 🚀 Pull Request Workflow

1. **Fork & Branch**: Create a feature branch with a descriptive name:
   - `git checkout -b feat/add-metric-filter`
   - `git checkout -b fix/upload-base64-padding`
   - `git checkout -b ui/improve-settings-feedback`
2. **Build & Verify Locally**:
   Ensure the client compiles cleanly without warnings:
   ```cmd
   :: Initialize MSVC 64-bit environment (Windows)
   call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

   :: Configure CMake with Ninja
   cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\6.10.2\msvc2022_64"

   :: Build executable
   cmake --build build --config Release
   ```
3. **Commit Guidelines**: Write clear, imperative commit messages:
   - `feat: add live latency indicator to connection badge`
   - `fix: handle base64 padding correctly during remote PDF upload`
   - `docs: update CONTRIBUTING.md with Qt 6 build instructions`
4. **Documentation**: Update [README.md](README.md) whenever new UI capabilities, configuration options, or dependencies are introduced.

