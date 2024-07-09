a071356(n) = sum(k=0, n\2, 2^(n-k)*binomial(n, 2*k)*binomial(2*k, k)/(k+1));
for(n=0, 28, print1(a071356(n), ", "))

\\ a(n) = binomial(n+2,2) * A071356(n)
a(n) = binomial(n+2, 2) * a071356(n);
for(n=0, 23, print1(a(n), ", ")) 