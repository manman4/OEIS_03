\\ G.f. of column k: 1/Product_{j=0..floor(k/2)} (1 - (k-2*j)^2*x).
a(n, k) = polcoef(1/prod(j=0, k\2, 1 - (k-2*j)^2*x + x*O(x^n)), n);
for(n=0, 10, for(k=0, n, print1(a(k, n-k), ", ")));
