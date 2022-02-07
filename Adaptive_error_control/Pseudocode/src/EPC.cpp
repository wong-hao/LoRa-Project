Input: List of node RF packet payloads payload(stack), corresponding same CRC value crc, cancidate payload mch, same RF packet payload size in bytes size,
        same device address mote_addr, same 2-octets frame counter mote_fcnt
Output: corrected RF packet payload corrected_payload

Algorithm 3 Enhanced Packet Combining Algorithm
Function EPC(crc, mch, payload(stack), size, mote_addr, mote_fcnt)
for [a, b] in combnk(payload(stack), 2) do
    mask ← XOR(a, b)
end if
corrected_payload ← Correct(mask, mch, crc, size, mote_addr, mote_fcnt)
if corrected_payload exists then
    Return corrected_payload
end if