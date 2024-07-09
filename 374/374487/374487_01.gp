a025235(n) = sum(k=0, n\2, 2^k*binomial(n, 2*k)*binomial(2*k, k)/(k+1));
for(n=0, 28, print1(a025235(n), ", "))

\\ a(n) = binomial(n+2,2) * A025235(n)
a(n) = binomial(n+2, 2) * a025235(n);
for(n=0, 25, print1(a(n), ", ")) 