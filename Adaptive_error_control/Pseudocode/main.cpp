data: Base64 encoded RF packet payload, padded
time: UTC time of pkt RX, us precision, ISO 8601 'compact' format
lsnr: Lora SNR ratio in dB (signed float, 0.1 dB precision)
stat: CRC status: 1 = OK, -1 = fail, 0 = no CRC
size: RF packet payload size in bytes (unsigned integer)
mote_fcnt: 2-octets frame counter
latency: max computation time counter for timestamping, 1 microsecond resolution


Input: SX1302 packet forwarder UDP packet message# Socket collect gateway mac address

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
          corrected_payload ← SOFT(crc, mch, payload(stack), size, mote_addr, mote_fcnt, lsnr(stack))
          if corrected_payload exists then
            send(corrected_message)
          end if
        end if
      end if
    end if
end if
