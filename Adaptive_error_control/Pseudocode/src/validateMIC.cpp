Input: Network session key specific for the end-devices NWKSKEY;

Function validateMIC(candidate, mote_fcnt, size, mote_addr)
# Validate Message integrity based on AES-CMAC algorithm
for addr in predefined_mote_address(stack) do
    if mote_addr = addr then
        nwkskey ← NWKSKEY
        Return aes_verifyMic(nwkskey, mote_addr, mote_fcnt, candidate, size)
    end if
end for