Input: List of node RF packet payloads payload(stack), corresponding same CRC value crc, cancidate payload mch, same RF packet payload size in bytes size
        same device address mote_addr, same 2-octets frame counter mote_fcnt
Output: corrected RF packet payload corrected_payload

Algorithm 4 Aggressive Packet Combining Algorithm
Function APC(crc, mch, payload(stack), size, mote_addr, mote_fcnt)
corrected_payload ← validateCRC(crc, majorityVoting(payload(stack)), size, mote_fcnt, mote_addr)
if corrected_payload exists then
    Return corrected_payload
else
    corrected_payload ← Correct(LeastReliableMask(payload(stack)), mch, crc, size, mote_addr, mote_fcnt)
    if corrected_payload exists then
        Return corrected_payload
    end if
end if