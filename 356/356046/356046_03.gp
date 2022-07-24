M=18;

a(n) = my(N=40, x='x+O('x^(n+1))); polcoef(sum(k=1, n, sigma(k, n)*x^k/(1-x^k))/(1-x), n);
for(n=1, M, print1(a(n), ", "));