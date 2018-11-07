% compute gflops of gaussian blur

close all
clear

%% generate test sizes file first
KERNEL_WIDTH = 8;
KERNEL_HEIGHT = 7;
filterRadii = 2:8;

i = [1 16:16:128];
imageSizes = [];
for filterRadius = filterRadii
    imageSizes = [imageSizes; [KERNEL_WIDTH*i' KERNEL_HEIGHT*i' (ones(1,length(i))*filterRadius)']];
end
dlmwrite('test-sizes.txt', imageSizes, '\t');

%% pull in results from test files and analyze them

% data from execution runs
cycles = dlmread('optimized/cycles.txt');

% define constants first
imChannels = 1;

opsPerFma = 2; % multiply and add

baseClock = 2.4; % billion cycles / second
boostClock = 3.2;

% calculate number of floating point operations
imPixels = imageSizes(:,1) .* imageSizes(:,2) .* imChannels;
flOps = imPixels .* (2.*imageSizes(:,3) + 1).^2 .* opsPerFma;

% calculate time it took
GFLOPs = flOps ./ (cycles ./ boostClock)

% plot(imPixels, GFLOPs)