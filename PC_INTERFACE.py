import serial
import time
import random
import os
import csv
from datetime import datetime
# import os
# from playsound import playsound

# --- Configuration ---
COM_PORT      = r'\\.\COM13'
BAUD_RATE     = 9600
WORD_SIZE     = 512    # bytes sent to the MCU per word
RESPONSE_SIZE = 512    # bytes echoed back: raw NAND read-back, no BCH/CRC, no status
NUM_WORDS     = 10
OUTPUT_DIR    = os.path.join(".", "Test_Output")


def save_results(rows,i):
    """Write the collected per-word results to .\\Test_Output\\Test_DD_MM_YYYY_<ID>.csv"""
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    date_str = datetime.now().strftime("%d_%m_%Y")
    # raw_id   = input("Enter a test ID for the filename: ").strip()
    raw_id   = f"0f_10k_0{i}"
    test_id  = "".join(c for c in raw_id if c.isalnum() or c in ("-", "_")) or "run"
    path     = os.path.join(OUTPUT_DIR, f"Test_{date_str}_{test_id}.csv")

    with open(path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["word_index", "result", "diff_bytes", "first_diff_byte",
                    "sent_word_hex", "recv_word_hex"])
        for r in rows:
            w.writerow([r["idx"], r["result"], r["diff"], r["first"],
                        r["sent"], r["recv"]])
    print(f"Saved {len(rows)} rows -> {path}")


def main():
    # RAW LINK INTEGRITY TEST (PC <-> MCU <-> NAND):
    #   - No bit-flips injected, no BCH/CRC logic on the MCU.
    #   - The PC sends 512 random bytes; the MCU writes them verbatim to a NAND
    #     page, reads them straight back, and echoes the 512 bytes unchanged.
    #   - A clean link means recv == sent for every word.
    print(f"Opening {COM_PORT} at {BAUD_RATE} baud...")
    rows = []
    try:
        with serial.Serial(COM_PORT, BAUD_RATE, timeout=2.0, write_timeout=2.0) as ser:
            print("Port opened. Waiting for USB stack to settle...")
            time.sleep(1.0)
            ser.reset_input_buffer()
            ser.reset_output_buffer()
            print(f"Starting RAW link test (no BCH, no flips): {NUM_WORDS} words\n")

            ok = mismatches = short = 0
            total_diff = 0
            start = time.time()

            for i in range(NUM_WORDS):
                payload = bytearray(random.getrandbits(8) for _ in range(WORD_SIZE))

                try:
                    ser.write(payload)
                except serial.SerialTimeoutException:
                    print(f"[{i+1}] Write timeout - MCU stopped listening.")
                    break

                resp = ser.read(RESPONSE_SIZE)
                if len(resp) != RESPONSE_SIZE:
                    print(f"[{i+1}] Short response: {len(resp)}/{RESPONSE_SIZE} bytes")
                    short += 1
                    rows.append({
                        "idx": i + 1, "result": "SHORT", "diff": -1, "first": -1,
                        "sent": bytes(payload).hex(), "recv": bytes(resp).hex(),
                    })
                    continue

                ok += 1
                received = bytes(resp)

                match = (bytes(payload) == received)
                diff  = 0
                first = -1
                if not match:
                    mismatches += 1
                    for idx, (a, b) in enumerate(zip(payload, received)):
                        if a != b:
                            diff += 1
                            if first < 0:
                                first = idx
                    total_diff += diff

                status_str = "MATCH" if match else f"MISMATCH ({diff}B, first@{first})"
                sent_hex = payload.hex()[:32] + "..."
                recv_hex = received.hex()[:32] + "..."
                print(f"[{i+1:2}] sent: {sent_hex}")
                print(f"     recv: {recv_hex}   {status_str}")

                rows.append({
                    "idx":    i + 1,
                    "result": "MATCH" if match else "MISMATCH",
                    "diff":   diff,
                    "first":  first,
                    "sent":   bytes(payload).hex(),
                    "recv":   received.hex(),
                })

            elapsed = time.time() - start
            tp = (NUM_WORDS * WORD_SIZE) / elapsed / 1024 if elapsed > 0 else 0

            clean = (mismatches == 0 and short == 0 and ok == NUM_WORDS)

            print("\n" + "=" * 42)
            print("  RAW LINK TEST RESULTS (PC-MCU-NAND)")
            print("=" * 42)
            print(f"Elapsed Time:          {elapsed:.2f} s")
            print(f"Throughput:            {tp:.2f} KB/s")
            print(f"Words round-tripped:   {ok} / {NUM_WORDS}")
            print(f"Short responses:       {short}")
            print(f"Payload mismatches:    {mismatches}")
            print(f"Total differing bytes: {total_diff}")
            print(f"Link verdict:          {'CLEAN' if clean else 'DIRTY (see above)'}")
            print("=" * 42)

    except serial.SerialException as e:
        print(f"Cannot open port: {e}")
        return
    except KeyboardInterrupt:
        print("\nAborted.")
        return

    # -- Optional CSV export --------------------------------------------------
    if rows:
        answer = input("\nSave results to CSV? (y/n): ").strip().lower()
        if answer in ("y", "yes"):
            save_results(rows)
        else:
            print("Results not saved.")


if __name__ == '__main__':
    main()
