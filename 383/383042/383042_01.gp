M=11;

\\ A(n,k) = (1/n) * Sum_{d|n} mu(n/d) * (k^d - (k-1)^d).
a(n, k) = (1/n) * sumdiv(n , d, moebius(n/d) * (k^d - (k-1)^d));
for(n=1, M, for(k=1, n, print1(a(k, n-k+1), ", "))); 

\\ A(n,k) = (1/n) * (k^n - (k-1)^n - Sum_{d<n and d|n} d * A(d,k)).
a(n, k)= (1/n) * (k^n - (k-1)^n - sum(d=1, n-1, (n%d==0) * d * a(d, k))); 
for(n=1, M, for(k=1, n, print1(a(k, n-k+1), ", "))); 

\\ A(n,k) = A074650(n,k) - A074650(n,k-1).
a074650(n, k)= (1/n) * (k^n - sum(d=1, n-1, (n%d==0) * d * a074650(d, k)));
b(n, k) = a074650(n, k) - a074650(n, k-1);
for(n=1, 20, for(k=1, n, print1(a(k, n-k+1)-b(k, n-k+1), ", "))); 

\\ Main diagonal gives A306173.
for(n=1, 20, print1(a(n, n), ", "));