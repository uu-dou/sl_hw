close all; clear all;
L = 16384;
fAudio = fopen ('input.raw');
xInput = fread (fAudio, inf, 'int16');
xInput = xInput'/32768;
xInput = xInput(1:L);
xInputLength = length(xInput);

fAudio = fopen ('ref.raw');
xRef = fread (fAudio, inf, 'int16');
xRef = xRef'/32768;
xRef = xRef(1:L);
xRefLength = length(xRef);

figure;

block_length = L;
shift = L;

iTerator = fix((length(xInput)-shift)/shift)+1;
cc = zeros(1,L);
window = hamming(block_length);

for i=1:iTerator
    k  = (i - 1) * shift + 1;
    near = xInput(k:(k + block_length -1));% .* window';
    Xnear = fft(near,1 * block_length);
    
    far = xRef(k:(k + block_length -1));% .* window';
    Xfar = fft(far, 1 * block_length);
    
    R = Xnear.*conj(Xfar);
    Rifft = ifft(R);
    c = fftshift(Rifft);
    cc(k:(k + block_length/2 -1)) = c(block_length/2 : block_length-1);
end
subplot (3, 1, 1);
plot(Rifft);
subplot (3, 1, 2);
plot(c);
subplot (3, 1, 3);
plot(cc);
[k,ind]=max(cc)




