%**************************************************************************
% CS 532
% Homework 4
% Juan Carlos Dibene Simental
%**************************************************************************

clear all

% Projection matrices *****************************************************
% Code from the script included in the homework zip file
allP = [ 776.649963 -298.408539  -32.048386  993.1581875  132.852554  120.885834 -759.210876 1982.174000  0.744869  0.662592 -0.078377 4.629312012;
         431.503540  586.251892 -137.094040 1982.053375    23.799522    1.964373 -657.832764 1725.253500 -0.321776  0.869462 -0.374826 5.538025391;
        -153.607925  722.067139 -127.204468 2182.4950     141.564346   74.195686 -637.070984 1551.185125 -0.769772  0.354474 -0.530847 4.737782227;
        -823.909119   55.557896  -82.577644 2498.20825    -31.429972   42.725830 -777.534546 2083.363250 -0.484634 -0.807611 -0.335998 4.934550781;
        -715.434998 -351.073730 -147.460815 1978.534875    29.429260   -2.156084 -779.121704 2028.892750  0.030776 -0.941587 -0.335361 4.141203125;
        -417.221649 -700.318726  -27.361042 1599.565000   111.925537 -169.101776 -752.020142 1982.983750  0.542421 -0.837170 -0.070180 3.929336426;
          94.934860 -668.213623 -331.895508  769.8633125 -549.403137  -58.174614 -342.555359 1286.971000  0.196630 -0.136065 -0.970991 3.574729736;
         452.159027 -658.943909 -279.703522  883.495000  -262.442566    1.231108 -751.532349 1884.149625  0.776201  0.215114 -0.592653 4.235517090];

P = zeros(3,4,8);

for m = 1:8
for n = 1:3
    P(n, 1:4, m) = allP(m, 4 * (n-1) + 1:4 * (n-1) + 4);
end
end

% Position of the cameras *************************************************
t = zeros(3, 8);

% Find t from P = K * R * [eye(3), -t] = [KR, -KRT]
% t = KR \ -KRT = -KR \ KRT
for camera = 1:8
    Pc = P(:, :, camera);
    t(:, camera) = -Pc(:, 1:3) \ Pc(:, end);
end

% Load color images *******************************************************
% width and height of the images
width  = 780;
height = 582;

im = zeros(height, width, 3, 8);

im(:, :, :, 1) = imread('cam00_00023_0000008550.png');
im(:, :, :, 2) = imread('cam01_00023_0000008550.png');
im(:, :, :, 3) = imread('cam02_00023_0000008550.png');
im(:, :, :, 4) = imread('cam03_00023_0000008550.png');
im(:, :, :, 5) = imread('cam04_00023_0000008550.png');
im(:, :, :, 6) = imread('cam05_00023_0000008550.png');
im(:, :, :, 7) = imread('cam06_00023_0000008550.png');
im(:, :, :, 8) = imread('cam07_00023_0000008550.png');

% Load silhouettes ********************************************************
sl = false(height, width, 8);

sl(:, :, 1) = imread('silh_cam00_00023_0000008550.pbm');
sl(:, :, 2) = imread('silh_cam01_00023_0000008550.pbm');
sl(:, :, 3) = imread('silh_cam02_00023_0000008550.pbm');
sl(:, :, 4) = imread('silh_cam03_00023_0000008550.pbm');
sl(:, :, 5) = imread('silh_cam04_00023_0000008550.pbm');
sl(:, :, 6) = imread('silh_cam05_00023_0000008550.pbm');
sl(:, :, 7) = imread('silh_cam06_00023_0000008550.pbm');
sl(:, :, 8) = imread('silh_cam07_00023_0000008550.pbm');

% Part 1 ******************************************************************
% Create voxel grid
step = 0.01;

step_x = step;
step_y = step;
step_z = step;

X = -2.5:step_x:2.5;
Y = -3.0:step_y:3.0;
Z =  0.0:step_z:2.5;

Xn = numel(X);
Yn = numel(Y);
Zn = numel(Z);

grid = false(Xn, Yn, Zn);

% Represent each voxel as a vertex centered in the voxel
vertices = zeros(3, Xn * Yn * Zn); % list of occupied voxels
index = 0;

% Estimate which voxels are occupied and which are free space
    gx = 0;
for x = X
    gx = gx + 1;
    gy = 0;
for y = Y
    gy = gy + 1;
    gz = 0;
for z = Z
    gz = gz + 1;
    
    vertex4 = [x; y; z; 1];
    
for camera = 1:8
    [u, v] = project_point(P(:, :, camera), vertex4);

    % A voxel is occupied if the projection of its vertex is inside the
    % silhouette in all images
    occupied = u >= 0 && u < width  && ...
               v >= 0 && v < height && ...
               sl(v + 1, u + 1, camera);

    if (~occupied), break; end
