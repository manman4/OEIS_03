\\ a(n) = Sum_{k=0..n} 4^k * binomial(n,k) * Catalan(k).
a(n) = sum(k=0, n, 4^k*binomial(n, k)*(2*k)!/(k!*(k+1)!));
for(n=0, 19, print1(a(n), ", "))