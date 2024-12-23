clear
clc 
close all

% Read Files Data
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


% FFT Plot
N = 10*1024;
windowFull = hamming(length(x7));
f = Fs7*(0:(N/2))/N;
freqRaw = fft(double(x5) .* windowFull, N);
freqRaw = abs(freqRaw/N);
freqRaw = freqRaw(1:N/2+1);
freqRaw(2:end-1) = 2*freqRaw(2:end-1);

f_filt = Fs7*(0:(N/2))/N;
freqFilt = fft(double(x7) .* windowFull, N);
freqFilt = abs(freqFilt/N);
freqFilt = freqFilt(1:N/2+1);
freqFilt(2:end-1) = 2*freqFilt(2:end-1);

hold on
plot(f, freqRaw)
plot(f_filt, freqFilt)
legend("Original", "Filtered")
hold off

% Time Plot
num_samples = 1 * Fs7;
x_trunc = x5(end-num_samples+1:end);
x7_trunc = x7(end-num_samples+1:end);
time_vector = (0:num_samples-1) / Fs7;

figure;
plot(time_vector, x_trunc);
hold on
plot(time_vector, x7_trunc);
xlabel('Time (s)');
ylabel('Amplitude');
title('Time');
legend("Original", "Filtered")
hold off


% Filter Design
Fs = 16000;
FilterOrder = 1;
fc = [20, 66, 220, 728, 2414, 8000];

fc1 = [fc(2) + 5];
wc1 = 2*fc1/Fs;
[b1,a1] = butter(FilterOrder,wc1);
fc2 = [-10 + fc(2) fc(3) + 10];
wc2 = 2*fc2/Fs;
[b2,a2] = butter(FilterOrder,wc2);
fc3 = [-50 + fc(3) fc(4) + 50];
wc3 = 2*fc3/Fs;
[b3,a3] = butter(FilterOrder,wc3);
fc4 = [-100 + fc(4) fc(5) + 100];
wc4 = 2*fc4/Fs;
[b4,a4] = butter(FilterOrder+1,wc4);
fc5 = [-300 + fc(5)];
wc5 = 2*fc5/Fs;
[b5,a5] = butter(FilterOrder+1,wc5,"high");


% Filter Bank Frequency Response
[h_theta1,theta1] = freqz(b1,a1, 2^15);
[h_theta2,theta2] = freqz(b2,a2, 2^15);
[h_theta3,theta3] = freqz(b3,a3, 2^15);
[h_theta4,theta4] = freqz(b4,a4, 2^15);
[h_theta5,theta5] = freqz(b5,a5, 2^15);

Fsplot = 16000;
figure;
loglog(theta1/(2*pi)*Fsplot, abs(h_theta1))
hold on
loglog(theta2/(2*pi)*Fsplot, abs(h_theta2))
loglog(theta3/(2*pi)*Fsplot, abs(h_theta3))
loglog(theta4/(2*pi)*Fsplot, abs(h_theta4))
loglog(theta5/(2*pi)*Fsplot, abs(h_theta5))
loglog(theta1/(2*pi)*Fsplot, 0.7*ones(1, length(h_theta1)))
xlim([10, Fsplot/2 + 1000])
ylim([10e-5, 2])
title("Filters Magnitude Response")
ylabel("Magnitude dB")
xlabel("Frequency Hz")
hold off

b = '{' + strjoin(string(b1), ',') + '}, {' ...
        + strjoin(string(b2), ',') + '}, {' ...
        + strjoin(string(b3), ',') + '}, {' ...
        + strjoin(string(b4), ',') + '}, {' ...
        + strjoin(string(b5), ',') + '}'

a = '{' + strjoin(string(a1), ',') + '}, {' ...
        + strjoin(string(a2), ',') + '}, {' ...
        + strjoin(string(a3), ',') + '}, {' ...
        + strjoin(string(a4), ',') + '}, {' ...
        + strjoin(string(a5), ',') + '}'