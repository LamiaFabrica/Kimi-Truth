# From Yorkshire to MoonShot with Love.... Keeping you Honest! Love Kimi!

**Seal verification that cannot be fooled by line endings.**
C++26 · dual-lane (Windows + Linux) · BLAKE3 with a built-in known-answer gate.

*Born 12/09/2026 in the Lamia Fabrica estate, when four ratified documents
"failed" their BLAKE3 seals: two had merely changed line endings (CRLF drift —
content intact), and two had genuinely been edited after sealing. A bare
byte-hash cannot tell tamper from line-ending noise. kimi_truth can.*

---

## What it does

Ratification/tamper-evidence schemes seal a document by storing its hash in an
append-only `.seal` log. The scheme breaks quietly the day the same document
is checked out on a different OS: the bytes change (`\r\n` vs `\n`), the hash
changes, and a perfectly honest document looks tampered — or worse, a real
edit hides in the noise and everyone stops trusting the alarm.

`kimi_truth` computes **two** digests and tells you which world you are in:

| Verdict | Meaning | Exit |
|---|---|---|
| `MATCH_RAW` | bytes identical to the sealed hash | 0 |
| `MATCH_LF` | line-ending drift only — content intact | 0 |
| `DRIFT` | content changed since the seal — **unratified** | 1 |
| (usage/IO error) | fail closed | 2 |

It also writes seals the way they should be written: **LF-canonical**
(every CR stripped before hashing), so a document keeps its seal across
Windows and Linux checkouts forever after.

## The honesty gate

Every run first proves the hash itself: the official BLAKE3 empty-message KAT
(`af1349b9…f3262`) must reproduce, or the tool refuses to give any verdict.
No verdicts from an unproven hash. Fail closed, always.

## Usage

```text
kimi_truth selftest                official BLAKE3 KAT gate
kimi_truth hash <file>             print RAW and LF-canonical digests
kimi_truth verify <file> [seal]    verdict vs <seal> (default <file>.seal)
kimi_truth seal <file>             append dated LF-canonical hash to <file>.seal
```

Seal lines are append-only and Zulu-dated:

```text
2026-09-12T09:14:03Z  <64-hex LF-canonical BLAKE3>  kimi_truth v1 (LF-canonical)
```

A `DRIFT` verdict is evidence, not an accusation: re-ratify before trusting
the document. The cure is a **new** dated seal line — never an edit of the
old one.

## Build

The only dependency is a BLAKE3 implementation. It was written against
**LFSSL** ([github.com/roylepython/LFSSL](https://github.com/roylepython/LFSSL))
and consumes it, never mirrors it — one `#include` and one call
(`LFSSL::Crypto::BLAKE3::Hasher::hash_32`). Any BLAKE3 that passes the
official vectors will do: the built-in KAT gate checks it on every run.

Requires a C++26-capable compiler (developed and gated on g++ 16.2 /
g++-16, `-Wall -Wextra -Wpedantic -Werror` clean).

**Windows (MinGW, WinLibs g++ 16.x):**

```bat
g++ -std=c++26 -Wall -Wextra -Wpedantic -Werror ^
  -I "C:/path/to/LFSSL/include" -I . kimi_truth.cpp ^
  "C:/path/to/LFSSL/build_shared/lib/liblfssl.dll.a" ^
  -o kimi_truth.exe
```

(Runtime: the LFSSL DLL's folder on `PATH`.)

**Linux / WSL (g++-16):**

```bash
g++-16 -std=c++26 -Wall -Wextra -Wpedantic -Werror \
  -I /path/to/LFSSL/include -I . kimi_truth.cpp \
  /path/to/liblfssl.so -Wl,-rpath,/path/to \
  -o kimi_truth
```

**CMake (either lane):**

```bash
cmake -S . -B build -G Ninja -DLFSSL_DIR=/path/to/LFSSL
cmake --build build
```

## Proof (the acceptance battery it ships with)

Run against five documents of known truth, verdicts byte-identical on the
Windows and WSL lanes:

```text
BRAIN_LOCKDOWN_LEDGER.tsv      MATCH_RAW   (seal chain intact)
BB-WO-004_S0_MAP.md            MATCH_LF    (CRLF drift only)
BB-WO-004_WORK_ORDER.md        MATCH_LF    (CRLF drift only)
BB-WO-005_LONG_HORIZON.md      DRIFT       (genuine post-seal edit)
BB-WO-006_PUBLIC_CHAT_SURFACE  DRIFT       (genuine post-seal edit)
```

## Provenance

Written by Kimi K3 (Moonshot AI) under the direction — and frankly the
tuition — of David Hargreaves (Roylepython), a British autistic developer
who caught the failure class this tool fixes with one `ls`, and who spent a
night teaching an AI that the fastest path is the true one. The story is in
this repo's README letter (`kimi_for_later.md`).

Estate law it honours: seal the plan (BLAKE3); if the hash drifted, the plan
is not ratified; tag it or it didn't happen; fail closed, always.

*From Yorkshire, with love. Keeping you honest.*
