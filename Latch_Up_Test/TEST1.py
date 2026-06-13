import serial
import time

PORT = r'\\.\COM7'

print(f"1. Opening {PORT}...")
try:
    ser = serial.Serial(PORT, baudrate=115200, timeout=2.0, write_timeout=2.0)
    
    print("2. Port open! Giving USB stack 1 second to wake up...")
    time.sleep(1.0)

    print("3. Sending trigger 'A'...")
    ser.write(b'A')

    print("4. Waiting for response...")
    response = ser.readline()

    if response:
        print(f"5. MCU Replies: {response.decode('utf-8', errors='ignore').strip()}")
    else:
        print("5. Timeout: MCU didn't reply.")

except Exception as e:
    print(f"Error: {e}")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("6. Port safely closed.")