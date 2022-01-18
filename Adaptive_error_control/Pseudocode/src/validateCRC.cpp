# validate Message integrity based on CCITT crc-16 algorithm
Function validateCRC(crc, candidate, size, mote_fcnt, mote_addr)
if crc = sx1302_lora_payload_crc(candidate, size) then
  if validateMIC(candidate, mote_fcnt, size, mote_addr) = 1 then
    Return candidate
  end if
end if
