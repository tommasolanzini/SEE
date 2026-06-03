import serial
import time
import random

# --- Configuration ---
COM_PORT = 'COM3'      
BAUD_RATE = 115200
SEND_SIZE = 512        # Payload size sent to MCU
RESPONSE_SIZE = 522    # 512 Payload + 8 Parity + 1 BCH Status + 1 CRC Status

# Test Settings
NUM_WORDS_TO_TEST = 100000

def main():
    print(f"Opening {COM_PORT} at {BAUD_RATE} baud...")
    try:
        with serial.Serial(COM_PORT, BAUD_RATE, timeout=2) as ser:
            print(f"Starting Stress Test: sending {NUM_WORDS_TO_TEST} words...\n")
            
            # Statistics counters
            successful_responses = 0
            total_bch_corrections = 0
            total_bch_failures = 0
            total_crc_errors = 0
            
            start_time = time.time()
            
            for i in range(NUM_WORDS_TO_TEST):
                # 1. Generate random data (512 bytes)
                payload = bytearray(random.getrandbits(8) for _ in range(SEND_SIZE))
                
                # 2. Send payload to the Microcontroller
                ser.write(payload)
                
                # 3. Read the response from the Microcontroller
                response = ser.read(RESPONSE_SIZE)
                
                if len(response) == RESPONSE_SIZE:
                    successful_responses += 1
                    
                    # 4. Extract status bytes at the end of the packet
                    # Byte 520 is BCH status (int8_t - signed)
                    # Byte 521 is CRC status (uint8_t - unsigned)
                    bch_status = int.from_bytes([response[520]], byteorder='little', signed=True)
                    crc_status = response[521]
                    
                    # Counting logic
                    if bch_status > 0:
                        total_bch_corrections += bch_status # Add the number of corrected bit flips
                    elif bch_status < 0:
                        total_bch_failures += 1 # Negative value indicates an uncorrectable error
                        
                    if crc_status != 0:
                        total_crc_errors += 1 # Non-zero CRC indicates a mismatch/failure
                        
                else:
                    print(f"Timeout or error at word {i}! Received {len(response)}/{RESPONSE_SIZE} bytes.")
                
                # (Optional) Print a progress update every 100 words
                if (i + 1) % 100 == 0:
                    print(f"Progress: {i + 1}/{NUM_WORDS_TO_TEST} words tested...")

            end_time = time.time()
            
            # --- PRINT FINAL REPORT ---
            elapsed_time = end_time - start_time
            throughput = (NUM_WORDS_TO_TEST * SEND_SIZE) / elapsed_time / 1024 # KB/s
            
            print("\n" + "="*40)
            print(" EDAC STRESS TEST RESULTS")
            print("="*40)
            print(f"Elapsed Time:          {elapsed_time:.2f} seconds")
            print(f"Upload Throughput:     {throughput:.2f} KB/s")
            print(f"Packets Received:      {successful_responses} / {NUM_WORDS_TO_TEST}")
            print(f"Total BCH Corrections: {total_bch_corrections}")
            print(f"Total BCH Failures:    {total_bch_failures}")
            print(f"Total CRC Errors:      {total_crc_errors}")
            print("="*40)
            
    except serial.SerialException as e:
        print(f"Unable to connect to the board. Error: {e}")

if __name__ == '__main__':
    main()