# ARP-OBERON 2608 Ultimate - Travis Scott Edition

**A 1970s-inspired dual-synth monster** combining the legendary ARP 2600 semi-modular architecture (left panel) with the rare CRB Elettronica Oberon (right panel).  

Built as a single-file Cmajor processor for maximum portability and low CPU usage.

## Features

- **Full ARP 2600 Left Side** — 3 VCOs with Saw, Square, Triangle and PWM
- **Oberon Right Side** — Triple-oscillator core with dedicated AM generator + 4-pole Oberon Ladder Filter (LP / HP / All-Pass modes)
- **25×25 Patch Bay** — 625 possible modulation routings (full matrix)
- **500 Procedural Presets** — Categorized into Pads, Leads, Bass, FX, and Plucks with Travis Scott / 1970s character
- **Advanced FX Chain**:
  - Airwindows-style console drive & saturation
  - Sound Entropy (organic controlled chaos)
  - Temanforce (harmonic tension + transient punch)
  - Secret Tape Machine (Studer A80-style wow/flutter + saturation)
- **8 Stereo Channels** — Multi-timbral layering support
- **Sample & Hold + Voltage Processor** (attenuverter + offset)
- Extremely low CPU (< 3%)

## Patch Bay (25×25 Matrix)

**Sources (0-24)**:
- 0 = None
- 1-3 = ARP VCO 1-3
- 4 = Rectifier / Envelope Follower
- 5 = Sample & Hold
- 6 = Envelope 1
- 7 = White Noise
- 8 = Pink Noise
- 9-10 = LFO 1-2
- 11-12 = External Audio Input L/R
- 13-24 = Reserved for future expansion

**Destinations (0-24)**:
- 0-2 = ARP VCO Frequency / PWM
- 3 = ARP VCF Cutoff
- 4 = ARP VCF Resonance
- 5 = Oberon Cutoff
- 6 = Oberon Resonance
- 7 = Oberon AM Depth
- 8-24 = Reserved

The `patchMatrix` array is a flattened 25×25 matrix. Index = `source * 25 + destination`.

## FX Chain Order

1. ARP 2600 / Oberon voice generation
2. Oberon 4-pole Ladder Filter
3. Sound Entropy (adds organic movement)
4. Temanforce (harmonic excitation + punch)
5. Airwindows-style console drive
6. Secret Tape Machine (final glue)

## Preset Categories (0–499)

- **0–99**: Atmospheric Pads (Astro Clouds, Utopia Glass, Sicko Atmosphere)
- **100–199**: Sicko Leads & Stabs (Goosebumps, Highest In The Room style)
- **200–299**: Deep Bass & Sub (40Hz Sub, Lost Forever, Rage Bass)
- **300–399**: FX & Entropy Swells (Tape Glitch, Spring Reverb chaos)
- **400–499**: Plucks & Percussive (FE!N Stab, Stargazing Pluck, Escape Plan)

## Installation & Usage

1. Copy the `.cmajor` file into your Cmajor projects folder
2. Open in the Cmajor IDE or compile to VST3/AU/CLAP
3. Load in your DAW
4. Select preset 0–499 for instant inspiration
5. Engage **Secret Tape** for classic 1970s warmth
6. Use the patch matrix to create complex, evolving modulations

## Tags

`#analog` `#virtualanalog` `#1970s` `#ARP2600` `#Oberon` `#TravisScott` `#synth` `#VST3` `#AU` `#CLAP` `#patchbay` `#modular` `#tape` `#saturation` `#entropy` `#airwindows` `#temanforce` `#padmonster` `#leadmachine` `#bassmonster` `#fxmachine` `#lowcpu` `#singlefile`

## Credits & Inspiration

- ARP 2600 (1970–1971) – Semi-modular legend
- CRB Elettronica Oberon (1979) – Rare Italian museum synth
- Airwindows – Console & saturation algorithms
- Travis Scott – Modern pad/lead aesthetic
- Classic 1970s tape machines (Studer A80 influence)

Made with love for the golden era of analog synthesis.

Enjoy the monster.

---

**Repository**: https://github.com/TheRealla/Leverage-_Travis-Scott-Edition-Arp-2600-Oberon-

**License**: MIT (free to use, modify, and distribute)
