# Mr. J Distributed System

![Language](https://img.shields.io/badge/language-C-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Release](https://img.shields.io/github/v/release/AlbertoMarquillas/mrj-distributed-system)
![Conventional Commits](https://img.shields.io/badge/Conventional%20Commits-1.0.0-orange)

---

## 📌 Overview

The **Mr. J Distributed System** is a fault‑tolerant, concurrent, and distributed application implemented in **C**. It follows a client–server architecture composed of multiple processes that communicate via **sockets** and synchronize through operating system primitives.

The system simulates a service where clients can connect to a central server, request distortion of media or text files, and receive processed results from specialized worker processes. Robustness is ensured through automatic recovery mechanisms that reassign tasks to other workers in case of process failure.

---

## 📂 Repository Structure

```
<repo>/
├─ src/                     # Source code in C
│  ├─ Fleck/                # Client process (shell interface)
│  ├─ Gotham/               # Central server process
│  ├─ Harley/               # Media distortion worker(s)
│  ├─ Enigma/               # Text distortion worker(s)
│  ├─ Arkham/               # Logging process (spawned by Gotham)
├─ test/                    # Placeholder for future tests
├─ docs/                    # Documentation
│  ├─ statement.pdf         # Assignment statement (Catalan)
│  └─ assets/               # Figures, diagrams, logs
├─ build/                   # Compilation outputs (ignored)
└─ README.md                # Project documentation
```

---

## ⚙️ Build Instructions

Ensure you have **GCC** (or another C compiler) installed. The project provides a `Makefile` that builds all modules.

### Build (PowerShell)

```powershell
cd src
make
```

This generates executables for:

* `Fleck` (client)
* `Gotham` (central server)
* `Harley` (media worker)
* `Enigma` (text worker)
* `Arkham` (logger, spawned by Gotham)

### Clean build

```powershell
make clean
```

---

## 🚀 Run Instructions

Each process reads a **configuration file** that specifies IP addresses, ports, and working directories. Example usage from PowerShell:

```powershell
# Start central server (Gotham)
./Gotham config_gotham.dat

# Start media worker (Harley)
./Harley config_harley.dat

# Start text worker (Enigma)
./Enigma config_enigma.dat

# Start client shell (Fleck)
./Fleck config_fleck.dat
```

Gotham will spawn the `Arkham` logger automatically during execution.

---

## 🔍 Features

* **Client process (Fleck):** interactive shell with commands (`CONNECT`, `LOGOUT`, `LIST MEDIA`, `LIST TEXT`, `DISTORT`, `CHECK STATUS`, `CLEAR ALL`).
* **Central server (Gotham):** manages all connections, routes client requests, monitors workers.
* **Workers (Harley/Enigma):** handle media and text distortions, support recovery if a worker crashes.
* **Logger (Arkham):** records all system events with timestamps in `logs.txt`.
* **Fault tolerance:** automatic reassignment of tasks if workers fail.
* **Concurrent architecture:** multiple clients and workers can be active simultaneously.

---

## 📚 What I Learned

Through this project I gained experience in:

* Systems programming in **C**.
* **Sockets** and network communication.
* **Concurrency** and process management.
* Error handling, robustness, and fault recovery mechanisms.
* Designing and organizing a distributed system with multiple components.

---

## 📜 License

This project is released under the [MIT License](LICENSE).
