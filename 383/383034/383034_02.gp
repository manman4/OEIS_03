M=25;

\\ a(n) = (1/n) * (2^n - 1 + Sum_{d<n and d|n} (-1)^(n/d) * d * a(d)).
a(n)= (1/n) * (2^n - 1 + sum(d=1, n-1, (n%d==0) * (-1)^(n/d) * d * a(d)));
for(n=1, M, print1(a(n), ", "));

\\ a(n) = A306156(n) - A209229(n).
a383023(n, k)= (1/n) * (k^n + sum(d=1, n-1, (n%d==0) * (-1)^(n/d) * d * a383023(d, k)));
a306156(n) = a383023(n, 2);
a209229(n) = a383023(n, 1);
b(n) = a306156(n) - a209229(n);
for(n=1, M, print1(a(n)-b(n), ", "));

