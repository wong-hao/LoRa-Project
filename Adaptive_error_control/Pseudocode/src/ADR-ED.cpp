Input: Uplink counter ADR_ACK_CNT,
        spreading facotr of the end-device’s last received frame SF
Output: Spreading facotr SF

while new uplink do
  if downlink response not exists then
    ADR_ACK_CNT ← ADR_ACK_CNT + 1
    if ADR_ACK_CNT = ADR_ACK_LIMIT then
      sets the ADR acknowledgment request bit (ADRACKReq)
    end if
    if ADR_ACK_CNT = ADR_ACK_LIMIT + ADR_ACK_DELAY then
      # Only decrease data rate and not change Txpower
      if SF < SF12 then
        SF ← SF + 1
      else
        resets the ADR acknowledgment request bit (ADRACKReq)
      end if
      Return SF
    end if
  else
    ADR_ACK_CNT ← 0
  end if
end while