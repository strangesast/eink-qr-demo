'''
'''
import time
import qrcode
import serial
from itertools import zip_longest


def get_qr(data):
    qr = qrcode.QRCode(border=0)
    qr.add_data(data)
    return qr.get_matrix()


def serialize(bits):
    args = [iter(bits)] * 8
    for b in zip_longest(fillvalue=0, *args):
        v = 0
        for i, bb in enumerate(b):
            v |= bb << i
        yield v


def deserialize(buf, size):
    bits = [bool((b >> i) & 1) for b in buf for i in range(8)]
    return bits


def toast(buf, s):
    for j in range(s):
        for i in range(s):
            ij = j * s + i
            print('X' if buf[ij // 8] >> (ij % 8) & 1 else ' ', end='')
        print()            


def main():
    with serial.Serial('/dev/ttyUSB0', 115200) as ser:
        while True:
            text = input().strip()
            print(f'{text=}')
            #'https://www.google.com'

            data = get_qr(text)
            size = len(data)
            buf = bytearray(serialize(y for x in data for y in x))

            ser.write(f'text:{size},{text}\n'.encode())
            ser.write(buf)

            for buf in ser:
                print(buf)
                line = buf.decode().strip()
                if 'done' in line:
                    break
            #deserialize(buf, size)

if __name__ == '__main__':
    main()
