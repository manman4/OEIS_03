J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a(n, k=2, m=1) = sumdiv(n, d, J(d, k)*sigma(d^m));
for(n=1, 42, print1(a(n), ", ")) 


b(n) = sum(i=1, n, sum(j=1, n, sigma( n/gcd([i, j, n]) )));
c(n) = sum(i=1, n, sum(j=1, n, sigma( ( gcd(i, n)/gcd([i, j, n]) )^2 )));

for(n=1, 50, print1(a(n)-b(n), ", ")) 
for(n=1, 50, print1(a(n)-c(n), ", ")) 

