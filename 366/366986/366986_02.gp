T(n, k) = my(N=60, x='x+O('x^(n+10))); polcoef( sum(j=1, n, x^j/(1 - x^j)^(k+1) ), n);
for(n=1, 12, for(k=1, n, print1(T(k, n-k), ", ")));
