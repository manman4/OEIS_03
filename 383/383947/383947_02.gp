\\ n*a(n) = (6*n-4)*a(n-1) + 7*n*a(n-2) for n > 1.
a(n) = my(v=[1, 2]); if(n<2, v[n+1], ((6*n-4)*a(n-1) + 7*n*a(n-2))/n);
for(n=0, 15, print1(a(n), ", "));


