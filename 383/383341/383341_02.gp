M=11;

\\ E.g.f. of column k: exp(-k*x) / (1-x)^(k+1).
a(n, k) = my(x='x+O('x^(n+1))); if(k==0, n!, n!*polcoef(exp(-k*x) / (1-x)^(k+1), n));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")))


