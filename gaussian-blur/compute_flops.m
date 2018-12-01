% compute gflops of gaussian blur

close all
clear

%% generate test sizes file first
KERNEL_WIDTH = 16;
KERNEL_HEIGHT = 8;
filterRadii = 2:8;

i = [1 16:16:128];
imageSizes = [];
for filterRadius = filterRadii
    imageSizes = [imageSizes; [KERNEL_WIDTH*i' KERNEL_HEIGHT*i' (ones(1,length(i))*filterRadius)']];
end
dlmwrite('test-sizes.txt', imageSizes, '\t');

%% pull in results from test files and analyze them

peakThrpt = 76.8;

% data from execution runs
cycles = dlmread('optimized/cycles.txt');
naivecycles = dlmread('naive/cycles.txt');

% define constants first
imChannels = 1;

opsPerFma = 2; % multiply and add

baseClock = 2.4; % billion cycles / second
boostClock = 3.2;

% calculate number of floating point operations
imPixels = imageSizes(:,1) .* imageSizes(:,2) .* imChannels;
flOps = imPixels .* (2.*imageSizes(:,3) + 1).^2 .* opsPerFma;

% remove ones that segfaulted
naiveflops = flOps([1:9, 11:18, 20:27, 29:36, 38:45, 47:54, 56:end]);

% calculate time it took
gflops = flOps ./ (cycles ./ boostClock);
naivegflops = naiveflops ./ (naivecycles ./ boostClock);

% sort based on flops and reorder GFLOPs to match
[sflops, I] = sort(flOps);
gflops = gflops(I);

[snaiveflops, I] = sort(naiveflops);
naivegflops = naivegflops(I);

% generate indices at which to plot peak
t = linspace(sflops(1), sflops(end));
peak = ones(1, length(t))*peakThrpt;

figure(1)
semilogx(sflops, gflops)
hold on
semilogx(snaiveflops, naivegflops)
semilogx(t, peak)
title('Comparison of Baseline, High-Performance, and Theoretical Peak of Gaussian Blur')
xlabel('Number of Floating Point Operations')
ylabel('GFLOP/s')
legend('High-performance Implementation', 'Naive Implementation', 'Peak Performance')
