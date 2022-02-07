Input: Max SNR of the last 20 received frames SNRm,
		spreading facotr of the end-device’s last received frame SF,
		equivalent Isotropically Radiated Power of the end-device’s last received frame Txpower,
		installation margin of the network (typically 10dB in most networks) margin_db

Output: Spreading facotr SF, equivalent isotropically radiated power Txpower

SNRmargin ← SNRm – SNR(SF) - margin_db;
Nstep ← floor(SNRmargin/3)
while Nstep != 0 do
	if Nstep > 0 then
		# Only increase data rate
		if SF > SF7 then
			SF ← SF - 1
		else
			Txpower ← Txpower - 3dB
		end if
		Nstep ← Nstep - 1
		if Txpower = min then
			Return SF, Txpower
		end if
	else
		if Txpower < max then
			Txpower ← Txpower + 3dB
			Nstep ← Nstep + 1
		else
			Return SF, Txpower
		end if
	end if
end while

return SF, Txpower