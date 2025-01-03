
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

%% Compute disparity maps

% Disparity range is 0 to 63
d_range = 63;

% compute disparity map for left and right images with SAD 3x3 window
dmL3 = compute_sad_disparity(3, rkL, rkR, -d_range);
dmR3 = compute_sad_disparity(3, rkR, rkL,  d_range);

% compute disparity map for left and right images with SAD 3x3 window
dmL15 = compute_sad_disparity(15, rkL, rkR, -d_range);
dmR15 = compute_sad_disparity(15, rkR, rkL,  d_range);

% write disparity maps (scale by 4 for visibility)
imwrite(uint8(dmL3 * 4), 'dispL_3x3.pgm');
imwrite(uint8(dmR3 * 4), 'dispR_3x3.pgm');

imwrite(uint8(dmL15 * 4), 'dispL_15x15.pgm');
imwrite(uint8(dmR15 * 4), 'dispR_15x15.pgm');

%% Functions

% compute disparity map using SAD with a window of wnd_len * wnd_len
% d_range is the maximum disparity and is a signed value
% if d_range < 0 im2 is compared from +x to -x (right to left)
% if d_range > 0 im2 is compared from -x to +x (left to right)
function dst = compute_sad_disparity(wnd_len, im1, im2, d_range)
offs = fix(wnd_len / 2);

rows = size(im1, 1);
cols = size(im1, 2);

% buffer for the result
dst = zeros(size(im1));

% compute disparity map
for row = (1 + offs):(rows - offs)
    by = (row - offs):(row + offs);
for col = (1 + offs):(cols - offs)
    bx = (col - offs):(col + offs);
    
    % read block from im1
    b1 = im1(by, bx);
    
    % find minimum cost disparity
    c1 = Inf;
    d1 = -1;
    
    for d = 0:sign(d_range):d_range
        % index of next block from im2
        bx2 = bx + d;
        if (min(bx2) < 1 || max(bx2) > cols), break; end
        
        % read next block from im2
        b2 = im2(by, bx2);
        
        % compute SAD for both blocks
        ad = abs(b1 - b2);
        sad = sum(ad(:));
        
        % store minimum cost and corresponding d
        if (sad < c1)
            c1 = sad;
            d1 = abs(d);
        end
    end
    
    % write disparity
    dst(row, col) = d1;
end
end

end
