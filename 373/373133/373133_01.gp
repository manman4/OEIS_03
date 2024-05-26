J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a(n, k=3, m=3) = sumdiv(n, d, J(d, k)*sigma(d^m));
for(n=1, 29, print1(a(n), ", ")) 


b(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sigma( ( n/gcd([i, j, k, n]) )^3 ))));

for(n=1, 50, print1(a(n)-b(n), ", ")) 

