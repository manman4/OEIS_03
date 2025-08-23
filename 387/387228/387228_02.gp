\\ n*a(n) = (6*n+6)*a(n-1) - 5*n*a(n-2) for n > 1.
a(n) = my(v=[1,12]); if(n<2, v[n+1],  ( (6*n+6)*a(n-1) - 5*n*a(n-2) )/n);
for(n=0, 19, print1(a(n),", "));   





