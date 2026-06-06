import serial
import time
import random 

# --- Configuration ---
COM_PORT = r'\\.\COM17'      
BAUD_RATE = 9600
SEND_SIZE = 512        # Payload size sent to MCU
RESPONSE_SIZE = 522    # 512 Payload + 8 Parity + 1 BCH Status + 1 CRC Status

# Test Settings
NUM_WORDS_TO_TEST = 10

def main():
    print(f"Opening {COM_PORT} at {BAUD_RATE} baud...")
    try:
        with serial.Serial(COM_PORT, BAUD_RATE, timeout=2.0, write_timeout=2.0) as ser:
            
            print("Port opened successfully! Giving USB stack 1 second to settle...")
            time.sleep(1.0) 
            
            ser.reset_input_buffer()
            ser.reset_output_buffer()
            
            print(f"Starting Stress Test: sending {NUM_WORDS_TO_TEST} words...\n")
            
            successful_responses = 0
            total_bch_corrections = 0
            total_bch_failures = 0
            total_crc_errors = 0
            
            start_time = time.time()
            
            for i in range(NUM_WORDS_TO_TEST):
                payload = bytearray(random.getrandbits(8) for _ in range(SEND_SIZE))
                
                try:
                    ser.write(payload)
                except serial.SerialTimeoutException:
                    print(f"Write timeout at word {i}! MCU stopped listening.")
                    break 
                
                response = ser.read(RESPONSE_SIZE)
                
                if len(response) == RESPONSE_SIZE:
                    successful_responses += 1
                    
                    # 1. Treat the entire 520-byte codeword as one giant Little-Endian integer
                    codeword_int = int.from_bytes(response[:520], byteorder='little')
                    
                    # 2. Shift right by 58 bits (32 for CRC + 26 for BCH) to recover the payload
                    recovered_payload_int = codeword_int >> 58
                    received_payload = recovered_payload_int.to_bytes(512, byteorder='little')
                    
                    # 3. The parity is actually at the bottom 58 bits! Mask them out.
                    raw_parity_int = codeword_int & ((1 << 58) - 1)
                    received_parity = raw_parity_int.to_bytes(8, byteorder='little')
                    
                    raw_bch = response[520]
                    raw_crc = response[521]
                    
                    bch_status = int.from_bytes([raw_bch], byteorder='little', signed=True)
                    crc_status = raw_crc
                    
                    # --- NEW RAW DEBUG OUTPUT ---
                    print(f"\n[Packet {i+1}]")
                    print(f"  Sent Payload (first 16) : {payload[:16].hex(' ').upper()}")
                    print(f"  Recv Payload (first 16) : {received_payload[:16].hex(' ').upper()}")
                    
                    # Exact comparison
                    if payload == received_payload:
                        print("  Payload Match           : YES (Exactly identical)")
                    else:
                        diff_count = sum(1 for a, b in zip(payload, received_payload) if a != b)
                        print(f"  Payload Match           : NO ({diff_count} bytes differ!)")
                        
                    print(f"  Extracted Parity        : {received_parity.hex(' ').upper()}")
                    print(f"  Raw BCH Byte            : 0x{raw_bch:02X} -> Signed: {bch_status}")
                    print(f"  Raw CRC Byte            : 0x{raw_crc:02X} -> Expected 1 (Valid) or 0 (Invalid)")
            
            end_time = time.time()
            
            elapsed_time = end_time - start_time
            throughput = (NUM_WORDS_TO_TEST * SEND_SIZE) / elapsed_time / 1024 
            
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
    except KeyboardInterrupt:
        print("\nTest safely aborted by user (Ctrl+C).")

if __name__ == '__main__':
    main()