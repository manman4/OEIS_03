a025237(n) = sum(k=0, n\2, 3^k*binomial(n, 2*k)*binomial(2*k, k)/(k+1));
for(n=0, 28, print1(a025237(n), ", "))

\\ a(n) = binomial(n+2,2) * A025237(n)
a(n) = binomial(n+2, 2) * a025237(n);
for(n=0, 23, print1(a(n), ", ")) 