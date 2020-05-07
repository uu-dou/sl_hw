clc;clear all;close all;
fs = 1000;T = 1/fs;N = 1000;% 采样率、时间间隔、采样点数
t = (0:N-1)*T;
f0 = 10;                    % 信号频率
delaynum = 20;              % 延时点数
x = sin(2*pi*f0*t);
y1 = x(1:length(x)-100);
y2 = x(delaynum+1:length(x)-100 + delaynum); %延时，截取相同长度信号


fAudio = fopen ('input.raw');
xAudio = fread (fAudio, inf, 'int16');
xAudio = xAudio'/32768;
xAudioLength = length(xAudio);

fAudio = fopen ('ref.raw');
rAudio = fread (fAudio, inf, 'int16');
rAudio = rAudio'/32768;
rAudioLength = length(rAudio);

x1 = xAudio;
x2 = rAudio;
N = length(x2);
time = -N+1:N-1;

% 传统时域法
% xcorrTime = zeros(2*N-1,1);
% m = 0;
% for i = -(N-1):N-1
%     m = m+1;
%     for t = 1:N
%         if 0<(i+t)&&(i+t)<=N
%             xcorrTime(m) = xcorrTime(m) + x2(t)*x1(t+i);
%         end 
%     end
% end
% xcorrTime = xcorrTime'/N;

% 频域法
Nfft = length(x1)+length(x2)-1;
xcorrfft = fftshift(ifft(fft(x1,Nfft).*conj(fft(x2,Nfft))));

% Matlab调用xcorr计算
xcorrMat = xcorr(x1,x2,'biased');
[k,ind] = max(xcorrMat);

figure;
subplot (2, 1, 1);
plot(time, xcorrfft);
subplot (2, 1, 2);
plot(time, xcorrMat);