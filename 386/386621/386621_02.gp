\\ Square array A(n,k), n>=0, k>=0, read by antidiagonals, where column k is the expansion of 1/sqrt(1 - 4*k*x - 4*x^2).
a(n, k) = my(x='x+O('x^(n+1))); polcoef(1/sqrt(1 - 4*k*x - 4*x^2), n);
for(n=0, 9, for(k=0, n, print1(a(k,n-k),", ")));