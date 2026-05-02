a(n) = sum(k=0, n\4, binomial(n, k)*binomial(2*n, n+4*k));
for(n=0, 19, print1(a(n),", "));

\\ a(n) = [x^n] ( (1+x)^2 * (1+x^4) )^n.
b(n) = polcoef( ( (1+x)^2 * (1+x^4) )^n, n);
for(n=0, 30, print1(a(n)-b(n),", "));
