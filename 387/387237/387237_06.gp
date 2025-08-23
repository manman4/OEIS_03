\\ Sum_{k=0..floor(n/2)} 3^(n-2*k) * binomial(n+2,n-2*k) * binomial(2*k+2,k)
b(n) = sum(k=0, n\2, 3^(n-2*k) * binomial(n+2,n-2*k) * binomial(2*k+2,k));
for(n=0, 20, print1(b(n), ", "));

\\ (binomial(n+4,2)/6) * A026377(n+2)
a(n) = (binomial(n+4,2)/6) * b(n);
for(n=0, 20, print1(a(n), ", "));