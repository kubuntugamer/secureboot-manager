# KDE Secure Boot Manager (`secureboot-manager`)

[![License: GPL v3](https://shields.io)](https://gnu.org)

A polished, native KDE Plasma systems utility designed to simplify machine owner keys (MOK) management and custom kernel/module signing under UEFI Secure Boot. Built natively with C++ and Qt 6, it completely eliminates the need for manual command-line execution, providing a secure, end-to-end graphical workflow that blends seamlessly into the modern KDE desktop.

---

## 🎨 Key Features

*   **Native KDE Aesthetic:** Implements a modern sidebar navigation paradigm matching Kubuntu's native look and feel.
*   **End-to-End Key Generation:** Automates OpenSSL X.509 certificate pair generation (`.priv` / `.pem`) for bootstrapping fresh installations or custom kernels (e.g., Liquorix).
*   **Automated Unified Staging:** Transparently interfaces with `mokutil` to stage keys directly into NVRAM, securely handling validation passwords for the next physical hardware reboot.
*   **Decoupled Submodule Architecture:** Follows strict Unix philosophy and Object-Oriented Design patterns. Logic parsing, data injection, and system execution routines are fully modularized and rigorously documented.
*   **Secure Privilege Elevation:** Launches safely under standard user permissions for monitoring tasks and seamlessly prompts for administrative validation via Polkit **only** when committing system changes.

---

## 🛠️ Compiling from Source (Terminal-Only Pipeline)

This project uses **CMake** natively. You do not need to install Qt Creator or any IDE to compile this project. It can be built entirely from the command line using standard system developer utilities.

### 1. Install Build Dependencies

Ensure your Kubuntu system has the necessary compiler packages, core Qt 6 library development files, and security policy packages installed:

```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-tools-dev qt6-l10n-tools libpolkit-qt6-1-dev
```

### 2. Clone the Repository

Clone the project files down to your local directory sandbox:

```bash
git clone https://github.com/kubuntugamer/secureboot-manager.git
cd secureboot-manager
```

### 3. Build and Compile

Generate the native configuration build scripts and execute the compiler toolchain entirely via terminal commands:

```bash
# 1. Create and enter a dedicated build directory
mkdir build && cd build

# 2. Run CMake to automatically generate standard Makefiles
cmake ..

# 3. Invoke the compiler to build the native binary executable
cmake --build .
```

Once the build sequence finishes, the fully optimized native binary file will materialize directly inside your `build/` folder, ready to be executed or compressed into your target `.deb` package payloads.

---

## ⚖️ License

This project is officially licensed under the **GNU General Public License v3 (GPLv3)**. It is fully compatible with upstream KDE developmental library requirements, guaranteeing that the source code remains open, protected against predatory patent claims, and properly attributed.
