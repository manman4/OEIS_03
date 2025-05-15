\\ column k is the expansion of 1/(1 - k*x) * Product_{j=0..k-1} (1 + j*x)/(1 - j*x).
a(n, k) = my(x='x+O('x^(n+1))); polcoef(1/(1 - k*x) * prod(j=0, k-1, (1+j*x)/(1-j*x)), n);
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));

