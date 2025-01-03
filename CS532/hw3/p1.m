%**************************************************************************
% CS 532
% Homework 3
% Juan Carlos Dibene Simental
%**************************************************************************

clear all

imageL = double(imread('teddyL.pgm'));
imageR = double(imread('teddyR.pgm'));

groundtruthL = double(imread('disp2.pgm')) / 4;

cornersL = harris(imageL / 16, 0.04, 1000);
cornersR = harris(imageR / 16, 0.04, 1000);

disp(['Corners image L: ' num2str(sum(cornersL(:)))]);
disp(['Corners image R: ' num2str(sum(cornersR(:)))]);

distances = pairwise_distances(imageL, cornersL, imageR, cornersR);

[~, I] = sort(distances(:, 5), 'ascend');
sorted = distances(I, :);
elements = numel(I);

disp(['Total distances: ' num2str(elements)]);

results = zeros(20, 2);
n = 0;

for percentage = 0.05:0.05:1
    n = n + 1;
    count = ceil(percentage * elements);
    
    [results(n, 1), results(n, 2)] = evaluate(sorted, groundtruthL, count);
end

%% Functions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function corners = harris(image, k, threshold)
% Step 1 ******************************************************************
fx = [-1, 0, 1;
      -1, 0, 1;
      -1, 0, 1];
  
fy = fx.';

% Compute Ix, Iy
Ix = convolve_inner(image, fx);
Iy = convolve_inner(image, fy);

% Compute Ix^2, Iy^2 and Ixy using elementwise multiplication
Ix2 = Ix .* Ix;
Iy2 = Iy .* Iy;
Ixy = Ix .* Iy;

% Step 2 ******************************************************************
% Gaussian filter from http://homepages.inf.ed.ac.uk/rbf/HIPR2/gsmooth.htm
G = (1 / 273) * [1,  4,  7,  4, 1;
                 4, 16, 26, 16, 4;
                 7, 26, 41, 26, 7;
                 4, 16, 26, 16, 4;
                 1,  4,  7,  4, 1];

% Apply Gaussian smoothing
Ix2G = convolve_inner(Ix2, G);
Iy2G = convolve_inner(Iy2, G);
IxyG = convolve_inner(Ixy, G);

% Step 3 ******************************************************************
iw = size(image, 2); % Image width
ih = size(image, 1); % Image height
R  = zeros(ih, iw);  % Harris response

% Compute Harris operator response function
%week 4  56 hessian
%week 4
%Ix2G .* Iy2G - IxyG.^2 - 

for row = 1:ih
for col = 1:iw
    H = [Ix2G(row, col), IxyG(row, col);
         IxyG(row, col), Iy2G(row, col)];
     
    R(row, col) = det(H) - k * trace(H)^2;
end
end

% Threshold
R_threshold = R > threshold;

% Step 4 ******************************************************************
mask = zeros(ih, iw); % Mask for non-maximum suppression

% Offset used to ignore all pixels for which any window falls out of the
% boundaries of the image
% Computing the image derivatives "shrinks" the image by 1 pixel each side
% Applying Gaussian smoothing "shrinks" the image by 2 pixels on each side
offset = 1 + 2;

% Compute mask
for row = (1 + offset):(ih - offset)
for col = (1 + offset):(iw - offset)
    window3x3 = R(row + (-1:1), col + (-1:1));
    
    % Get the index of the maximum element of the 3x3 window
    [~, I] = max(window3x3(:));
    
    % 5 is the index of the pixel in the center of the 3x3 window
    mask(row, col) = (I == 5); 
end
end

% Apply non-maximum suppression
corners = R_threshold .* mask;
end

function distances = pairwise_distances(image1, corners1, image2, corners2)
% Step 5 ******************************************************************
% Get indices of corners
[row1, col1] = find(corners1);
[row2, col2] = find(corners2);

indices1 = [col1, row1].';
indices2 = [col2, row2].';

N = size(indices1, 2);
M = size(indices2, 2);

% Table for storing the distances, each row is [x1, y1, x2, y2, sad]
distances = zeros(N * M, 2 + 2 + 1);
index = 0;

% Compute distances
for xy1 = indices1
    win1 = image1(xy1(2) + (-1:1), xy1(1) + (-1:1)); % 3x3 window
for xy2 = indices2
    win2 = image2(xy2(2) + (-1:1), xy2(1) + (-1:1)); % 3x3 window
    
    % Compute SAD
    ad  = abs(win2 - win1);
    sad = sum(ad(:));
    
    index = index + 1;
    distances(index, :) = [xy1.', xy2.', sad];
end
end
end

function [correct, incorrect] = evaluate(sorted, groundtruth, range)
% Step 6 ******************************************************************
correct   = 0;
incorrect = 0;

for index = 1:range
    entry = sorted(index, :);
    
    x1 = entry(1);
    y1 = entry(2);
    x2 = entry(3);
    y2 = entry(4);
    
    error = abs(groundtruth(y1, x1) - sqrt((x1 - x2)^2 + (y1 - y2)^2));
    
    if (error <= 1), correct   = correct   + 1;
    else,            incorrect = incorrect + 1;
    end
end
end

function output = convolve_inner(input, kernel)
iw = size(input, 2);
ih = size(input, 1);
kw = size(kernel, 2);
kh = size(kernel, 1);

oh = fix(kh / 2);
ow = fix(kw / 2);

output = zeros(ih, iw);

for row = (1 + oh):(ih - oh)
for col = (1 + ow):(iw - ow)   
    block = input((row - oh):(row + oh), (col - ow):(col + ow)) .* kernel;
    output(row, col) = sum(block(:));
end
end
end
