Input: recent 6 packets snr snr(stack), GW集合 Ω, ED集合 Φ, Intensity of network λ, 扩频因子集合[1,7] χ,
传输功率集合[1,5] β, Length of payload Lpayload

#Initialize loop
min←0
do
    lastmin←min
    for i in Φ do
        for sfi, tpi in χ, β do
             eei = getEE(sfi, tpi, Average(snr(stack)i), Lpayload)
             if eei is optimal then
                 Return
             end if
        end for
    end for
    #Calculate minimum energy efficiency
    min←ee0
    for i in Φ do
        if(eei<min) then
            min←eei
        end if
    end if
while min-lastmin > δ

Function GetEE(SF, TP, SNR, Lpayload)
Rb=SF*BW/2^SF
Ppreable=0.5*Q()
Pheader=
Ppayload=
PRRk=
Pcollision=
PRR=PRRk*Pcollision
PDR=
EE=
return EE