Algorithm 3 Enhanced Packet Combining Algorithm
Function EPC(crc, mch, payload(stack), size, mote_addr, mote_fcnt)
if corrected_payload not exists then
  for [a, b] in combnk(payload(stack), 2) do
    mask ← XOR(a, b)
  end if
  corrected_payload ← Correct(mask, mch, crc, size, mote_addr, mote_fcnt)
  if corrected_payload exists then
    Return corrected_payload
  end if
end if