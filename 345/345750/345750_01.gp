M=18;

\\ A168243
a(n) = n!*polcoeff(prod(k=1, n, (1+x^k+x*O(x^n))^(1/k)), n);
b(n) = sum(k=0, n, stirling(n, k, 2)*a(k));

for(n=0, M, print1(a(n), ", "));
for(n=0, M, print1(b(n), ", "));