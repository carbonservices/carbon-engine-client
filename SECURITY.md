# Security Policy

## Overview

**Carbon Engine Client** is a high-performance desktop workbench built with modern C++20, Qt 6 Quick / QML, and Qt WebEngine for corporate ESG disclosures and statutory SEBI BRSR compliance analytics. In corporate compliance and sustainability workflows, annual reports and draft disclosures frequently contain Unpublished Price Sensitive Information (UPSI).

We take security considerations seriously, including client-side credential protection, transport encryption, embedded browser sandboxing, native buffer safety, and local diagnostic log hygiene.

---

## Supported Versions

Security updates and patches are actively maintained for the following versions:

| Version | Supported |
|---|---|
| 1.x (Current master) | :white_check_mark: |
| < 1.0 | :x: |

---

## Reporting a Security Vulnerability

If you discover a security vulnerability in Carbon Engine Client, please **do not open a public GitHub issue**. Publicly disclosing an issue puts production compliance workflows and sensitive corporate disclosures at risk.

### Private Reporting Channels

Please submit vulnerability reports through one of the following methods:
1. **GitHub Private Vulnerability Reporting**: Navigate to the **Security** tab of this repository and select **"Report a vulnerability"**.
2. **Email**: Send detailed security advisories directly to `carbon.esgreports@gmail.com`.

### What to Include in Your Report

To help our security team triage and resolve issues efficiently, please include:
- A clear description of the vulnerability and its potential impact.
- Step-by-step reproduction instructions or a minimal Proof of Concept (PoC).
- Environment details: Operating system (Windows, macOS, Linux), Qt version (e.g., Qt 6.10.2), compiler version (MSVC 2022, GCC 13+, Clang 17+), and display scaling/DPI configuration.
- Sanitized log output from `%USERPROFILE%\Documents\CarbonESG\logs\` (ensure all personal file paths and company names are redacted).
- If you have an advisory remediation or patch suggestion, feel free to include it.

### Response Timeline

- **Initial Acknowledgement**: Within 48 hours of receipt.
- **Triage & Reproducibility Assessment**: Within 5 business days.
- **Remediation & Coordinated Disclosure**: We will collaborate with you to verify the fix and coordinate a release timeline before public disclosure.

---

## Core Security Commitments & Architecture

### 1. Bring Your Own Key (BYOK) & Credential Protection
- **Client-Side Storage**: The user provides their own Gemini API key via the GUI settings dialog. Keys are stored locally on the client machine via system settings (`QSettings`) and are never dispatched to telemetry services, third-party analytics, or unauthorized hosts.
- **Header Isolation**: API keys are transmitted exclusively over secure HTTPS channels directly to the user-configured Carbon Engine Server endpoint via the `X-Gemini-API-Key` HTTP header.
- **Log Masking**: API keys are never echoed to stdout/stderr or written to persistent disk log files. Any diagnostic logging automatically masks secret tokens.

### 2. Transport Security & Endpoint Integrity
- **Encrypted Remote Transport**: Connecting to remote serverless deployments (Google Cloud Run) strictly enforces HTTPS/TLS encryption.
- **Endpoint Validation**: Dynamic backend URLs are sanitized and validated through `ApiClientBase` before any network request or polling cycle is initiated.

### 3. Embedded WebEngine & Chromium Sandbox Security
- **Process Isolation**: The embedded HTML audit dossier viewer is powered by `QtWebEngineQuick` (Chromium engine) and runs in an isolated subprocess with sandboxing enabled.
- **Self-Contained Rendering**: Generated statutory audit dossiers are rendered as self-contained HTML reports with inline base64 and SVG assets, minimizing external resource fetching and eliminating cross-origin script injection vectors.

### 4. Memory Safety & Asynchronous Buffer Streaming
- **Asynchronous Network Buffers**: Document uploads stream base64-encoded PDF payloads asynchronously via `QNetworkAccessManager` without blocking the main GUI thread or loading unbounded buffers into unmanaged memory.
- **Rotated Diagnostics**: Application logs are managed by a thread-safe rotating file logger (`Logger`) that enforces size caps (5 MB per file, 3 backups) to protect against disk exhaustion attacks.

