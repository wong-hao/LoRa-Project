Input: List of node RF packet payloads payload(stack), corresponding same CRC value crc, cancidate mch, same RF packet payload size in bytes size
        same device address mote_addr, same 2-octets frame counter mote_fcnt, corresponding LoRa SNR ratios lsnr
Output: corrected RF packet payload corrected_payload

Algorithm 5 SOFT Algorithm
Function SOFT(crc, mch, payload(stack), size, mote_addr, mote_fcnt, lsnr(stack))
corrected_payload ← validateCRC(crc, softDecoding(payload(stack), lsnr(stack)), size, mote_fcnt, mote_addr)
if corrected_payload exists then
  Return corrected_payload
end if