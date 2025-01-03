%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CS 532
% Homework 1
% Problem 2
% Juan Carlos Dibene Simental
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all

%% dolly zoom

% load data
load('data.mat');

% point cloud
data3DC = {BackgroundPointCloudRGB,ForegroundPointCloudRGB};

% rotation of the camera (constant)
R = eye(3);

% position where the bounding box of the foreground object is about 400x600
start = 5 * [0; 0; -0.25];

% find z-distance from the camera to some point of the foreground object
f_reference = ForegroundPointCloudRGB(3, 1);
d_reference = start(3) + f_reference;

%% generate video

% camera z-displacement per frame
dz = 0.025;

% video is 15 fps * 5 seconds
frame_rate = 15;
frames = frame_rate * 5;

images = cell(1, frames);

for step = 1:frames
    disp(['Rendering frame ' num2str(step) ' / ' num2str(frames)]);
    
    % move camera
    t = start + [0; 0; dz] * step;
    
    % calculate focal length scaling factor by similar triangles
    scale = (t(3) + f_reference) / d_reference;
    
    S = [scale,     0, 0;
             0, scale, 0;
             0,     0, 1];
         
    % new projection matrix
    P = K * S * [R, t];
    
    % render image
    images{1, step} = PointCloud2Image(P, data3DC, crop_region, filter_size);
end

% write to video
vw = VideoWriter('dolly.avi');
vw.FrameRate = frame_rate;

open(vw);
for k = 1:frames, writeVideo(vw, images{1, k}); end
close(vw);
