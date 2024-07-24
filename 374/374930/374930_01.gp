\\ a(n) = (31*sigma_5(n) - 70*(n+1)*sigma_3(n) + (40*n^2+60*n+9)*sigma(n))/1920.
a(n) = (31*sigma(n, 5) - 70*(n+1)*sigma(n, 3) + (40*n^2+60*n+9)*sigma(n))/1920;
for(n=3, 43, print1(a(n), ", "))