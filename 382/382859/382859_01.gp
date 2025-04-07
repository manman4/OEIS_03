M=19;

b(n) = sum(k=0, n, binomial(n, k)*binomial((n-1)*(k+1), n-k));
for(n=0, M, print1(b(n), ", "));

\\ a(n) = [x^n] (1 + x/(1-x)^n)^n.
a(n) = my(x='x+O('x^(n+1))); polcoef((1 + x/(1-x)^n)^n, n);
for(n=0, 50, print1(a(n)-b(n), ", "));



