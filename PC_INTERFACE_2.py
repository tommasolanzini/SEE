import serial
import struct
import os
import time

# --- Configuration ---
# Use the raw string prefix r'\\.\' to bypass the Windows registry bug for COM ports
COM_PORT = r'\\.\COM17' 
BAUD_RATE = 115200 # Make sure this matches your ATSAMV71 UART configuration

PAYLOAD_SIZE = 512
CODEWORD_SIZE = 598
STATUS_SIZE = 4 # 32-bit integer
TOTAL_RX_SIZE = CODEWORD_SIZE + STATUS_SIZE

def run_single_test():
    try:
        # 1. Open the Serial Port
        print(f"Opening {COM_PORT} at {BAUD_RATE} baud...")
        
        # Added write_timeout to protect against Windows kernel deadlocks
        with serial.Serial(COM_PORT, BAUD_RATE, timeout=5.0, write_timeout=5.0) as ser:
            
            print("Port opened successfully! Giving USB stack 1 second to settle...")
            # Crucial delay to allow the USB physical layer to stabilize
            time.sleep(1.0) 
            
            # Flush any leftover garbage data from previous runs
            ser.reset_input_buffer()
            ser.reset_output_buffer()
            
            # 2. Generate a 590-byte dummy payload
            original_payload = os.urandom(PAYLOAD_SIZE)
            
            # 3. Send to MCU
            print(f"Sending {PAYLOAD_SIZE} bytes to MCU...")
            try:
                ser.write(original_payload)
            except serial.SerialTimeoutException:
                print("\n[ERROR] Write timeout! The MCU stopped listening.")
                return
            
            # 4. Wait for and read the response
            print(f"Waiting for {TOTAL_RX_SIZE} bytes from MCU...")
            response = ser.read(TOTAL_RX_SIZE)
            
            if len(response) != TOTAL_RX_SIZE:
                print(f"\n[ERROR] Expected {TOTAL_RX_SIZE} bytes, got {len(response)}.")
                return

            # 5. Split the response into codeword and status
            codeword_bytes = response[:CODEWORD_SIZE]
            status_bytes = response[CODEWORD_SIZE:]
            
            # Unpack the 4-byte little-endian integer status
            correction_status = struct.unpack('<i', status_bytes)[0]
            print(f"MCU reported correction status: {correction_status} errors fixed.")

            # 6. The Magic: Undo the 58-bit shift
            # Convert the 598 bytes into one giant integer (Little Endian)
            codeword_int = int.from_bytes(codeword_bytes, byteorder='little')
            
            # Shift right by 58 bits to strip the CRC and BCH parity and realign the data
            extracted_int = codeword_int >> 58
            
            # Convert back to exactly 590 bytes
            extracted_payload = extracted_int.to_bytes(PAYLOAD_SIZE, byteorder='little')

            # 7. Verify!
            if original_payload == extracted_payload:
                print("\n[SUCCESS] Extracted payload perfectly matches the original!")
            else:
                print("\n[FAILED] Mismatch detected.")
                # Optional: print out where it failed for debugging
                for i in range(PAYLOAD_SIZE):
                    if original_payload[i] != extracted_payload[i]:
                        print(f"First mismatch at byte {i}: Sent {original_payload[i]}, Got {extracted_payload[i]}")
                        break

    except serial.SerialException as e:
        print(f"\n[ERROR] Serial Port Error: {e}")
    except KeyboardInterrupt:
        print("\nTest safely aborted by user (Ctrl+C).")

if __name__ == "__main__":
    run_single_test()