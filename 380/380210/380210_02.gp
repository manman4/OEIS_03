M=19;

Bell(n) = n!*polcoeff(exp(exp(x+x*O(x^n))-1), n);
\\ a(n) = Sum_{k=0..n} 5^k * 2^(n-k) * Stirling1(n,k) * Bell(k).
a(n) = sum(k=0, n, 5^k * 2^(n-k) * stirling(n, k, 1) * Bell(k));
for(n=0, M, print1(a(n), ", "))

print1("a(36) is negative.");
print1( a(36) );
