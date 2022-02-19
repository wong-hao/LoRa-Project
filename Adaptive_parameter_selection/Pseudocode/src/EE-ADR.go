Input: Average of EDi recent 6 packets snr at GWk snr(stack)i,k, GW集合 Ω, ED集合 Φ, Intensity of network λ, 扩频因子集合[1,7] χ,
		传输功率集合[1,5] β, Length of payload (bits) Lpayload, Tinterval Packet interval time, threshold ξ
		EDi所用的扩频因子 sfi, EDi所用的传输功率 tpi, Total number of GWs N, Total number of EDs M
#Initialize loop
minEE←0
λ←M/Tinterval
#Run at least once
do
	lastminEE←minEE
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
	getMinEE(ee, minEE)
while minEE-lastminEE > ξ

Function GetEE(SF, TP, SNR(Ω), Lpayload)
Rb=SF*100k/2^SF
Ppreable(Ω)=0.5*Q()
Pheader(Ω)=
Ppayload(Ω)=
PRR(Ω)=
Pcollision=
PRR(Ω)=PRR(stack)*Pcollision
for k in Ω
	PER = PER * (1-PRRk)
end for
PDR = 1 - PER
EE=
return EE

Function getMinEE(ee(stack), minEE)
minEE←ee0
for i in Φ do
	if(eei<minEE) then
		minEE←eei
	end if
end for