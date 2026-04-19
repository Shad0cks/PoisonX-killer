# PoisonX — Kernel Driver Process Termination POC

> **Disclaimer — For Educational & Research Purposes Only**
> This project is a Proof of Concept developed strictly for cybersecurity research, academic study, and authorized penetration testing environments. It must **never** be used against systems, networks, or processes without explicit written authorization from the owner. The author(s) assume **no responsibility** for any misuse, damage, or illegal activity resulting from the use of this code. Use it only in isolated lab environments or within the scope of a legally authorized engagement.

---

## Background — The Driver

`PoisonX.sys` is a **Microsoft-signed** kernel driver publicly disclosed on **April 10, 2026** by security researcher **Bilal Retiat**.

| Property | Value |
|---|---|
| Driver filename | `PoisonX.sys`|
| Signer | Microsoft Windows Hardware Compatibility Publisher |
| Signing validity | October 10, 2024 – October 8, 2025 |
| File creation timestamp | March 24, 2025 |
| LOLDrivers entry | [fc3467c3-6109-447d-b438-7a4276c3d8e5](https://www.loldrivers.io/drivers/fc3467c3-6109-447d-b438-7a4276c3d8e5/) |
| Blocklist status | **Not yet blocklisted** by Windows Defender or major EDR vendors as of April 2026 |
| Kernel imports | `ZwTerminateProcess`, `IoGetCurrentProcess`, `KeStackAttachProcess` |

The driver exposes an IOCTL interface that allows any user-mode process with a valid handle to **terminate arbitrary processes at kernel level**, bypassing the PPL (Protected Process Light) model that normally shields EDR agents from being killed from user space.

---

## Overview

**PoisonX-killer** is a user-mode client that communicates with the `PoisonX.sys` driver through a Windows IOCTL interface to terminate an arbitrary process by its PID. This POC demonstrates the attack surface that a Microsoft-signed BYOVD driver exposes, and is intended to help defenders understand, detect, and mitigate such techniques before the driver is formally blocklisted.

The binary enumerates a predefined list of well-known EDR, AV, DLP, and remote access processes, resolves their PIDs at runtime, and sends each PID to the driver via `DeviceIoControl`. This is a technique studied in red team research and known in the threat intelligence community as **Bring Your Own Vulnerable Driver (BYOVD)**.

---

## Architecture

```
[ PoisonX-killer.exe (user mode) ]
        |
        | CreateFile  →  \\.\{F8284233-48F4-4680-ADDD-F8284233}
        |
        | DeviceIoControl (IOCTL: 0x22E010)
        |    input:  PID (string)
        |    output: "ok" on success
        v
[ PoisonX.sys (kernel mode driver) ]
        |
        v
   ZwTerminateProcess / kernel-level kill
```

---

## Components

| File | Description |
|---|---|
| `main.cpp` | User-mode client — process enumeration, IOCTL dispatch |
| `Makefile` | Build rules for MSVC (standard) and OLLVM (obfuscated) |
| `build.bat` | Helper script to initialize MSVC env and invoke nmake |

> The kernel driver (`PoisonX.sys`) itself is **not included** in this repository.

---

## Target Process Categories

The hardcoded process list covers the following categories :

- **Antivirus / EPP** — Avast, Avira, Kaspersky, Bitdefender, Symantec, McAfee, ESET, Webroot, Panda, Sophos
- **EDR / XDR** — CrowdStrike Falcon, SentinelOne, Cortex XDR (Palo Alto), FireEye/Trellix HX, Elastic Defend, Rapid7, Tanium, Trend Micro Apex One
- **Microsoft Defender** — `msmpeng.exe`, `mssense.exe`, `mpdefendercoreservice.exe`, `windefend.exe`
- **DLP** — Symantec DLP, McAfee/Trellix DLP, Microsoft Defender DLP
- **SIEM / Log forwarders** — Sysmon (x86/x64), Winlogbeat, nxlog
- **VPN / Network security** — GlobalProtect, FortiClient, Cisco Secure Client, OpenVPN
- **Remote access** — TeamViewer, Microsoft RDP/mstsc, Quick Assist
- **MDM / Endpoint management** — Microsoft Intune, SCCM, ManageEngine Endpoint Central
- **Cloud sync / backup** — OneDrive, Dropbox, Box, Google Drive, iCloud
- **Password managers** — 1Password, Bitwarden, KeePass, KeePassXC
- **Virtualization** — VMware Tools, WSL

---

## Build

### Prerequisites

- Windows 10/11 (x64)
- Visual Studio 2022 (Community or higher) with the **Desktop development with C++** workload
- *(Optional)* OLLVM 16.0.6 for obfuscated builds

### Standard build (MSVC)

```bat
build.bat
```

The output binary is placed at `out\PoisonX-killer.exe`.

### Obfuscated build (OLLVM)

Edit `Makefile` to point `OLLVM_BIN` to your OLLVM installation, then run:

```bat
nmake obfuscated
```

OLLVM passes applied: instruction substitution (`-sub`), basic block splitting (`-split`), control-flow flattening (`-fla`), bogus control flow (`-bcf`).

### Clean

```bat
nmake clean
```

---

## How It Works

1. The binary opens a handle to the driver device: `\\.\{F8284233-48F4-4680-ADDD-F8284233}`
2. It loops over the hardcoded process list and calls `CreateToolhelp32Snapshot` to resolve each name to a live PID.
3. For each found PID, it sends the PID as a null-terminated ASCII string to the driver via `DeviceIoControl` with IOCTL code `0x22E010`.
4. The driver responds with `"ok"` on successful termination.
5. The loop runs continuously until the handle is closed.

---

## Detection & Defensive Notes

This POC was built to support blue team research. Relevant detection opportunities include:

- **Driver load events** — monitor for unsigned or vulnerable drivers loading via `Sysmon Event ID 6` or ETW `Microsoft-Windows-Kernel-PnP`.
- **IOCTL patterns** — `DeviceIoControl` calls to non-standard device names are detectable via kernel callbacks or ETW.
- **Process termination from kernel** — `Sysmon Event ID 5` with a kernel-originated termination context.
- **BYOVD mitigations** — Microsoft HVCI (Hypervisor-Protected Code Integrity) and the Windows Driver Block List prevent loading known-vulnerable drivers. Since `PoisonX.sys` is not yet blocklisted (as of April 2026), HVCI is currently the primary mitigation.
- **YARA / Sigma / Sysmon rules** — Detection rules for `PoisonX.sys` are available on the [LOLDrivers entry](https://www.loldrivers.io/drivers/fc3467c3-6109-447d-b438-7a4276c3d8e5/).

---

## Legal Notice

This software is provided **"as is"**, for **authorized security research and educational use only**. Deploying it outside of a controlled lab or a legally scoped penetration testing engagement is illegal and unethical. The author(s) bear **no liability** for any direct or indirect damage caused by misuse of this code.
