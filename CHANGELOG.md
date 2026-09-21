# Changelog — kimi_truth (Kimi-Truth)

Append-only, Zulu-dated (PRES-01). One seam per change. en-GB.

## 2026-09-21 — pathing correction (no logic change)

**Context.** The tool's documented lane paths and CMake LFSSL discovery pointed
at locations that no longer exist (old home `Projects/tools/tools/Kimi_truth`;
libs under `build_shared/lib`, `build_fortress_win/lib`, `Forum 2026/build_linux`).
Canonical home is now `AvalonOS/inc/Repos/Kimi-Truth`; LFSSL is its sibling
`AvalonOS/inc/Repos/LFSSL`.

**Changed (pathing only, no behaviour):**
- `CMakeLists.txt` — LFSSL discovery repointed to the sibling repo and the real
  lane build dirs: `build_win_gcc162/lib/liblfssl.dll.a` (Windows),
  `build_wsl_gcc162/lib/liblfssl.so.1.0.0` (Linux, with `-Wl,-rpath`). Fails
  closed (`FATAL_ERROR`) when the library is absent.
- `kimi_truth.hpp` — lane comment corrected to the canonical home.
- `KIMI_TRUTH.env` — `HOME_*` / `BIN_*` and both build recipes corrected to the
  canonical home and the real LFSSL paths.

**Unchanged (proven behaviour):** `kimi_truth.cpp`, the KAT boot gate, verdicts
and exit codes.

**Evidence (dual-lane green, MEMO L-021):**
- Windows — WinLibs g++ 16.2.0, Ninja: configure + build exit 0;
  `kimi_truth.exe selftest` → `TRUTH: SELFTEST PASS (BLAKE3 official empty
  vector, LFSSL)`. Runtime needs `LFSSL/build_win_gcc162/bin` on `PATH`
  (liblfssl.dll).
- WSL — Ubuntu-26.04 g++-16, Ninja: configure + build exit 0;
  `./kimi_truth selftest` → `TRUTH: SELFTEST PASS`.
- LFSSL libs used: `build_win_gcc162/lib/liblfssl.dll.a` (+ `bin/liblfssl.dll`),
  `build_wsl_gcc162/lib/liblfssl.so.1.0.0`.
