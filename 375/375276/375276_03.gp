\\ n * a(n) = 2*(2*n-1)*a(n-1) + 4*(2*n-3)*a(n-3) - 4*(n-2)*a(n-4).
a(n) = my(v=[1,2,6,24]); if(n<4, v[n+1], ( 2*(2*n-1)*a(n-1) + 4*(2*n-3)*a(n-3) - 4*(n-2)*a(n-4) )/n);
for(n=0, 25, print1(a(n), ", "))

