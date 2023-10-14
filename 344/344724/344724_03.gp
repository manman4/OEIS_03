M=18;

a(n) = my(x='x+O('x^(n+10))); polcoef(sum(k=1, n, (k^n-(k-1)^n)*x^k/(1+x^k))/(1-x), n);
for(n=1, M, print1(a(n), ", "));
