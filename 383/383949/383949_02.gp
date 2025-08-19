\\ n*a(n) = (6*n-2)*a(n-1) - 5*n*a(n-2) for n > 1.
a(n) = my(v=[1, 4]); if(n<2, v[n+1], ((6*n-2)*a(n-1) - 5*n*a(n-2))/n);
for(n=0, 15, print1(a(n), ", "));

