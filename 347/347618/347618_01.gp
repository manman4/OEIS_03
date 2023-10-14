T(n, k) = L=n^k+1; x='x+O('x^L); polcoef(sum(i=n, n^k, x^i/prod(j=1, i, 1-x^j)), n^k); 
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", ")));