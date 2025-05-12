\\ column k is the expansion of 1/Product_{j=0..k} (1 - j*x)^2.
a(n, k) = my(x='x+O('x^(n+1))); polcoef(1/prod(j=0, k, (1-j*x)^2), n);
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));

