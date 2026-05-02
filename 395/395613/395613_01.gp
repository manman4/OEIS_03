a(n) = sum(k=0, (2*n)\3, binomial(n, k)*binomial(3*n, n+3*k));
for(n=0, 19, print1(a(n),", "));

\\ a(n) = [x^(2*n)] ( (1+x)^3 * (1+x^3) )^n.
b(n) = polcoef( ( (1+x)^3 * (1+x^3) )^n, 2*n);
for(n=0, 30, print1(a(n)-b(n),", "));