end    
    if (~occupied), continue; end
    
    % If the voxel is occupied add its vertex to the vertices list and mark
    % the voxel in the grid
    index              = index + 1;
    vertices(:, index) = vertex4(1:3);
    grid(gx, gy, gz)   = true;
end
end
    disp(['Completed x: ' num2str(x) ' / ' num2str(X(end))]);
end

% Trim list, index contains the number of vertices in the list
vertices = vertices(:, 1:index);

% Part 2 ******************************************************************
keep  = true(index, 1); % mask to select which vertices to keep
index = 0;

% Identify surface points
% A surface point is (the vertex of) a voxel that is not completely
% surrounded by occupied voxels
% A voxel is completely surrounded if all the voxels in its 3x3x3
% neighborhood are occupied
for vertex = vertices
    % Get the grid coordinates of the voxel from its vertex
    gx = find(X == vertex(1), 1);
    gy = find(Y == vertex(2), 1);
    gz = find(Z == vertex(3), 1);
    
    index = index + 1;
    
    % Check if the voxel is completely surrounded (3x3x3 = 27)
    keep(index) = gx <=  1 || gy <=  1 || gz <=  1 || ...
                  gx >= Xn || gy >= Yn || gz >= Zn || ...
                  block_sum(grid(gx+(-1:1), gy+(-1:1), gz+(-1:1))) < 27;
end

% Keep only the surface points
surface_vertices = vertices(:, keep);
surface_elements = size(surface_vertices, 2);

%colors = repmat(255 * [1; 0; 1], 1, surface_elements);
%write_ply('model_1color.ply', surface_vertices, colors);

% Part 3 ******************************************************************
colors = zeros(3, surface_elements);
index  = 0;

% Assign a color to each surface point using the color images
for vertex = surface_vertices
    index = index + 1;
    
    % Compute the normal of the point
    normal = [0; 0; 0];
    
    gx = find(X == vertex(1), 1);
    gy = find(Y == vertex(2), 1);
    gz = find(Z == vertex(3), 1);
    
for dx = -1:1
for dy = -1:1
for dz = -1:1
    ix = gx + dx;
    iy = gy + dy;
    iz = gz + dz;

    % The normal points towards the unoccupied neighboring voxels (mean)
    % For completely surrounded voxels the normal would be 0 but those are
    % not included in surface_vertices
    occupied = ix >  1 && iy >  1 && iz >  1 && ...
               ix < Xn && iy < Yn && iz < Zn && ...
               grid(ix, iy, iz);

    normal = normal + (~occupied) * [dx; dy; dz];
end
end
end
    normal = normal / norm(normal);
    
    % Assign a color to the point using the normal (from a single image)
    vertex4 = [vertex; 1];
    max_o   = -Inf;
    
for camera = 1:8
    % Compute the ray from the point to the optical center of the camera
    ray = t(:, camera) - vertex;
    ray = ray / norm(ray);

    % Find the camera that maximizes the dot product of the normal and the
    % ray
    % The maximum possible value is 1 and in this case it is highly likely
    % that the camera observes the point (not occluded)
    % The minimum possible value is -1 and in this case the camera
    % "observes" a back-face so it is highly likely that the camera does
    % not observe the point (it is occluded)
    cos_o = dot(ray, normal);
    if (cos_o <= max_o), continue; end

    % Read the corresponding pixel from the image of the camera that
    % maximizes the dot product and assign that as the color of the point
    max_o = cos_o;
    [u, v] = project_point(P(:, :, camera), vertex4);
    colors(:, index) = im(v + 1, u + 1, :, camera);
end
end

write_ply('model_fullcolor.ply', surface_vertices, colors);

%% Functions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Computes P*v where P is a camera projection matrix and v = [x, y, z, 1]
% The result is rounded to integer pixel coordinates
function [u, v] = project_point(P, vertex4)
p = P * vertex4;
p = round(p(1:2) / p(3));
u = p(1);
v = p(2);
end

% Sums the all the elements of the input matrix
function s = block_sum(block)
s = sum(block(:));
end

% Plots points in 3D
function draw_points(vertices, colors)
scatter3(vertices(1, :), vertices(2, :), vertices(3, :), 1, colors, '.');
end

% Write ply file
function write_ply(filename, vertices, colors)
f = fopen(filename, 'wt');
fprintf(f, 'ply\n');
fprintf(f, 'format ascii 1.0\n');
fprintf(f, 'element vertex %d\n', size(vertices, 2));
fprintf(f, 'property float x\n');
fprintf(f, 'property float y\n');
fprintf(f, 'property float z\n');
fprintf(f, 'property uchar red\n');
fprintf(f, 'property uchar green\n');
fprintf(f, 'property uchar blue\n');
fprintf(f, 'element face 0\n');
fprintf(f, 'end_header\n');

data = [vertices; colors];

for d = data
    fprintf(f, '%f %f %f %d %d %d\n', d(1), d(2), d(3), d(4), d(5), d(6));
end

fclose(f);
end
