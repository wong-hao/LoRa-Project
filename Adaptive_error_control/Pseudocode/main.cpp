RECEIVE_DELAY1: The first receive window RX1
TOA: the total on-the-air transmission time of a LoRa packet
NWKSKEY: network session key specific for the end-devices
DEVADDR: device addresses
data: Base64 encoded RF packet payload, padded
time: UTC time of pkt RX, us precision, ISO 8601 'compact' format
lsnr: Lora SNR ratio in dB (signed float, 0.1 dB precision)
stat: CRC status: 1 = OK, -1 = fail, 0 = no CRC
size: RF packet payload size in bytes (unsigned integer)
mote_fcnt: 2-octets frame counter
latency: max computation time counter for timestamping, 1 microsecond resolution



Output: corrected packet







latency ← RECEIVE_DELAY1 - TOA
[NWKSKEY(stack), DEVADDR(stack)] ← Activation By Personalization

# Socket collect gateway mac address
MAC_address ← read(message)
# Match collected gateway mac address with predefined one
if MAC_address in predefined_gateway_address then
    # Parse JSON struct to get metadata
    [data(stack), time(stack)] ← json.Unmarshal(message)
    # Enough data collected
    if len(data(stack)) = len(predefined_gateway_address) then
      # Compare metadata(stack) (time)
      if metadata(stack) all equals then
        # Parse JSON struct to get metadata
        [stat(stack), crc(stack), lsnr(stack), size(stack)] ← json.Unmarshal(message)
        # Base64 decode 'data' to get 'payload'
        [payload(stack), mote_addr(stack), mote_fcnt(stack)] ← b64_to_bin(data(stack))
        # Compare metadata(stack) (stat, mote_addr, crc and size)
        if metadata(stack) all equals then
          mch ← SC(lsnr(stack), payload(stack))
          corrected_payload ← EPC(crc, mch, payload(stack), size, mote_addr, mote_fcnt)
          corrected_payload ← APC(crc, mch, payload(stack), size, mote_addr, mote_fcnt)
          corrected_payload ← SOFT(crc, mch, payload(stack), size, mote_addr, mote_fcnt)
          if corrected_payload exists then
            send(corrected_message)
          end if
        end if
      end if
    end if
end if

# Choose the payload with highest SNR as candidate
Function SC(lsnr, payload(stack))
mch ← compareSNR(lsnr, payload(stack))
return mch

# Enhanced Packet Combining Algorithm
Function EPC(crc, mch, payload(stack), size, mote_addr, mote_fcnt)
if corrected_payload not exists then
  for [a, b] in combnk(payload(stack), 2) do
    mask ← XOR(a, b)
  end if
  corrected_payload ← Correct(mask, mch, crc, size, mote_addr, mote_fcnt)
  if corrected_payload exists then
    return corrected_payload
  end if
end if

# Aggressive Packet Combining Algorithm
Function APC(crc, mch, payload(stack), size, mote_addr, mote_fcnt)
if corrected_payload not exists then
  corrected_payload ← validateCRC(crc, majorityVoting(payload(stack)), size, mote_fcnt, mote_addr)
  if corrected_payload exists then
    return corrected_payload
  else
    corrected_payload ← Correct(LeastReliableMask(payload(stack)), mch, crc, size, mote_addr, mote_fcnt)
    if corrected_payload exists then
      return corrected_payload
    end if
  end if
end if

# SOFT Algorithm
Function SOFT(crc, mch, payload, size, mote_addr, mote_fcnt)
if corrected_payload not exists then
  corrected_payload ← validateCRC(crc, softDecoding(payload(stack)), size, mote_fcnt, mote_addr)
  if corrected_payload exists then
    return corrected_payload
  endif
end if

# Brute-force search the candidate in finite time
Function Correct(mask, mch, crc, size, mote_addr, mote_fcnt)
while time_use < latency do
  while brute_force(mask) exists do
    corrected_payload ← validateCRC(crc, xor(brute_force(mask), mch), size, mote_addr, mote_fcnt)
  end while
  if corrected_payload exists then
    return corrected_payload
  end if
end while

# validate Message integrity based on CCITT crc-16 algorithm
Function validateCRC(crc, candidate, size, mote_fcnt, mote_addr)
if crc = sx1302_lora_payload_crc(candidate, size) then
    if validateMIC(candidate, mote_fcnt, size, mote_addr) = 1 then
      return candidate
    end if
end if

# validate Message integrity based on AES-CMAC algorithm
Function validateMIC(candidate, mote_fcnt, size, mote_addr)
for addr in DEVADDR(stack) do
  if mote_addr = addr then
    nwkskey ← NWKSKEY
    return aes_verifyMic(nwkskey, mote_addr, mote_fcnt, candidate, size)
  end if
end for
