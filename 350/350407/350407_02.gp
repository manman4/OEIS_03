M=29;

a(n) = x='x+O('x^(n+10)); polcoef(1/prod(k=1, n, 1+x+x^2+x^3+x^4), n);
for(n=0, M, print1(a(n), ", "));