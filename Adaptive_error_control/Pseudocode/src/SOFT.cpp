Algorithm 5 SOFT Algorithm
Function SOFT(crc, mch, payload(stack), size, mote_addr, mote_fcnt, lsnr(stack))
if corrected_payload not exists then
  corrected_payload ← validateCRC(crc, softDecoding(payload(stack), lsnr(stack)), size, mote_fcnt, mote_addr)
  if corrected_payload exists then
    Return corrected_payload
  end if
end if