import serial
import time
import random

# --- Configuration ---
COM_PORT       = r'\\.\COM13'
BAUD_RATE      = 9600
SEND_SIZE      = 512  
RESPONSE_SIZE  = 522   # 512 payload + 8 parity + 1 BCH status + 1 CRC status
NUM_WORDS      = 10

def main():
    print(f"Opening {COM_PORT} at {BAUD_RATE} baud...")
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
                print(f"[{i+1:2}] sent: {sent_hex}")
                print(f"     recv: {recv_hex}   {status_str}")

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
    except KeyboardInterrupt:
        print("\nAborted.")

if __name__ == '__main__':
    main()