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







latency <- RECEIVE_DELAY1 - TOA
[NWKSKEY(set), DEVADDR(set)] <- Activation By Personalization

# Socket collect gateway mac address
MAC_address <- read(message)
# Match collected gateway mac address with predefined one
if MAC_address in predefined_gateway_address then
    # Parse JSON struct to get metadata
    [data(set), time(set)] <- json.Unmarshal(message)
    # Enough data collected
    if len(data(set)) = len(predefined_gateway_address) then
      # Compare metadata(set)1 (time)
      if metadata(set)1 all equals then
        # Parse JSON struct to get metadata
        [stat(set), crc(set), lsnr(set), size(set)] <- json.Unmarshal(message)
        # Base64 decode 'data' to get 'payload'
        [payload(set), mote_addr(set), mote_fcnt(set)] <- b64_to_bin(data(set))
        # Compare metadata(set)2 (stat, mote_addr, crc and size)
        if metadata(set)2 all equals then
          mch <- SC(lsnr(set), payload(set))
          corrected_payload <- EPC(crc, mch, payload(set), size, mote_addr, mote_fcnt)
          corrected_payload <- APC(crc, mch, payload(set), size, mote_addr, mote_fcnt)
          corrected_payload <- SOFT(crc, mch, payload(set), size, mote_addr, mote_fcnt)
          if corrected_payload exists then
            send(corrected_message)
          end if
        end if
      end if
    end if
end if

# Choose the highest SNR one as candidate
Function SC(lsnr, payload(set))
mch <- compareSNR(lsnr, payload(set))
return mch

# Enhanced packet combining algorithm
Function EPC(crc, mch, payload(set), size, mote_addr, mote_fcnt)
if corrected_payload not exists then
  for [a, b] in combnk(payload(set), 2) do
    mask <- xor(a, b)
  end if
  corrected_payload <- correct(mask, mch, crc, size, mote_addr, mote_fcnt)
  if corrected_payload exists then
    return corrected_payload
  end if
end if

# Aggressive packet combining algorithm
Function APC(crc, mch, payload, size, mote_addr, mote_fcnt)
if corrected_payload not exists  then
  corrected_payload <- validateCRC(crc, majorityVoting(payload(set)), size, mote_fcnt, mote_addr)
  if corrected_payload exists then
    return corrected_payload
  else
    corrected_payload <- correct(LeastReliableMask(payload(set)), mch, crc, size, mote_addr, mote_fcnt)
    if corrected_payload exists then
      return corrected_payload
    end if
  end if
end if

# SOFT Algorithm
Function SOFT(crc, mch, payload, size, mote_addr, mote_fcnt)
if corrected_payload not exists then
  corrected_payload <- validateCRC(crc, softDecoding(payload(set)), size, mote_fcnt, mote_addr)
  if corrected_payload exists then
    return corrected_payload
  endif
end if

# Brute-force search the candidate in finite time
Function correct(mask, mch, crc, size, mote_addr, mote_fcnt)
while time_use < latency do
  while brute_force(mask) exists do
    corrected_payload <- validateCRC(crc, xor(brute_force(mask), mch), size, mote_addr, mote_fcnt)
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
for addr in DEVADDR(set) do
  if mote_addr = addr then
    nwkskey <- NWKSKEY
    return aes_verifyMic(nwkskey, mote_addr, mote_fcnt, candidate, size)
  end if
end for
