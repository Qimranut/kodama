// A second translation unit that includes wav_writer.hpp.
//
// checks.cpp includes the same header, so if writeWav16 is not `inline`
// the linker reports a duplicate definition (GCC/Clang: "multiple definition",
// MSVC: LNK2005) and KodamaChecks will not build at all.

#include <writer/wav_writer.hpp>

void kodamaOdrCheck() {}
