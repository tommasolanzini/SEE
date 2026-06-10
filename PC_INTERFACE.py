import serial
import time
import random
import os
import csv
from datetime import datetime

# --- Configuration ---
COM_PORT       = r'\\.\COM13'
BAUD_RATE      = 9600
PAYLOAD_SIZE   = 512   # bytes of payload sent to the MCU per word
RESPONSE_SIZE  = 522   # 512 payload + 8 parity + 1 BCH status + 1 CRC status
NUM_WORDS      = 10
OUTPUT_DIR     = os.path.join(".", "Test_Output")

# Protocol host -> MCU (per word): 1 control byte (flip count) + PAYLOAD_SIZE bytes.
# The MCU injects `flip count` bit errors into the codeword after BCH/CRC encoding.


# ── Small interactive helpers ────────────────────────────────────────────────
def ask_int(prompt, lo, hi):
    while True:
        raw = input(prompt).strip()
        try:
            v = int(raw)
        except ValueError:
            print(f"  Please enter an integer between {lo} and {hi}.")
            continue
        if lo <= v <= hi:
            return v
        print(f"  Value must be between {lo} and {hi}.")


def ask_yes_no(prompt):
    while True:
        raw = input(prompt).strip().lower()
        if raw in ("y", "yes"):
            return True
        if raw in ("n", "no"):
            return False
        print("  Please answer y or n.")


def sanitize_id(raw):
    """Keep the filename safe: letters, digits, dash and underscore only."""
    cleaned = "".join(c for c in raw.strip() if c.isalnum() or c in ("-", "_"))
    return cleaned or "run"


# ── CSV output ───────────────────────────────────────────────────────────────
def save_results(rows, num_flips):
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    date_str = datetime.now().strftime("%d_%m_%Y")
    test_id  = sanitize_id(input("Enter a test ID for the filename: "))
    path     = os.path.join(OUTPUT_DIR, f"Test_{date_str}_{test_id}.csv")

    with open(path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow([
            "word_index", "flips_injected", "bch_status", "crc",
            "payload_match", "diff_bytes", "sent_word_hex", "corrected_word_hex",
        ])
        for r in rows:
            w.writerow([
                r["idx"], num_flips, r["bch"],
                "OK" if r["crc_ok"] else "FAIL",
                "OK" if r["match"] else "MISMATCH",
                r["diff"], r["sent"], r["recv"],
            ])
    print(f"Saved {len(rows)} rows -> {path}")


# ── Test run ─────────────────────────────────────────────────────────────────
def run_test(ser, num_flips):
    """Stream NUM_WORDS words; return (rows, elapsed_seconds)."""
    rows = []
    start = time.time()

    for i in range(NUM_WORDS):
        payload = bytearray(random.getrandbits(8) for _ in range(PAYLOAD_SIZE))
        frame   = bytes([num_flips]) + bytes(payload)   # control byte + payload

        try:
            ser.write(frame)
        except serial.SerialTimeoutException:
            print(f"[{i+1}] Write timeout — MCU stopped listening.")
            break

        resp = ser.read(RESPONSE_SIZE)
        if len(resp) != RESPONSE_SIZE:
            print(f"[{i+1}] Short response: {len(resp)}/{RESPONSE_SIZE} bytes")
            continue

        # The firmware returns the already-decoded, byte-aligned payload in the
        # first 512 bytes, then 8 parity bytes, then BCH status, then CRC status.
        received_payload = bytes(resp[:512])
        bch_raw    = resp[520]
        crc_raw    = resp[521]
        bch_status = bch_raw if bch_raw < 128 else bch_raw - 256  # signed
        crc_ok     = (crc_raw == 1)

        match = (bytes(payload) == received_payload)
        diff  = 0 if match else sum(a != b for a, b in zip(payload, received_payload))

        status_str = (f"BCH={bch_status:+d}  CRC={'OK' if crc_ok else 'FAIL'}"
                      f"  payload={'OK' if match else f'MISMATCH({diff}B)'}")
        print(f"[{i+1:2}] sent: {payload.hex()[:32]}...")
        print(f"     recv: {received_payload.hex()[:32]}...   {status_str}")

        rows.append({
            "idx":    i + 1,
            "sent":   bytes(payload).hex(),
            "recv":   received_payload.hex(),
            "bch":    bch_status,
            "crc_ok": crc_ok,
            "match":  match,
            "diff":   diff,
        })

    return rows, time.time() - start


def print_summary(rows, elapsed, num_flips):
    received       = len(rows)
    mismatches     = sum(1 for r in rows if not r["match"])
    bch_corrected  = sum(1 for r in rows if r["bch"] > 0)
    bch_failed     = sum(1 for r in rows if r["bch"] < 0)
    crc_errors     = sum(1 for r in rows if not r["crc_ok"])
    tp = (NUM_WORDS * PAYLOAD_SIZE) / elapsed / 1024 if elapsed > 0 else 0.0

    print("\n" + "=" * 42)
    print("  EDAC STRESS TEST RESULTS")
    print("=" * 42)
    print(f"Flips injected/word:   {num_flips}")
    print(f"Elapsed Time:          {elapsed:.2f} s")
    print(f"Throughput:            {tp:.2f} KB/s")
    print(f"Packets received:      {received} / {NUM_WORDS}")
    print(f"Payload mismatches:    {mismatches}")
    print(f"BCH corrections:       {bch_corrected}")
    print(f"BCH failures:          {bch_failed}")
    print(f"CRC errors:            {crc_errors}")
    print("=" * 42)


def main():
    print("=== EDAC Stress Test ===")
    num_flips = ask_int("How many bit flips to inject per word? (0-255): ", 0, 255)

    if not ask_yes_no(f"Proceed with the test ({NUM_WORDS} words, {num_flips} flips/word)? (y/n): "):
        print("Aborted.")
        return

    print(f"\nOpening {COM_PORT} at {BAUD_RATE} baud...")
    rows, elapsed = [], 0.0
    try:
        with serial.Serial(COM_PORT, BAUD_RATE, timeout=2.0, write_timeout=2.0) as ser:
            print("Port opened. Waiting for USB stack to settle...")
            time.sleep(1.0)
            ser.reset_input_buffer()
            ser.reset_output_buffer()
            print(f"Starting stress test: {NUM_WORDS} words, {num_flips} flips/word\n")
            rows, elapsed = run_test(ser, num_flips)
    except serial.SerialException as e:
        print(f"Cannot open port: {e}")
        return
    except KeyboardInterrupt:
        print("\nAborted.")
        return

    if not rows:
        print("No words were received — nothing to summarise or save.")
        return

    print_summary(rows, elapsed, num_flips)

    if ask_yes_no("\nSave results to CSV? (y/n): "):
        save_results(rows, num_flips)
    else:
        print("Results not saved.")


if __name__ == '__main__':
    main()
