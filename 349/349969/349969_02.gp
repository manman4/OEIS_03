M=18;

a(n) = x='x+O('x^(n+10)); polcoef(sum(k=0, n, x^k/(1-n*k*x)), n);
for(n=0, M, print1(a(n), ", "));