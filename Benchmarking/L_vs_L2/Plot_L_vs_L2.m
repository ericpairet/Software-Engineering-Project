
NM = [10005 39790 110592 172800 270000 464920 574000 708588 874800];

% L2
L2 = [0.11 0.5 1.42 2.06 3.73 5.45 6.51 7.89 9.14];
t_L2 = @(nm) 1.109e-05 .* nm;
sys_L2 = [0.92 5.08 24.17 43.2 85.15 247.33 483.23 608.17 826.812];
t_sys_L2 = @(nm) 8.991e-10 .* nm.^2 + 0.0001939 .* nm;

% L
sys_L = [0.23 1.96 4.83 8.54 18.01 53.28 102.28 124.42 160.22];
t_sys_L = @(nm) 1.544e-10 .* nm.^2 + 5.786e-05 .* nm;

% Plot
b_nm = 0:10:874800;
plot( NM , L2 , '*b' ); hold on;
plot( b_nm , t_L2( b_nm ) , 'b' ); hold on;
plot( NM , sys_L , '*r' ); hold on;
plot( b_nm , t_sys_L( b_nm ) , 'r' ); hold on;
plot( NM , sys_L2 , '*k' ); hold on;
plot( b_nm , t_sys_L2( b_nm ) , 'k' ); hold off;

xlim([0 900000]);
ylim([0 900]);

xlabel('N * M [pixels]')
ylabel('Time [s]')
