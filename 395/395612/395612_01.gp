a(n) = sum(k=0, n, binomial(n, k)*binomial(4*n, n+2*k));
for(n=0, 19, print1(a(n),", "));

\\ a(n) = [x^(3*n)] ( (1+x)^4 * (1+x^2) )^n.
b(n) = polcoef( ( (1+x)^4 * (1+x^2) )^n, 3*n);
for(n=0, 30, print1(a(n)-b(n),", "));
