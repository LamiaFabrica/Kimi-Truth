// kimi_truth.hpp — seal verification that cannot be fooled by line endings.
//
// Estate tool (Lamia Fabrica). Consumes LFSSL BLAKE3 (the estate's only
// sanctioned hash; official-vectors KAT enforced at boot) — never mirrors it.
//
// The failure class this fixes (BettyBot, 12/09/2026): four ratified work
// orders "failed" their BLAKE3 seals — two were CRLF drift only (content
// byte-identical after LF normalisation), two had genuinely drifted. A bare
// byte-hash cannot tell tamper from line-ending noise; kimi_truth can.
//
// Canonical rule: seals are computed over LF-normalised content, so a file
// keeps its seal across Windows (CRLF) and Linux (LF) checkouts.
//
// Lanes (one spelling per lane, never crossed) — canonical home is
// AvalonOS/inc/Repos/Kimi-Truth; pathing corrected 21/09/2026:
//   Windows  C:\McMaker Projects\Projects\AvalonOS\inc\Repos\Kimi-Truth
//   Git Bash /c/McMaker Projects/Projects/AvalonOS/inc/Repos/Kimi-Truth
//   WSL      /mnt/c/McMaker Projects/Projects/AvalonOS/inc/Repos/Kimi-Truth
//
// Locale: en-GB. Clock: Zulu. Fail closed, always.

#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <vector>

namespace kimi::truth {

// Official BLAKE3 KAT result for the empty message, as hex.
// Boot gate: if the linked LFSSL cannot reproduce this, no verdict the tool
// gives may be trusted — every entry point fails closed.
inline constexpr const char* kKatEmptyHex =
    "af1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262";

enum class Verdict {
    match_raw,  // bytes identical to the sealed hash
    match_lf,   // line-ending drift only — content intact
    drift       // content changed since the seal — unratified
};

struct VerifyResult {
    Verdict verdict;
    std::string sealed_hash;
    std::string raw_hash;
    std::string lf_hash;
};

// The KAT boot gate. True only if LFSSL BLAKE3("") == the official vector.
[[nodiscard]] bool selftest();

[[nodiscard]] std::expected<std::vector<uint8_t>, std::string>
read_file(const std::string& path);

// LF-canonical form: every CR removed. The canonical sealing basis.
[[nodiscard]] std::vector<uint8_t> lf_canonical(const std::vector<uint8_t>& raw);

[[nodiscard]] std::string blake3_hex(const std::vector<uint8_t>& data);

// Latest 64-hex token in an append-only seal log (newest entry wins).
[[nodiscard]] std::expected<std::string, std::string>
last_sealed_hash(const std::string& seal_path);

[[nodiscard]] std::expected<VerifyResult, std::string>
verify(const std::string& file, const std::string& seal_path);

// Append "<Zulu ISO>\t<LF hash>\tkimi_truth v1 (LF-canonical)" to <file>.seal.
[[nodiscard]] std::expected<std::string, std::string>
seal(const std::string& file);

[[nodiscard]] std::string zulu_now();

} // namespace kimi::truth
