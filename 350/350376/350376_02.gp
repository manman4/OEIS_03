M=16;

a(n) = x='x+O('x^(n+10)); polcoef(1/prod(k=1, n, 1-k*x)^2, n);
for(n=0, M, print1(a(n), ", "));