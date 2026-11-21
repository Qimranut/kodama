# Kodama

A C++20 library that traces sound through 3D scenes with ray tracing and renders
the result as binaural (headphone) audio using measured HRTFs.

You describe a room as triangles with absorbing materials, place a sound source
and a listener, and Kodama fires rays from the source, bounces them off the
surfaces, collects the ones that reach the listener, and turns those paths into a
left/right impulse response. Convolving a dry sound with that response places it
in the room.

> **Status:** early-stage and under active development. See
> [Limitations](#limitations) for what the physics does and does not model yet.

## Features

- **Scene** of triangles with per-material absorption
- **Ray propagation** with specular reflection, even sampling of directions
  (Fibonacci sphere), and early termination of rays that have lost too much energy
- **Spherical listener capture** with energy normalized so the result does not
  depend on ray count or sphere size ([how it works](#how-it-works))
- **Distance and air attenuation**: inverse-square spreading plus the classical
  Stokes-Kirchhoff absorption term
- **Binaural rendering**: SOFA HRTF datasets via
  [libmysofa](https://github.com/hoene/libmysofa), with fractional-sample delays
- **Convolution**: direct and FFT-based (radix-2 Cooley-Tukey)
- **16-bit WAV output**
- Header-only core with a small static library target (`KodamaCore`)

## Quick start

The full program is in [`examples/room_demo.cpp`](examples/room_demo.cpp). The
core of it:

```cpp
// A 6 x 4 x 3 m room, 10% absorption on every surface
materials::Registry materials;
const size_t plaster = materials.registerMaterial({ "plaster", 0.1f });

scene::Scene room;
// ... addTriangle({ v0, v1, v2, plaster }) for each face ...

// Source strength is the energy at referenceDist_ metres from the source
const acoustics::SoundSource source{ { 1.f, 1.f, 1.5f }, 1.f, 1.f };

acoustics::Listener listener;
listener.pos_ = { 5.f, 3.f, 1.5f };
listener.radius_ = 0.5f;

// Fire 200,000 rays, up to 20 bounces each; keep those that reach the listener
acoustics::Propagator propagator;
const auto paths = propagator.propagateMany(
    source, listener, room, materials, acoustics::AcousticMedium{}, 200000, 20);

// Turn the paths into a left/right impulse response with measured HRTFs
audio::SofaHRTFProvider hrtf;
std::string error;
hrtf.load("hrir.sofa", 48000, error);

const auto ir = acoustics::Response::buildBinaural(paths, 48000, listener, hrtf);

// Put a mono sound in the room, then write it out
audio::Buffer wet = audio::Convolver::processStereo(dryMonoBuffer, ir);
writer::writeWav16("room.wav", wet);
```

Run the complete example against any SOFA HRIR file:

```
KodamaRoomDemo path/to/hrir.sofa
```

In the example's 6 x 4 x 3 m room, tracing 200,000 rays and rendering a 0.4 second
stereo file takes a second or two in a Release build.

## Building

Requirements: a C++20 compiler, CMake 3.20 or newer, and
[vcpkg](https://github.com/microsoft/vcpkg) (libmysofa is installed from
`vcpkg.json` automatically).

1. Set the `VCPKG_ROOT` environment variable to your vcpkg folder.
2. Configure and build:

```
cmake --preset default
cmake --build build --config Release
```

### HRTF data

Kodama does not bundle any HRTF datasets. You need a SOFA file containing
head-related impulse responses; many are freely available from the
[SOFA conventions](https://www.sofaconventions.org/) database list. Check the
license of whichever dataset you use. The libmysofa repository also ships a
couple of small sample files in its `share/` folder.

## Tests

```
ctest --test-dir build -C Release --output-on-failure
```

`KodamaChecks` needs no audio files or SOFA data. It checks:

- the FFT against a plain DFT (sizes 2 to 4096) and FFT convolution against
  direct convolution
- the listener capture fraction against its closed form
- that free-field energy follows 1/d² and does not change with ray count or
  capture radius
- that the ray generator produces unit, evenly spread directions
- that the binaural response is finite and unchanged by refactors
- that the WAV writer header can be included from several translation units
  without link errors

`KodamaTest` is a scratch listening-test program. It expects local audio and
SOFA files and is not part of `ctest`.

## How it works

**Listener capture sphere.** A ray is an infinitely thin line, so it would never
hit a point-sized listener. Instead the listener is a sphere of radius
`Listener::radius_`, and any ray that passes through it counts as heard. Rays
fired evenly in all directions hit a sphere of radius `r` at distance `d` about
`N * r^2 / (4 d^2)` times, so hit density already carries the inverse-square
falloff. Each captured path's energy is therefore divided by the number of rays
expected to hit the sphere, so the summed energy for the direct sound is
`strength * (referenceDist / d)^2` regardless of how many rays are traced or how
large the sphere is. A larger sphere gives more hits (less noise) but blurs the
arrival direction and time slightly; aim for 100 or more hits on the paths that
matter.

**Impulse response.** Each captured path becomes a delayed, attenuated copy of
the HRIR for the direction the sound arrives from, with the HRTF's interaural
delay added and a 4-tap Lagrange filter for sub-sample timing. The copies are
summed into a left and right impulse response.

**Conventions.** World space is +X right, +Y forward, +Z up. A listener faces +Y
by default, and positive azimuth is to the listener's right.

## Limitations

- Reflections are perfectly specular; no scattering, diffraction or transmission
- Absorption does not depend on frequency, and each ray carries a single
  frequency
- The Stokes-Kirchhoff air term is classical theory only and is far smaller than
  measured air absorption at audible frequencies
- Paths are summed by amplitude, so paths landing in the same sample add
  coherently
- Intersection testing is brute force over all triangles (no acceleration
  structure yet)
- A source is a single mono signal at a point

## Next steps

- Bounding volume hierarchy for scene intersection
- Frequency-dependent absorption and ISO 9613-1 air absorption
- FFT convolution in the render path
- Energy-based accumulation of paths
