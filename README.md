# SEE Mitigation — Space Engineering Practical (SEP)

TU Delft Space Engineering Practical project on **Single Event Effects (SEE)
mitigation** on **COTS (commercial off-the-shelf) hardware**. The goal is to
protect data stored in an external NAND flash against radiation-induced bit
errors using an **EDAC (Error Detection And Correction)** scheme, and to
characterise its performance both on real hardware and in simulation.

**Authors:** G.M.M. Bellumé, C. Koleszar, S. Ilegitim, T. Lanzini, E. Pascanean.

---

## Core idea

A 512-byte data payload is protected with a **BCH(t=2) + CRC-32 codec over
GF(2¹³)**, producing a 520-byte codeword (512 payload + 8 parity). The codeword
is stored, deliberately corrupted with injected bit flips (or by real radiation
during a latch-up / beam campaign), read back, and corrected. The codec
(`gf2_poly`) is shared between the embedded firmware and the offline tools so
that hardware results and simulations use the exact same math.

---

## Repository layout

The repo contains two parallel work streams, each a self-contained MPLAB X
embedded project plus its companion PC-side Python tooling.

### `Latch_Up_Test/` — hardware-in-the-loop
Firmware and host scripts for running the EDAC on a Microchip SAM / PIC32
microcontroller (MPLAB Harmony 3, USB CDC).

- **`src/main.c`** — main application state machine: receives a payload over USB
  CDC, encodes it (BCH+CRC), bit-bangs it to an external NAND flash chip via the
  ONFI protocol, optionally injects bit flips, reads it back, corrects errors,
  and returns the codeword plus BCH/CRC status to the PC.
- **`src/gf2_poly.*`, `gf2_poly.cpp`** — the heap-free BCH/CRC codec.
- **`src/app.c`** — Harmony USB-CDC glue (manages the COM port).
- **`PC_INTERFACE.py`** — host-side EDAC stress test (send random words, tally
  corrections / failures / mismatches, report throughput).
- **`CHECK_WORKING_PAGES.py`, `TEST1.py`** — flash page / physical-layer health
  checks over the serial link.
- **`plot_EDAC_C.py`, `test_bch.cpp`, `working_pages.c`** — supporting test and
  plotting code.
- **`Space_Engineering_Practical.X/`, `src/config/`, `src/packs/`** — MPLAB X
  project and Harmony-generated framework / device-pack code (auto-generated).

### `Software_Test/` — offline simulation & analysis
A PC-only counterpart used to model the radiation environment and validate the
codec without hardware.

- **`env_simulator.py`** — Monte-Carlo simulation of the on-orbit upset rate
  (SPENVIS-derived bit-error rate, scrubbing cadence, MBU statistics) estimating
  the residual uncorrectable-error rate over the mission lifetime.
- **`env_simulator_plot.py`, `plot_EDAC_C.py`** — plotting of simulation /
  campaign results.
- **`ANALYZE_BCH_CAMPAIGN.py`, `ANALYZE_Dry_Runs.py`, `COMPLETE_ANALYSIS.py`** —
  post-processing of test-campaign data.
- **`runs_augmented.csv`** — accumulated simulation output.
- **`gf2_poly.cpp`, `test_bch.cpp`, `main_real.c`** — the same codec and a BCH
  test harness for offline runs.

> Most files under `*/src/config/`, `*/src/packs/`, `*.X/nbproject/`, and the
> `*_default*` component folders are MPLAB Harmony / MPLAB X generated artifacts,
> not hand-written project code.

---

## Typical workflow

1. **Hardware:** build `Latch_Up_Test/Space_Engineering_Practical.X` in MPLAB X
   and flash the MCU. Connect over USB; the board enumerates as a COM port.
2. Run `PC_INTERFACE.py` (adjust `COM_PORT`) to stream payloads through the
   encode → store → corrupt → read-back → correct pipeline and collect BCH/CRC
   statistics.
3. **Simulation:** run `Software_Test/env_simulator.py` to project the
   uncorrectable-error rate for the chosen scrubbing frequency and mission
   duration, then visualise with the plotting / analysis scripts.

---

## Requirements

- **Embedded:** MPLAB X IDE, XC32 compiler, MPLAB Harmony 3 (for the `.X`
  projects).
- **Host:** Python 3 with `pyserial`, `numpy`, `tqdm`, and `matplotlib`.
- A C++17 compiler (e.g. `g++`) to build the offline `test_bch` / `gf2_poly`
  tools.