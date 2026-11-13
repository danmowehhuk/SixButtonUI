# Opens a pseudo-terminal, prints the slave path, then relays stdin to the
# master end - the slave stands in for the device's serial port.
import os
import sys
import tty

master, slave = os.openpty()
tty.setraw(slave)
print(os.ttyname(slave), flush=True)
for line in sys.stdin:
    os.write(master, line.encode())
