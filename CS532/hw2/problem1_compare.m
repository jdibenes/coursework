
clear all

%% Load images

% ground truth for left image
% divide by 4
% matlab automatically rounds uint8 types to the nearest integer in uint8
gtL = imread('disp2.pgm') / 4;

% read disparity maps for the left image
% the disparity values are scaled by 4 in the images

% disparity map computed with a window of 3x3
dL_3x3 = imread('dispL_3x3.pgm') / 4;

% disparity map computed with a window of 15x15
dL_15x15 = imread('dispL_15x15.pgm') / 4;

%% Compare with ground truth

% compute percentage of bad pixels (disparity error > 1)
% ignore pixels of windows that fall outside the image

p3x3 = compare_disparity_goundtruth_d1(gtL, dL_3x3, 3);

p15x15 = compare_disparity_goundtruth_d1(gtL, dL_15x15, 15);

%% Functions

% compute percentage of bad pixels with disparity error > 1
% ignore pixels of windows that fall outside the image
% window size is wnd_len * wnd_len
% gt is the groud truth
% im is the disparity map
function percentage = compare_disparity_goundtruth_d1(gt, im, wnd_len)
offs = fix(wnd_len / 2);

rows = size(gt, 1);
cols = size(gt, 2);

% compare only for windows inside the image
by = (1 + offs):(rows - offs);
bx = (1 + offs):(cols - offs);

bgt = gt(by, bx);
bim = im(by, bx);

% compute absolute disparity error
diff = abs(bgt - bim);

% count pixels with disparity error > 1
count = sum(diff(:) > 1);

% compute percentage count / total pixels in block (not in image)
percentage = count / (numel(by) * numel(bx)) * 100;
end
