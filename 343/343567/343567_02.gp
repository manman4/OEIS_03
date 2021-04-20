M=20;

a(n) = my(x='x+O('x^(n+1))); polcoef(sum(k=1, n, (k*x)^k/(1-x^k)^n), n);
for(n=1, M, print1(a(n), ", "));