% compute gflops of gaussian blur

close all
clear

%% generate test sizes file first
KERNEL_WIDTH = 8;
KERNEL_HEIGHT = 8;

i = [1 16:16:512];
imageSizes = [KERNEL_WIDTH*i' KERNEL_HEIGHT*i'];
dlmwrite('test-sizes.txt', imageSizes, 'x');

%% pull in results from test files and analyze them

% data from execution runs
cycles = dlmread('optimized/cycles.txt');

% define constants first
imChannels = 1;

filterRadius = 2;

opsPerFma = 2; % multiply and add

baseClock = 2.4; % billion cycles / second
boostClock = 3.2;

% calculate number of floating point operations
filterElements = (2*filterRadius + 1)^2;
imPixels = imageSizes(:,1) .* imageSizes(:,2) .* imChannels;
flOps = imPixels .* filterElements .* opsPerFma;

% calculate time it took
GFLOPs = flOps ./ (cycles ./ boostClock)

% plot(imPixels, GFLOPs)