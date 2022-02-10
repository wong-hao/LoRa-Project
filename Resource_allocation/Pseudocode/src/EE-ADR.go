Input: Average of EDi recent 6 packets snr at GWk snr(stack)i,k, GW集合 Ω, ED集合 Φ, Intensity of network λ, 扩频因子集合[1,7] χ,
		传输功率集合[1,5] β, Length of payload (bits) Lpayload, Tinterval Packet interval time, threshold ξ
		EDi所用的扩频因子 sfi, EDi所用的传输功率 tpi, Total number of GWs N, Total number of EDs M
#Initialize loop
min←0
λ←M/Tinterval
#Run at least once
do
	lastmin←min
	#Calculate energy efficiency of EDi
	for i in Φ do
		for sfi, tpi in χ, β do
			eei = getEE(sfi, tpi, Average(snr(stack)i,k), Lpayload)
			if eei is optimal then
				#Assign the parameter combination
				Return
			end if
		end for
	end for
	#Calculate minimal energy efficiency
	min←ee0
	for i in Φ do
		if(eei<min) then
			min←eei
		end if
	end for
while min-lastmin > ξ

Function GetEE(SF, TP, SNR(stack), Lpayload)
Rb=SF*BW/2^SF
Ppreable(stack)=0.5*Q()
Pheader(stack)=
Ppayload(stack)=
PRR(stack)=
Pcollision=
PRR(stack)=PRR(stack)*Pcollision
for k in stack
PDR = PDR * PRRk
end for
EE=
return EE