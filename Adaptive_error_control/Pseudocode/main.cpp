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
[NWKSKEY, DEVADDR] <- Activation By Personalization

while (true) do
    select(max_fd + 1, &ser_fdset, NULL, NULL, &mytime);
    for i in CLI_NUM do
        if client_fds[i] != 0 then
            read(client_fds[i], message)
            [MAC_address] <- collect(message)
            if MAC_address in predefined_Gateway_address then
                [data, time] <- collect(message)
                if len(data) = Num(Gateway) then
                  if time all equals then
                    [stat, crc, lsnr, size] <- collect(message)
                    [payload, mote_addr, mote_fcnt] <- decode(data)
                    if stat = -1 and mote_addr all equals and crc all equals then
                      mch <- compareSNR(lsnr, payload)
                      corrected_payload <- EPC(crc, mch, payload, size, mote_addr, mote_fcnt)
                      corrected_payload <- APC(crc, mch, payload, size, mote_addr, mote_fcnt)
                      corrected_payload <- SOFT(crc, mch, payload, size, mote_addr, mote_fcnt)
                      if corrected_payload exists then
                        send(socket, corrected_packet)
                      end if
                    end if
                  end if
                end if
            end if
        end if
    end for
end while

Function EPC(crc, mch, payload, size, mote_addr, mote_fcnt)
if corrected_payload not exists then
  for [a, b] in combnk(payload, 2) do
    mask <- xor(a, b)
  end if
  corrected_payload <- correct(mask, mch, crc, size, mote_addr, mote_fcnt)
  if corrected_payload exists then
    return corrected_payload
  end if
end if

Function APC(crc, mch, payload, size, mote_addr, mote_fcnt)
if corrected_payload not exists  then
  corrected_payload <- validateCRC(crc, majorityVoting(payload), size, mote_fcnt, mote_addr)
  if corrected_payload exists then
    return corrected_payload
  else
    corrected_payload <- correct(LeastReliableMask(payload), mch, crc, size, mote_addr, mote_fcnt)
    if corrected_payload exists then
      return corrected_payload
    end if
  end if
end if

Function SOFT(crc, mch, payload, size, mote_addr, mote_fcnt)
if corrected_payload not exists then
  corrected_payload <- validateCRC(crc, softDecoding(payload), size, mote_fcnt, mote_addr)
  if corrected_payload exists then
    return corrected_payload
  endif
end if

Function correct(mask, mch, crc, size, mote_addr, mote_fcnt)
while time_use < latency do
  ec <- brute_force(mask)
  while ec exists do
    corrected_payload <- validateCRC(crc, xor(ec, mch), size, mote_addr, mote_fcnt)
  end while
  if corrected_payload exists then
    return corrected_payload
  end if
end while

Function validateCRC(crc, candidate, size, mote_fcnt, mote_addr)
if crc = sx1302_lora_payload_crc(candidate, size) then
    if validateMIC(candidate, mote_fcnt, size, mote_addr) = 1 then
      return candidate
    end if
end if

Function validateMIC(candidate, mote_fcnt, size, mote_addr)
for addr in DEVADDR do
  if mote_addr = addr then
    nwkskey <- corresponding_NWKSKEY
    return aes_verifyMic(nwkskey, mote_addr, mote_fcnt, candidate, size)
  end if
end for
