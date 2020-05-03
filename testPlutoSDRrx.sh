#!/bin/bash

FREQ="105200000"      # UKW/FM at 105.2 MHz
RFBW="300000"         # RF filter bandwidth 300 kHz
RXAGC="slow_attack"   # Gain Control Mode: manual/fast_attack/slow_attack/hybrid
RXGAIN="71"           # gain in dB for 'manual'
DEFAULT_FS="2112000"  # default samplerate

echo "usage: $0 <IP> <Samplerate>"

echo ""
echo "using internal settings from top of $0 :"
echo "  FREQ $FREQ"
echo "  RFBW $RFBW"
echo "  RXAGC $RXAGC"
echo "  RXGAIN $RXGAIN"
echo "  DEFAULT_FS $DEFAULT_FS"
echo ""

IP="$1"
if [ -z "$IP" ]; then
  IP="192.168.2.1"
  echo "using default IP $IP"
fi

FS="$2"
if [ -z "$FS" ]; then
  FS="$DEFAULT_FS"
  echo "using default samplerate FS $FS Hz"
fi

# set buffer to 50 ms
BUFSIZ=$[ $FS / 20 ]


# iio_info -u ip:$IP
# iio_attr -u ip:$IP -d
# iio_attr -u ip:$IP -c ad9361-phy .
# iio_attr -u ip:$IP -d ad9361-phy .
# iio_attr -u ip:$IP -d ad9361-phy rx_path_rates
# iio_attr -u ip:$IP -d ad9361-phy filter_fir_config
# iio_attr -u ip:$IP -c cf-ad9361-lpc .
# iio_attr -u ip:$IP -c xadc .

echo -e "\nsetting sampling_frequency of ad9361-phy .."
iio_attr -u ip:$IP -i -c ad9361-phy voltage0 sampling_frequency $FS
iio_attr -u ip:$IP -i -c ad9361-phy voltage2 sampling_frequency $FS

echo -e "\nsetting frequency .."
iio_attr -u ip:$IP -c ad9361-phy RX_LO frequency $FREQ

echo -e "\nsetting RF input bandwidth .."
iio_attr -u ip:$IP -i -c ad9361-phy voltage0 rf_bandwidth $RFBW
iio_attr -u ip:$IP -i -c ad9361-phy voltage2 rf_bandwidth $RFBW

# iio_attr -u ip:$IP -c ad9361-phy . |grep hardwaregain
# dev 'ad9361-phy', channel 'voltage0' (input), attr 'hardwaregain', value '73.000000 dB'
# dev 'ad9361-phy', channel 'voltage0' (input), attr 'hardwaregain_available', value '[-1 1 73]'
# dev 'ad9361-phy', channel 'voltage0' (output), attr 'hardwaregain', value '-20.000000 dB'
# dev 'ad9361-phy', channel 'voltage0' (output), attr 'hardwaregain_available', value '[-89.750000 0.250000 0.000000]'

echo -e "\nsetting RX Gain Control Mode to $RXAGC .."
iio_attr -u ip:$IP -c ad9361-phy voltage0 gain_control_mode $RXAGC

# im Folgenden Einlesen mit versch. RX Gain Werten: 0, +30 und +60 dB
if [ "$RXAGC" = "manual" ]; then
echo -e "\nsetting RX Gain to $RXGAIN .."
  iio_attr -u ip:$IP -i -c ad9361-phy voltage0 hardwaregain $RXGAIN
fi

echo -e "\nsampling_frequency and path:"
iio_attr -u ip:$IP -i -c ad9361-phy . sampling_frequency
iio_attr -u ip:$IP -d ad9361-phy rx_path_rates

echo -e "\nsampling_frequency stream:"
iio_attr -u ip:$IP -c cf-ad9361-lpc . sampling_frequency
iio_attr -u ip:$IP -c cf-ad9361-lpc . sampling_frequency_available

echo -e "\nstarting record (Ctrl+C to abort) .."
RAW2WAV=$(which rtl_raw2wav)
if [ -z "$RAW2WAV" ]; then
  time iio_readdev -u ip:$IP -b $BUFSIZ  cf-ad9361-lpc  >samples.bin
else
  time iio_readdev -u ip:$IP -b $BUFSIZ  cf-ad9361-lpc \
  | rtl_raw2wav -w samples.wav -f $FREQ -s $FS -c 2 -b 16
fi
echo "done."
