import serial
import time
import random
import os
import csv
from datetime import datetime
# import os
# from playsound import playsound

# --- Configuration ---
BAUD_RATE      = 9600   
SEND_SIZE      = 512
RESPONSE_SIZE  = 522   # 512 payload + 8 parity + 1 BCH status + 1 CRC status
NUM_WORDS      = 100000
OUTPUT_DIR     = os.path.join(".", "Test_Output")


def save_results(rows,i):
    """Write the collected per-word results to .\\Test_Output\\Test_DD_MM_YYYY_<ID>.csv"""
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    date_str = datetime.now().strftime("%d_%m_%Y")
    # raw_id   = input("Enter a test ID for the filename: ").strip()
    raw_id   = f"0f_10k_MCU_{i}"
    test_id  = "".join(c for c in raw_id if c.isalnum() or c in ("-", "_")) or "run"
    path     = os.path.join(OUTPUT_DIR, f"Test_{date_str}_{test_id}.csv")

    with open(path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["word_index", "bch_status", "crc", "payload_match",
                    "diff_bytes", "sent_word_hex", "corrected_word_hex"])
        for r in rows:
            w.writerow([r["idx"], r["bch"],
                        "OK" if r["crc_ok"] else "FAIL",
                        "OK" if r["match"] else "MISMATCH",
                        r["diff"], r["sent"], r["recv"]])
    print(f"Saved {len(rows)} rows -> {path}")


def main():
    COM_number = input("Insert Communication port number: ")
    COM_PORT = fr'\\.\COM{COM_number}'
    print(f"Opening {COM_PORT} at {BAUD_RATE} baud...")
    for j in range(1,31):
        rows = []
        try:
            with serial.Serial(COM_PORT, BAUD_RATE, timeout=2.0, write_timeout=2.0) as ser:
                print("Port opened. Waiting for USB stack to settle...")
                time.sleep(1.0)
                ser.reset_input_buffer()
                ser.reset_output_buffer()
                print(f"Starting stress test: {NUM_WORDS} words\n")

                ok = bch_corrections = bch_failures = crc_errors = payload_mismatches = 0
                start = time.time()

                for i in range(NUM_WORDS):
                    payload = bytearray(random.getrandbits(8) for _ in range(SEND_SIZE))

                    try:
                        ser.write(payload)
                    except serial.SerialTimeoutException:
                        print(f"[{i+1}] Write timeout — MCU stopped listening.")
                        break

                    resp = ser.read(RESPONSE_SIZE)
                    if len(resp) != RESPONSE_SIZE:
                        print(f"[{i+1}] Short response: {len(resp)}/{RESPONSE_SIZE} bytes")
                        continue

                    ok += 1

                    # ── Decoding ────────────────────────────────────────────────
                    # The firmware does NOT ship the raw codeword: it runs the BCH/CRC
                    # decode on-chip and returns the already-decoded, byte-aligned
                    # payload in the first 512 bytes, then 8 parity bytes, then the
                    # BCH status byte and the CRC status byte.
                    # (The codeword itself is bit-systematic, not byte-systematic:
                    #  the payload is shifted up by 58 bits, so resp[:512] of the raw
                    #  codeword would NOT be the payload — see gf2_extract_payload.)
                    received_payload = bytes(resp[:512])
                    received_parity  = bytes(resp[512:520])
                    bch_raw          = resp[520]
                    crc_raw          = resp[521]
                    bch_status       = bch_raw if bch_raw < 128 else bch_raw - 256  # signed
                    crc_ok           = (crc_raw == 1)

                    if bch_status > 0:
                        bch_corrections += 1
                    elif bch_status < 0:
                        bch_failures += 1
                    if not crc_ok:
                        crc_errors += 1

                    match = (bytes(payload) == received_payload)
                    diff  = 0
                    if not match:
                        payload_mismatches += 1
                        diff = sum(a != b for a, b in zip(payload, received_payload))

                    status_str = (f"BCH={bch_status:+d}  CRC={'OK' if crc_ok else 'FAIL'}"
                                f"  payload={'OK' if match else f'MISMATCH({diff}B)'}")
                    sent_hex = payload.hex()[:32] + "..." if SEND_SIZE > 16 else payload.hex()
                    recv_hex = received_payload.hex()[:32] + "..." if SEND_SIZE > 16 else received_payload.hex()
                    # print(f"[{i+1:2}] sent: {sent_hex}")
                    # print(f"     recv: {recv_hex}   {status_str}")

                    # Collect the full-word result for optional CSV export.
                    rows.append({
                        "idx":    i + 1,
                        "sent":   bytes(payload).hex(),
                        "recv":   received_payload.hex(),
                        "bch":    bch_status,
                        "crc_ok": crc_ok,
                        "match":  match,
                        "diff":   diff,
                    })

                elapsed = time.time() - start
                tp = (NUM_WORDS * SEND_SIZE) / elapsed / 1024

                print("\n" + "=" * 42)
                print("  EDAC STRESS TEST RESULTS")
                print("=" * 42)
                print(f"Elapsed Time:          {elapsed:.2f} s")
                print(f"Throughput:            {tp:.2f} KB/s")
                print(f"Packets received:      {ok} / {NUM_WORDS}")
                print(f"Payload mismatches:    {payload_mismatches}")
                print(f"BCH corrections:       {bch_corrections}")
                print(f"BCH failures:          {bch_failures}")
                print(f"CRC errors:            {crc_errors}")
                print("=" * 42)

        except serial.SerialException as e:
            print(f"Cannot open port: {e}")
            return
        except KeyboardInterrupt:
            print("\nAborted.")
            return
        # sound_path = os.path.abspath('Dolci_Peccati.mp3')
        # playsound(sound_path)
        # ── Optional CSV export ──────────────────────────────────────────────────
        if rows:
            # answer = input("\nSave results to CSV? (y/n): ").strip().lower()
            # if answer in ("y", "yes"):
            save_results(rows, j)
            # else:
            #     print("Results not saved.")


if __name__ == '__main__':
    main()
