\\ n*a(n) = (8*n-2)*a(n-1) - 12*n*a(n-2) for n > 1.
a(n) = my(v=[1, 6]); if(n<2, v[n+1], ((8*n-2)*a(n-1) - 12*n*a(n-2))/n);
for(n=0, 15, print1(a(n), ", "));

