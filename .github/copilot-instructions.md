**Purpose**
- **Scope:** Brief, actionable guidance for AI coding agents working on this repository.
- **Repo snapshot:** The workspace contains `fileCompression` (binary/executable) and `fileCompression.c` (source file currently empty).

**Quick Goals for an Agent**
- **Discover behavior:** Inspect the `fileCompression` executable to learn expected CLI behavior before changing `fileCompression.c`.
- **Rebuild safely:** Reproduce the binary build from `fileCompression.c` and preserve CLI semantics.
- **Document & test:** Add a minimal `README.md` and a small test harness if behavior is inferred or implemented.

**Commands & Examples (macOS / zsh)**n+- **Inspect the executable:** `file fileCompression` and `strings fileCompression | head` to find clues about CLI args and usage.
- **Try runtime help:** `./fileCompression --help` or `./fileCompression -h` (if executable supports it).
- **Build (default):** `clang -O2 -Wall -Wextra -o fileCompression fileCompression.c`
- **Build (debug + sanitizers):** `clang -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -o fileCompression_dbg fileCompression.c`
- **Run under debugger:** `lldb -- ./fileCompression` then `run` inside `lldb`.

**What to look for in code changes**
- **CLI surface:** Keep CLI flags and output format stable when updating the `fileCompression` implementation to avoid breaking downstream users.
- **I/O behavior:** Check whether `fileCompression` reads/writes files or stdin/stdout; preserve behavior or add backward-compatible flags.
- **Performance:** Compression tools often care about buffer sizes and streaming — prefer incremental changes and benchmarks.

**Project-specific patterns (discoverable)**
- **Single-binary repo:** This repo appears to center on one executable. Treat changes as high-impact and keep commits small and reversible.
- **Missing metadata:** There is no `README.md`, `Makefile`, or tests present. When adding files, reference `fileCompression` and `fileCompression.c` explicitly.

**When behavior is unknown**
- **Safe exploration approach:**
  - Step 1: Run `file fileCompression` and `strings fileCompression | sed -n '1,120p'` to extract usage hints.
  - Step 2: Run `./fileCompression` with common flags (`-h`, `--help`, `-v`, `--version`) to see output.
  - Step 3: If still unclear, open an issue or add a short `INVESTIGATE.md` documenting findings before changing code.

**Testing & CI notes**
- **No CI files:** There are no workflow or test manifests to rely on — run local builds and sanitizers locally.
- **Recommended quick test harness:** Create a `tests/` folder with a small shell script that runs the binary on known inputs and checks outputs with `diff`.

**If you edit code**
- **Create a PR:** Include build instructions, observed behavior (how you inferred it), and a small example that exercises the change.
- **Preserve executable name:** Keep the built artifact named `fileCompression` unless you supply a clear migration path.

**Where to look next (key files)**
- `fileCompression` — existing executable to inspect (binary). 
- `fileCompression.c` — source file to edit and rebuild.

Please review and tell me if you want more details (build matrix, example inputs, or a starter `README.md`).