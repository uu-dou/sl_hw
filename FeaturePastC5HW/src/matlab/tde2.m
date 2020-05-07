close all; clear all;

fAudio = fopen ('input.raw');
xInput = fread (fAudio, inf, 'int16');
xInput = xInput'/32768;
xInputLength = length(xInput);

fAudio = fopen ('ref.raw');
xRef = fread (fAudio, inf, 'int16');
xRef = xRef'/32768;
xRefLength = length(xRef);

figure;
% subplot (2, 1, 1);
% plot(xInput);
% plot(xRef);

N = 1024;
L = 1024;
iTerator = length(xInput)/N;
Sxy = zeros(1,xRefLength);

for i=1:iTerator
    x = xInput(1+(i-1)*N : (i-1)*N+L);
    X1 = fft(x,N);
    y = xRef(1+(i-1)*N : (i-1)*N+L);
    Y1 = fft(x,N);
    
    s = X1.*conj(Y1);
    Cxy=fftshift(ifft(s));
    Sxy(1+(i-1)*N/2 : (i-1)*N/2+N/2) = s(1:N/2);
end
subplot (2, 1, 2);
plot(Sxy);



