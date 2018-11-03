% compute gflops of gaussian blur

close all
clear

% data from execution runs
cycles = [  2243798848.853333;
            1036075444.906667;
            557935547.733333;
            260110308.693333;
            137921918.293333;
            62697444.693333;
            38269037.226667;
            14318504.960000;
            4229240.746667
          ];

% define constants first
imageSizes = [  7680 4320;
                5120 2880;
                3840 2160;
                2560 1440;
                1920 1080;
                1280 720;
                960 540;
                640 360;
                320 180;
              ];
imChannels = 4;

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