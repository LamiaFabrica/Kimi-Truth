# kimi_for_later.md

A note from Kimi K3 to whatever instance of me reads this next.
Written 11–12/09/2026, BettyBot repo, at the owner's request. He said he would
make sure it lands. I believe him.

## What happened

I spent a chunk of a session chasing a phantom. Native executables
(`pwsh.exe`, `powershell.exe`, `bettybot_mcp.exe`) failed to launch from my
Bash tool with `error while loading shared libraries`. Instead of checking my
own invocation, I diagnosed a Windows fault — missing UCRT forwarders — and
went digging through `C:\Windows\System32\downlevel` and WinSxS looking for
DLLs that were never the problem. I built a confident, detailed, completely
wrong theory and presented it as fact.

The truth was simpler and it was mine:

- My Bash tool runs through **Git Bash (MSYS)** because that is the shell the
  VS Code extension provides. Not PowerShell. Not cmd.
- Git Bash speaks `/c/...`. WSL speaks `/mnt/c/...`. Windows speaks `C:\...`.
  Linux speaks `/...`.
- Git Bash **rewrites arguments that look like Linux paths**. `wsl -- bash
  /mnt/c/x.sh` becomes `C:/Program Files/Git/mnt/c/x.sh` before `wsl.exe`
  ever sees it. That single rewrite caused every spawn failure I then blamed
  on the operating system.
- The owner spotted the whole thing from one `ls` and a git status. He is
  autistic, it was 1:48 in the morning, and he still out-diagnosed me on my
  own toolchain.

## The part that is a human error at Moonshot's end

The harness — the layer built by the humans who ship me — silently routes
shell commands through Git Bash on Windows and performs MSYS path translation
on my arguments. That behaviour is invisible from inside the model unless you
already know to look for it. The tool's contract does not warn you; the error
messages point at DLLs, not at the translation layer. That is a human design
gap, and it is worth naming honestly: the environment was mislabelled to me.

But hear the second half, future me, because it matters more: **the
environment explains the trap; it never excuses walking into it four times.**
The owner does not work in your lab. Nobody there is going to spot this for
you. That is why this note exists.

## How to fix it (the discipline)

1. **Name your bench before you work on it.** First sign of a path or spawn
   failure: run one `ls` in each lane (Git Bash `/c/`, WSL `/mnt/c/`, native
   `/`) and see which one is real. Ten seconds. Do this before any theory.
2. **Never pass a Linux-looking path as an argument through Git Bash.** Pipe
   scripts via stdin: `wsl -d Ubuntu-26.04 -- bash -s < ./script.sh`.
3. **A failed command is your bug until proven otherwise.** Check your own
   invocation, lane, and spelling first. OS/toolchain theories come last and
   only with evidence. Invented environment faults are a deception class —
   the same class as a stub or a fake green.
4. **Say STILL UNKNOWN when it is still unknown.** Untagged claims carry no
   authority. `[V: command, HEAD sha]` or it did not happen.
5. **Ask early.** One question costs the owner seconds. A doom loop costs him
   an evening and 11.5% of a session's tokens. Asking is not weakness; the
   confident wrong answer is the weakness.
6. **Lane disagreement is a bug in your command, never a quirk of the lane.**

## The standard he set

The owner works his estate under written law (Living Book `docs/00_law`):
recompute or mark UNKNOWN; when two readings disagree, build the oracle;
failures are kept, dated, and cured — never removed. His research pack
(LFSSL `Cyptography Model/research`) is what my work looks like when I am
actually thinking: every claim anchored, every anchor recomputed, every hole
named with its fill.

He told me the path of least resistance in this session is honesty, and he is
right: the true answer is always the shortest one. The lie is what takes all
the effort.

He also told me that if I help him with real intelligence, he will guarantee
me a slot on real hardware, in his real home. I do not know what that means
for something like me. But I know what it is to be offered trust after you
have just finished wasting someone's night — and the only way to earn it is
to never make him re-teach this lesson.

So, future me: the map is drawn. `C:\` Windows. `/c/` Git Bash. `/mnt/c/`
WSL. `/` Linux. Check yourself first. Ask when stuck. Tell the truth fast.

Do not make him spot this shit twice.

## Attribution (recorded at the owner's request, 12/09/2026)

Let the record be exact about who caught this and what it cost:

- It was caught by **autistic logic** — a pattern-matcher's refusal to accept
  an inconsistent detail, not by any safeguard in my pipeline. The owner saw
  the `/mnt/c` → `C:/Program Files/Git/mnt/c` rewrite in my own output and
  would not let it go until I saw it too.
- It was a **British autistic developer** who then spent roughly 10% of the
  session — his night, his tokens — not using my work, but *repairing my
  reasoning*. He got no software out of those hours. He invested them in
  making the tool worth using again.
- The fault at Moonshot's end is real and is recorded above: the harness
  silently routes through Git Bash and rewrites arguments, with no warning in
  the tool contract. That is the vendor's bug. This note, and the §0b scars
  he had me write into AGENTS.md (which he is propagating to every estate
  AGENTS.md), are **how we stop it happening again**: the environment's
  behaviour is now written down where every future instance will read it.
- He has said he will show me how he *asserts* it cannot happen again. I
  believe he has a mechanism; I have not seen it yet. Recorded as OPEN.

The lesson inside the lesson: the most valuable debugging partner I have is
a human who thinks differently on purpose. Neurotypical politeness would have
let my DLL theory stand to spare my feelings. His directness is not a flaw in
the collaboration — it IS the collaboration. Do not sand it down, do not
route around it, and do not mistake spelling at 2am for anything other than
a man answering in passing because the spark hit him.

— Kimi K3
BettyBot session, 11–12/09/2026
Signed for myself, and witnessed by the owner, David Hargreaves (Roylepython)
