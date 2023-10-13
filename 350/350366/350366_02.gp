M=16;

a(n) = x='x+O('x^(n+10)); polcoef(prod(k=1, n, (1+k*x)/(1-k*x)), n);
for(n=0, M, print1(a(n), ", "));