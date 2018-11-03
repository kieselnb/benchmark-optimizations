% compute gflops of gaussian blur

close all
clear

% data from execution runs
cycles = [  111832573828.000015;
            52776829108.000008;
            28934324004.000004;
            13410113864.000002;
            7277940352.000001;
            3233988332.000000;
            1797784464.000000;
            849847248.000000;
            212438680.000000
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

baseClock = 2.4; % cycles / second

% calculate number of floating point operations
filterElements = (2*filterRadius + 1)^2;
imPixels = imageSizes(:,1) .* imageSizes(:,2) .* imChannels;
flOps = imPixels .* filterElements .* opsPerFma;

% calculate time it took
GFLOPs = flOps ./ (cycles ./ baseClock);
plot(imPixels, GFLOPs)