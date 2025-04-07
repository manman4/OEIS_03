\\ column k is the expansion of (1 + x/(1-x)^k)^k.
a(n, k) = my(x='x+O('x^(n+1))); polcoef((1 + x/(1-x)^k)^k, n);
for(n=0, 20, for(k=0, n, print1(a(k, n-k)", ")));