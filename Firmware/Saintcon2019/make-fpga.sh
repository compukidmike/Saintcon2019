
# convert the fpga binary to a intel hex
OFFSET=0x801f000

objcopy --change-addresses $OFFSET -I binary --output ihex fpga.bin fpga.ihex
