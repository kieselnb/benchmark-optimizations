% compute gflops of gaussian blur

close all
clear

%% generate test sizes file first
KERNEL_WIDTH = 16;
KERNEL_HEIGHT = 8;
filterRadii = 2:8;

i = 1:8;
imageSizes = [];
for filterRadius = filterRadii
    imageSizes = [imageSizes; [KERNEL_WIDTH*i' KERNEL_HEIGHT*i' (ones(1,length(i))*filterRadius)']];
end
dlmwrite('test-sizes.txt', imageSizes, '\t');

%% pull in results from test files and analyze them

% for FMA, 2 units, retire two insns/cycle
% 8 floats per insn, 2 floating point ops per insn
% freq of 2.4 GHz
% peak = 2 * 2 * 8 * 2.4 (GFLOPs/sec)
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
naiveflops = flOps([1:16 18:24 26:32 34:40 42:48 51:end]);
% naiveflops = flOps([1:8, 10:16, 18:24, 27:32, 35:40 43:48 52:end]);

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
plot(sflops, gflops)
hold on; grid on;
plot(snaiveflops, naivegflops)
plot(t, peak)
title('Comparison of Baseline, High-Performance, and Theoretical Peak of Gaussian Blur')
xlabel('Number of Floating Point Operations')
ylabel('GFLOP/s')
legend('High-performance Implementation', 'Naive Implementation', 'Peak Performance')
