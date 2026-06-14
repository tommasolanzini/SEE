# SEE Mitigation — Space Engineering Practical (SEP)
**TU Delft AE4897 — Space Engineering Practical**

Authors: G.M.M. Bellumé, C. Koleszar, S. Ilegitim, T. Lanzini, E. Pascanean

---

## Project overview

This repository contains the firmware, host-side test scripts, and simulation
tools developed for a verification campaign on **Single Event Effect (SEE)
mitigation** applied to a COTS-based spacecraft mass memory subsystem. The
subsystem pairs a **Microchip ATSAMV71Q21B MCU** with a **Micron
MT29F32G08CBADAWP MLC NAND Flash** and targets a two-year Low Earth Orbit
mission at 600 km SSO.

The campaign is structured around three test activities:

- **Software Test** — functional verification of the BCH+CRC firmware through
  controlled bit-flip injection directly on the MCU, with the NAND excluded
  from the loop (see [dry-run findings](#dry-run)).
- **Latch-Up Test** — bench characterisation of the LT6118-based hardware
  protection circuit through on-board fault-injection MOSFETs, measuring
  cut-off time and recovery against the 10 µs / 30 s requirements.
- **Radiation Test** — proton-beam exposure at Holland PTC (planned; not
  executed within the project timeline — see report §1.x).

---

## Repository structure
├── Software_Test/

│   ├── Space_Engineering_Practical.X/   # MPLAB X firmware project (XC32)

│   │   ├── main.c                        # MCU entry point, USB-CDC init

│   │   ├── app.c / app.h                 # BCH+CRC encode/decode pipeline

│   │   └── gf2_poly.cpp / .h            # GF(2^13) BCH codec (shared)

│   └── PC_INTERFACE.py                  # Host test orchestration script

├── Latch_Up_Test/

│   └── ...                              # LUP firmware and acquisition script│

---

## Core EDAC scheme

A 512-byte payload is protected with a **BCH(m=13, t=2) + CRC-32 codec over
GF(2¹³)**, producing a 520-byte codeword (512 B payload + 4 B BCH parity +
4 B CRC). The codec (`gf2_poly`) is shared between the embedded firmware and
the offline simulation tools so that hardware results and analytical predictions
use the exact same mathematical implementation.

BCH alone has a non-negligible miscorrection probability (~3×10⁻²) when the
number of errors exceeds *t*. The appended CRC-32 reduces the probability of an
undetected miscorrection to ~7×10⁻¹², satisfying requirement MMS.05.03.

---

## Hardware

| Component | Design (rad-hard) | Test surrogate (COTS) |
|---|---|---|
| MCU | SAMV71Q21RT | ATSAMV71Q21B |
| NAND Flash | 3× UT81NDQ512G8T (SLC) | MT29F32G08CBADAWP (MLC) |
| LUP circuit | LT6118 + IRLML6346 + FDC638P | unchanged |

The MLC surrogate exhibits a high intrinsic Raw Bit Error Rate (RBER), which
motivated excluding the NAND from the software test loop.

---

## Key results

| Test | Primary metric | Result | Requirement |
|---|---|---|---|
| Software (EDAC) | Uncorrectable error rate | 3.11×10⁻¹³ ± 1.2×10⁻¹⁵ bit⁻¹s⁻¹ | < 10⁻¹² bit⁻¹s⁻¹ (MMS.05.01) |
| Software (EDAC) | CRC false positives | 0 / 300,000 words | 0 (MMS.05.03) |
| Latch-Up | Mean cut-off time | 8.72 µs ± 0.16 µs (100 shots) | < 10 µs (MMS.04.01) |
| Latch-Up | Recovery time | ~100 µs | < 30 s (MMS.04.02) |
| Radiation | — | Not executed | — |

---

## Report

The full verification report is available in the project Overleaf. The
repository firmware version used for testing corresponds to the tagged release
specified in the report.
