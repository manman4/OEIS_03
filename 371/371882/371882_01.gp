h(n) = sum(k=1, n, 1/k);

\\ a(n) = Sum_{k=0..n} binomial(n,k)^7 * (1 - 7*k*(H(k) - H(n-k)), where H(n) is the n-th harmonic number.
a(n) = sum(k=0, n, binomial(n, k)^7 * (1 - 7*k*(h(k) - h(n-k))));
for(n=0, 15, print1(a(n), ", "))
