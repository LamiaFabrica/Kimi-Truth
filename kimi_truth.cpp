// kimi_truth.cpp — CLI + implementation for kimi_truth (see the header for the
// contract and the failure class this fixes).
//
// Usage:
//   kimi_truth selftest                official BLAKE3 KAT gate (fail closed)
//   kimi_truth hash <file>             print RAW and LF-canonical digests
//   kimi_truth verify <file> [seal]    verdict vs <seal> (default <file>.seal)
//   kimi_truth seal <file>             append dated LF-canonical hash to <file>.seal
//
// Exit codes: 0 match/ok · 1 DRIFT or KAT failure · 2 usage or IO error.

#include "kimi_truth.hpp"

#include <lfssl/crypto/blake3/blake3.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>

namespace BLAKE3 = LFSSL::Crypto::BLAKE3;

namespace kimi::truth {
namespace {

std::string hex_of(const std::array<uint8_t, 32>& h) {
    static constexpr char kD[] = "0123456789abcdef";
    std::string out;
    out.reserve(64);
    for (uint8_t b : h) { out.push_back(kD[b >> 4]); out.push_back(kD[b & 0xF]); }
    return out;
}

bool is_hex64(const std::string& tok) {
    return tok.size() == 64 &&
           std::all_of(tok.begin(), tok.end(), [](char c) {
               return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
           });
}

} // namespace

bool selftest() {
    return hex_of(BLAKE3::Hasher::hash_32(nullptr, 0)) == kKatEmptyHex;
}

std::expected<std::vector<uint8_t>, std::string> read_file(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return std::unexpected("cannot read " + path);
    std::vector<uint8_t> out((std::istreambuf_iterator<char>(in)),
                             std::istreambuf_iterator<char>());
    return out;
}

std::vector<uint8_t> lf_canonical(const std::vector<uint8_t>& raw) {
    std::vector<uint8_t> out;
    out.reserve(raw.size());
    std::copy_if(raw.begin(), raw.end(), std::back_inserter(out),
                 [](uint8_t b) { return b != '\r'; });
    return out;
}

std::string blake3_hex(const std::vector<uint8_t>& data) {
    return hex_of(BLAKE3::Hasher::hash_32(data));
}

std::expected<std::string, std::string> last_sealed_hash(const std::string& seal_path) {
    std::ifstream in(seal_path);
    if (!in.is_open()) return std::unexpected("cannot read " + seal_path);
    std::string hash, tok;
    while (in >> tok) {
        if (is_hex64(tok)) hash = tok;
    }
    if (hash.empty()) return std::unexpected("no 64-hex hash in " + seal_path);
    return hash;
}

std::expected<VerifyResult, std::string> verify(const std::string& file,
                                                const std::string& seal_path) {
    auto raw = read_file(file);
    if (!raw) return std::unexpected(raw.error());
    auto sealed = last_sealed_hash(seal_path);
    if (!sealed) return std::unexpected(sealed.error());
    VerifyResult r{Verdict::drift, *sealed, blake3_hex(*raw),
                   blake3_hex(lf_canonical(*raw))};
    if (r.raw_hash == r.sealed_hash) r.verdict = Verdict::match_raw;
    else if (r.lf_hash == r.sealed_hash) r.verdict = Verdict::match_lf;
    return r;
}

std::expected<std::string, std::string> seal(const std::string& file) {
    auto raw = read_file(file);
    if (!raw) return std::unexpected(raw.error());
    const std::string line = std::format("{}\t{}\tkimi_truth v1 (LF-canonical)",
                                         zulu_now(), blake3_hex(lf_canonical(*raw)));
    const std::string seal_path = file + ".seal";
    std::ofstream out(seal_path, std::ios::app);
    if (!out.is_open()) return std::unexpected("cannot append " + seal_path);
    out << line << "\n";
    return line;
}

std::string zulu_now() {
    return std::format("{:%Y-%m-%dT%H:%M:%SZ}",
                       std::chrono::floor<std::chrono::seconds>(
                           std::chrono::system_clock::now()));
}

} // namespace kimi::truth

namespace {

int gate() {
    if (kimi::truth::selftest()) return 0;
    std::cout << "TRUTH: SELFTEST FAIL — linked BLAKE3 does not reproduce the "
                 "official KAT; refusing to say anything else (fail closed)\n";
    return 1;
}

} // namespace

int main(int argc, char** argv) {
    using namespace kimi::truth;
    const auto usage = "usage: kimi_truth <selftest|hash|verify|seal> <file> [seal]\n";
    if (argc < 2) { std::cerr << usage; return 2; }
    const std::string cmd = argv[1];

    if (cmd == "selftest" && argc == 2) {
        if (gate() != 0) return 1;
        std::cout << "TRUTH: SELFTEST PASS (BLAKE3 official empty vector, LFSSL)\n";
        return 0;
    }
    if (gate() != 0) return 1;  // no verdicts from an unproven hash

    if (cmd == "hash" && argc == 3) {
        auto raw = read_file(argv[2]);
        if (!raw) { std::cerr << "kimi_truth: " << raw.error() << "\n"; return 2; }
        std::cout << "RAW  " << blake3_hex(*raw) << "  " << argv[2] << "\n";
        std::cout << "LF   " << blake3_hex(lf_canonical(*raw)) << "  " << argv[2] << "\n";
        return 0;
    }
    if (cmd == "verify" && (argc == 3 || argc == 4)) {
        const std::string seal_path = argc == 4 ? argv[3] : std::string(argv[2]) + ".seal";
        auto r = verify(argv[2], seal_path);
        if (!r) { std::cerr << "kimi_truth: " << r.error() << " (fail closed)\n"; return 2; }
        switch (r->verdict) {
            case Verdict::match_raw:
                std::cout << "TRUTH: MATCH_RAW  " << argv[2] << "\n";
                return 0;
            case Verdict::match_lf:
                std::cout << "TRUTH: MATCH_LF   " << argv[2]
                          << "  (line-ending drift only — content intact)\n";
                return 0;
            case Verdict::drift:
                std::cout << "TRUTH: DRIFT      " << argv[2]
                          << "  (content changed since seal — unratified)\n"
                          << "  sealed: " << r->sealed_hash << "\n"
                          << "  raw:    " << r->raw_hash << "\n"
                          << "  lf:     " << r->lf_hash << "\n";
                return 1;
        }
    }
    if (cmd == "seal" && argc == 3) {
        auto line = seal(argv[2]);
        if (!line) { std::cerr << "kimi_truth: " << line.error() << "\n"; return 2; }
        std::cout << "SEALED " << *line << " -> " << argv[2] << ".seal\n";
        return 0;
    }
    std::cerr << usage;
    return 2;
}
