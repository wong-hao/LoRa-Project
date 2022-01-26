Intput: The first receive window RX1 RECEIVE_DELAY1;
        the total on-the-air transmission time of a uplink and a downlink LoRa packet TOA;

Function Correct(mask, mch, crc, size, mote_addr, mote_fcnt)
# Brute-force search the candidate in finite time
while time_use < RECEIVE_DELAY1 - TOA do
  while brute_force(mask) exists do
    corrected_payload ← validateCRC(crc, XOR(brute_force(mask), mch), size, mote_addr, mote_fcnt)
  end while
  if corrected_payload exists then
    Return corrected_payload
  end if
end while