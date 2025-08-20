\\ a(n) = Sum_{k=0..n} 3^k * binomial(n,k) * Catalan(k+1).
catalan(n) = binomial(2*n, n)/(n+1);
a(n) = sum(k=0, n, 3^k * binomial(n, k) * catalan(k+1));
for(n=0, 25, print1(a(n),", "));
