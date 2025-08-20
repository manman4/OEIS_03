\\ G.f. of column k: 2/(1 - (2*k+1)*x + sqrt((1-x) * (1-(4*k+1)*x))).
a(n, k) = my(x='x+O('x^(n+3))); polcoef( 2/(1 - (2*k+1)*x + sqrt((1-x) * (1-(4*k+1)*x))) , n);
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));
