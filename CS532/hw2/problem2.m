
clear all

% load images
imL = imread('teddyL.pgm');
imR = imread('teddyR.pgm');

%% Compute rank-transformed images ignoring windows that fall outside the image

% width and height of window
rank_len = 5;

% buffers for the resulting rank transformed images
rkL = zeros(size(imL));
rkR = zeros(size(imR));

rows = size(imL, 1);
cols = size(imL, 2);

offs = fix(rank_len / 2);

% compute rank transform for left and right images
for row = (1 + offs):(rows - offs)
    by = (row - offs):(row + offs);
for col = (1 + offs):(cols - offs)
    bx = (col - offs):(col + offs);
    
    % rank transformed left image
    rtL = imL(by, bx) < imL(row, col);
    rkL(row, col) = sum(rtL(:));
    
    % rank transformed right image
    rtR = imR(by, bx) < imR(row, col);
    rkR(row, col) = sum(rtR(:));
end
end

%% Compute PKRN maps

% Disparity range is 0 to 63
d_range = 63;

% compute PKRN map for left and right images with SAD 3x3 window
pkL3 = compute_PKRN(3, rkL, rkR, -d_range);
pkR3 = compute_PKRN(3, rkR, rkL,  d_range);

% write PKRN maps
imwrite(pkL3, 'PKRN_L_3x3.pgm');
imwrite(pkR3, 'PKRN_R_3x3.pgm');

%% Generate disparity map with top 50% pixels and compare with ground truth

% ground truth for left image
% divide by 4
% matlab automatically rounds uint8 types to the nearest integer in uint8
gtL = imread('disp2.pgm') / 4;

% read disparity map for the left image
% the disparity values are scaled by 4 in the images
dmL3 = imread('dispL_3x3.pgm') / 4;

% select top 50% confidence pixels
keepL = pkL3(:) >= 0.5;

dmL3 = dmL3(:);
gtL = gtL(:);

dmL_keep = dmL3(keepL);
gtL_keep = gtL(keepL);

% compute disparity error and percentage of bad pixels (error > 1)
% for top 50%
error = abs(dmL_keep - gtL_keep);

number_kept = sum(keepL);
percentage = sum(error > 1) / number_kept * 100;

%% Write sparse disparity maps

dmR3 = imread('dispR_3x3.pgm') / 4;
dmR3 = dmR3(:);

cutL = ~keepL;
cutR = pkR3(:) < 0.5;

dmL3(cutL) = 0;
dmR3(cutR) = 0;

dmL3 = reshape(dmL3, size(imL));
dmR3 = reshape(dmR3, size(imL));

imwrite(dmL3 * 4, 'dispL_3x3_PKRN.pgm');
imwrite(dmR3 * 4, 'dispR_3x3_PKRN.pgm');

%% Functions

% compute PKRN map using SAD with a window of wnd_len * wnd_len
% d_range is the maximum disparity and is a signed value
% if d_range < 0 im2 is compared from +x to -x (right to left)
% if d_range > 0 im2 is compared from -x to +x (left to right)
function dst = compute_PKRN(wnd_len, im1, im2, d_range)
offs = fix(wnd_len / 2);

rows = size(im1, 1);
cols = size(im1, 2);

% buffer for the result
dst = zeros(size(im1));

% compute PKRN map
for row = (1 + offs):(rows - offs)
    by = (row - offs):(row + offs);
for col = (1 + offs):(cols - offs)
    bx = (col - offs):(col + offs);
    
    % read block from im1
    b1 = im1(by, bx);
    
    % find minimum cost and second smallest cost
    c1 = Inf;
    c2 = Inf;
    
    for d = 0:sign(d_range):d_range
        % index of next block from im2
        bx2 = bx + d;
        if (min(bx2) < 1 || max(bx2) > cols), break; end
        
        % read next block from im2
        b2 = im2(by, bx2);
        
        % compute SAD for both blocks
        ad = abs(b1 - b2);
        sad = sum(ad(:));
        
        % store minimum cost and second smallest cost
        if (sad < c1)
            c2 = c1;
            c1 = sad;
        elseif (sad < c2)
            c2 = sad;
        end
    end

    % compute reciprocal of PKRN (for bounding to 0,1)
    if (c2 <= 0), recp_conf = 1;
    else,         recp_conf = c1 / c2;
    end

    % write confidence (in range (0,1) 0 for lowest confidence 1 is highest)
    dst(row, col) = 1 - recp_conf;
end
end

end
