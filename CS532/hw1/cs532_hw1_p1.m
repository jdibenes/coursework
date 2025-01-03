%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CS 532
% Homework 1
% Problem 1
% Juan Carlos Dibene Simental
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all

%% Homography

% point format is [x; y]

% read source image
src_img = double(imread('basketball-court.ppm'));
src_img = src_img ./ 255;

src_width  = size(src_img, 2);
src_height = size(src_img, 1);

% selected points in source image
src_points = [
    249,  53; % top left
    403,  75; % top right
    280, 280; % bottom right
     25, 194; % bottom left
].';

% normalize points to [0, 1]
src_points_n = pixeltonormalized(src_width, src_height, src_points);

% normalized points in destination image
dst_points_n = [
    1, 0; % top right
    1, 1; % bottom right
    0, 1; % bottom left
    0, 0; % top left
].';

% convert points to homogeneous coordinates
src_hp = R2toP3(src_points_n);
dst_hp = R2toP3(dst_points_n);

% find homography H * dst_hp = src_hp
H = homography(dst_hp, src_hp);

%% New image

% create image
dst_width  = 940;
dst_height = 500;

dst_img = zeros(dst_height, dst_width, 3);

% create a matrix with all the pixel coordinates in the destination image
[Y, X] = meshgrid(1:dst_height, 1:dst_width);
dst_points_all = [X(:).'; Y(:).'];

% for each pixel in the destination image find the corresponding pixel in
% the source image 
dst_hp_all = R2toP3(pixeltonormalized(dst_width, dst_height, dst_points_all));
src_hp_all = H * dst_hp_all;

src_points_all = normalizedtopixel(src_width, src_height, P3toR2(src_hp_all));

% render image pixel per pixel
for point_pair = [dst_points_all; src_points_all]
    dst_x = point_pair(1);
    dst_y = point_pair(2);
    src_x = point_pair(3);
    src_y = point_pair(4);
    
    dst_img(dst_y, dst_x, :) = bilinear2D(src_img, src_width, src_height, src_x, src_y);
end

% save image
imwrite(dst_img, 'new-basketball.png');

%% Functions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% normalizes points in matlab pixel coordinates to [0, 1]
function points_n = pixeltonormalized(width, height, points)
points_n = (points - 1);
points_n(1, :) = points_n(1, :) / (width  - 1);
points_n(2, :) = points_n(2, :) / (height - 1);
end

% converts normalized 2D points to pixel coordinates
function points = normalizedtopixel(width, height, points_n)
points = [points_n(1, :) * (width - 1); points_n(2, :) * (height - 1)] + 1;
end

% converts from 2D to 2D homogeneous coordinates
function points_h = R2toP3(points)
points_h = [points(1:2, :); ones(1, size(points, 2))];
end

% converts from 2D homogeneous coordinates to 2D
function points = P3toR2(points_h)
points = points_h(1:2, :) ./ repmat(points_h(3, :), 2, 1);
end

% implementation of the DLT algorithm described in:
% https://cseweb.ucsd.edu/classes/wi07/cse252a/homography_estimation/homography_estimation.pdf
function H = homography(h_src_p, h_dst_p)
n_src = size(h_src_p, 2);

if (n_src ~= size(h_dst_p, 2))
    error('src and dst must have the same number of points');
end

zeros_nx3 = zeros(3, n_src);

% generate linear system of equations
% equation (11)
ax = [-h_src_p; zeros_nx3; h_src_p .* repmat(h_dst_p(1, :), 3, 1)];

% equation (12)
ay = [zeros_nx3; -h_src_p; h_src_p .* repmat(h_dst_p(2, :), 3, 1)];

% equation (14) interleave ax and ay
A = zeros(9, n_src * 2);
A(:, 1:2:end) = ax;
A(:, 2:2:end) = ay;

% equation (17) solve homogeneous system with SVD
[U, S, v] = svd(A.');

% coefficients of H are the 9th column of v because matlab sorts the
% singular values in decreasing order
H = reshape(v(:, 9), 3, 3).';
end

% implementation of the 2D bilinear interpolation algorithm described in:
% https://en.wikipedia.org/wiki/Bilinear_interpolation
function px = bilinear2D(img, width, height, x, y)
l = floor(x);
t = floor(y);
r = ceil(x);
b = ceil(y);

% return black if [x, y] is outside the image
if (l > width || t > height || r < 1 || b < 1)
    px = 0;
    return;
end

% find neighbor pixels (square 2x2)
channels = size(img, 3);
square2x2 = zeros(2, 2, channels);

w = [l r];
h = [t b];

% handle corner cases
if (l < 1), w = r; elseif (r > width ), w = l; end
if (t < 1), h = b; elseif (b > height), h = t; end
 
square2x2(h - t + 1, w - l + 1, :) = img(h, w, :);

if (l == r), r = l + 1; end
if (t == b), b = t + 1; end

% these are constant for all channels
vx = [r - x, x - l];
vy = [b - y; y - t];

% perform interpolation for each channel
px = zeros(channels, 1);
for k = 1:channels, px(k) = (vx * square2x2(:, :, k).' * vy); end
end
