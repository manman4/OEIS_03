M=13;

a(n) = x='x+O('x^(n+10)); n^n*polcoef(sum(k=0, n, (k*x)^k/(1-k*x)), n);
for(n=0, M, print1(a(n), ", "));