import serial
import time

# --- Configuration ---
COM_PORT   = r'\\.\COM17'
BAUD_RATE  = 9600          # irrelevant for USB CDC
SEND_SIZE  = 512           # trigger payload (content ignored by MCU)
RESP_SIZE  = 528           # 520 codeword + 8 trailer bytes
NUM_PAGES  = 50            # how many sequential pages to write+read

def main():
    print(f"Opening {COM_PORT} ...")
    with serial.Serial(COM_PORT, BAUD_RATE, timeout=3.0, write_timeout=3.0) as ser:
        time.sleep(1.0)
        ser.reset_input_buffer()
        ser.reset_output_buffer()

        trigger = bytes(SEND_SIZE)
        results = []
        print(f"Sequential write+read on {NUM_PAGES} pages...\n")

        for i in range(NUM_PAGES):
            ser.write(trigger)
            resp = ser.read(RESP_SIZE)
            if len(resp) != RESP_SIZE:
                print(f"[{i}] SHORT RESPONSE: got {len(resp)} bytes")
                continue

            mism_bytes = resp[520]
            mism_bits  = resp[521] | (resp[522] << 8)
            first_off  = resp[523] | (resp[524] << 8)
            page       = resp[525] | (resp[526] << 8)
            erased     = resp[527] & 0x01

            off_str = "-" if first_off == 0xFFFF else str(first_off)
            tag = " (erase+w+r)" if erased else ""
            flag = "" if mism_bytes == 0 else "  <-- ERROR"
            print(f"page {page:3d}{tag:13s} mismatch_bytes={mism_bytes:3d}  "
                  f"mismatch_bits={mism_bits:4d}  first_offset={off_str}{flag}")
            results.append((page, mism_bytes, mism_bits, first_off))

        if not results:
            print("No valid responses.")
            return

        print("\n" + "=" * 52)
        bad = [r for r in results if r[1] > 0]
        print(f"Pages clean : {len(results) - len(bad)}/{len(results)}")
        print(f"Pages bad   : {len(bad)}/{len(results)}")

        if not bad:
            print("\nVERDICT: sequential multi-page write+read is CLEAN.")
            print("  -> The PHYSICAL layer is fully fine.")
            print("  -> The corruption is in the gf2 encode/decode path or")
            print("     in the PC big-integer '>>58' unpacking. Test gf2 next.")
        else:
            bad_pages = ", ".join(str(r[0]) for r in bad)
            print(f"\nVERDICT: physical write+read FAILS on pages: {bad_pages}")
            print("  -> Page-dependent physical problem (write-disturb,")
            print("     sequential-program rule, or fast write+read cadence).")
            # quick hint: do failures cluster after page 0?
            if all(r[0] > 0 for r in bad):
                print("  Note: page 0 is clean, failures only on later pages")
                print("        -> strongly suggests it is NOT raw bit-bang but")
                print("           something accumulating per program cycle.")
        print("=" * 52)


if __name__ == '__main__':
    main()