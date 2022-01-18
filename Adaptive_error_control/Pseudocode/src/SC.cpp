Algorithm 2 Selection Combining Algorithm
Function SC(lsnr(stack), payload(stack))
  # Choose the payload with highest SNR as candidate
  mch ← compareSNR(lsnr(stack), payload(stack))
Return mch
