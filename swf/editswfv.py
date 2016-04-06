import sys
import struct

if len(sys.argv) < 3:
	print('%s version path' % (sys.argv[0]))
	sys.exit(1)

with open(sys.argv[2], 'rb+') as f:
	f.seek(3)
	f.write(struct.pack('B', int(sys.argv[1])))
	f.close()
