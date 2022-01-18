Algorithm 4 Aggressive Packet Combining Algorithm
Function APC(crc, mch, payload(stack), size, mote_addr, mote_fcnt)
if corrected_payload not exists then
  corrected_payload ← validateCRC(crc, majorityVoting(payload(stack)), size, mote_fcnt, mote_addr)
  if corrected_payload exists then
    Return corrected_payload
  else
    corrected_payload ← Correct(LeastReliableMask(payload(stack)), mch, crc, size, mote_addr, mote_fcnt)
    if corrected_payload exists then
      Return corrected_payload
    end if
  end if
end if