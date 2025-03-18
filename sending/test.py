import serial

ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)  # 請修改為正確的 USB 埠
ser.flush()

while True:
    user_input = input("輸入指令: ")
    ser.write(user_input.encode('utf-8') + b'\n')  # 傳送指令到 Arduino
    
    response = ser.readline().decode('utf-8').strip()
    print("來自 Arduino 的回應:", response)
