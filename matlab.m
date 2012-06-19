clc, clear, close all;

%% Generates sparse asymmetric M,b
%% such that "Mx = b"
% rng(0);
% M = sprand(10,5,3)
% assert(sprank(M)==5);
% x = rand(5,1)
% b = M*x
% save matlab.mat;
load matlab.mat;

% Generates C++ code to set matrix
[i,j,z] = find(M);
for n=1:numel(i)
    disp(sprintf('M.set_coef(%d, %d, %.3f)',i(n)-1,j(n)-1 ,z(n)));
end    

% Generates C++ code to set b
for n=1:numel(b)
    disp(sprintf('b[%d] = %.3f;',n-1,b(n)));
end