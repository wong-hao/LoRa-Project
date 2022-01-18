Intput: RECEIVE_DELAY1: The first receive window RX1; TOA: the total on-the-air transmission time of a LoRa packet

latency ← RECEIVE_DELAY1 - TOA

# Brute-force search the candidate in finite time
Function Correct(mask, mch, crc, size, mote_addr, mote_fcnt)
while time_use < latency do
  while brute_force(mask) exists do
    corrected_payload ← validateCRC(crc, XOR(brute_force(mask), mch), size, mote_addr, mote_fcnt)
  end while
  if corrected_payload exists then
    Return corrected_payload
  end if
end while