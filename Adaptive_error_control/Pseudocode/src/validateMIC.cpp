Input: NWKSKEY: network session key specific for the end-devices; DEVADDR: device addresses

# validate Message integrity based on AES-CMAC algorithm
Function validateMIC(candidate, mote_fcnt, size, mote_addr)
for addr in DEVADDR(stack) do
  if mote_addr = addr then
    nwkskey ← NWKSKEY
    Return aes_verifyMic(nwkskey, mote_addr, mote_fcnt, candidate, size)
  end if
end for