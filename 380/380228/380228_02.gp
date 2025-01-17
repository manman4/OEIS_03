M=21;

Bell(n) = n!*polcoeff(exp(exp(x+x*O(x^n))-1), n);
\\ a(n) = Sum_{k=0..n} 2^(n-k) * Stirling2(n,k) * Bell(k).
a(n) = sum(k=0, n, 2^(n-k) * stirling(n, k, 2) * Bell(k));
for(n=0, M, print1(a(n), ", "))

