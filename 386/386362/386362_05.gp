\\ a(n) = Sum_{k=0..floor(n/2)} 9^k * 7^(n-2*k) * binomial(n,2*k) * Catalan(k).
catalan(n) = binomial(2*n, n)/(n+1);
a(n) = sum(k=0, n\2, 9^k * 7^(n-2*k) * binomial(n, 2*k) * catalan(k));
for(n=0, 25, print1(a(n),", "));
