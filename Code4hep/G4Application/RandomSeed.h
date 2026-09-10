#ifndef Code4hep_G4Application_RandomSeed_h
#define Code4hep_G4Application_RandomSeed_h

#include <cstdint>

namespace c4h {

constexpr long geantEventSeed(std::uint32_t baseSeed, std::uint32_t run,
                              std::uint64_t event) {
  std::uint64_t value = static_cast<std::uint64_t>(baseSeed) ^
                        (static_cast<std::uint64_t>(run) << 32U) ^ event;
  value += 0x9e3779b97f4a7c15ULL;
  value = (value ^ (value >> 30U)) * 0xbf58476d1ce4e5b9ULL;
  value = (value ^ (value >> 27U)) * 0x94d049bb133111ebULL;
  value ^= value >> 31U;
  return 1L + static_cast<long>(value % 900000000ULL);
}

} // namespace c4h

#endif
