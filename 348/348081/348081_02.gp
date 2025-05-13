\\ T(n, k) = (1/(2*k)!)*Sum_{j=0..2*k} (-1)^j*binomial(2*k, j)*(k - j)^(2*n).
T(n, k) = (1/(2*k)!)*sum(j=0, 2*k, (-1)^j*binomial(2*k, j)*(k-j)^(2*n));
a381512(n, k) = (2*n+k)!/k!*polcoef(sinh(x+x*O(x^(2*n+k)))^k, 2*n+k);
\\ a(n) = A036969(3*n,2*n) = A269945(3*n,2*n).
b(n) = T(3*n, 2*n);
for(n=0, 20, print1(b(n),", "));

\\ a(n) = (1/(4*n)!) * Sum_{k=0..4*n} (-1)^k * (2*n-k)^(6*n) * binomial(4*n,k).
a(n) = (1/(4*n)!) * sum(k=0, 4*n, (-1)^k*(2*n-k)^(6*n)*binomial(4*n,k));
for(n=0, 100, print1(a(n)-b(n),", "));
