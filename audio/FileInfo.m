clear
clc 
close all


filename1 = 'royalty_44.1k_16bit_stereo.wav';        
[x1,Fs1] = audioread(filename1, "native");

filename2 = 'royalty_44.1k_16bit_mono.wav';        
[x2,Fs2] = audioread(filename2, "native");

filename3 = 'royalty_16k_16bit_stereo.wav';        
[x3,Fs3] = audioread(filename3, "native");

filename4 = 'royalty_16k_16bit_mono.wav';        
[x4,Fs4] = audioread(filename4, "native");

filename5 = 'super_shy_44.1k_16bit_stereo.wav';        
[x5,Fs5] = audioread(filename5, "native");

filename6 = 'super_shy_16k_16bit_stereo.wav';        
[x6,Fs6] = audioread(filename6, "native");

filename7 = 'output.wav';        
[x7,Fs7] = audioread(filename7, "native");

info = audioinfo(filename7);

% FFT
N = 10*1024;
f = Fs1*(0:(N/2))/N;
freqRaw = fft(x1, N);
freqRaw = abs(freqRaw/N);
freqRaw = freqRaw(1:N/2+1);
freqRaw(2:end-1) = 2*freqRaw(2:end-1);

f_filt = Fs7*(0:(N/2))/N;
freqFilt = fft(x7, N);
freqFilt = abs(freqFilt/N);
freqFilt = freqFilt(1:N/2+1);
freqFilt(2:end-1) = 2*freqFilt(2:end-1);

hold on
plot(f, freqRaw)
plot(f_filt, freqFilt)
legend("Original", "Filtered")
hold off