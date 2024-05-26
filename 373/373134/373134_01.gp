J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a(n, k=4, m=4) = sumdiv(n, d, J(d, k)*sigma(d^m));
for(n=1, 24, print1(a(n), ", ")) 


b(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sum(l=1, n, sigma( ( n/gcd([i, j, k, l, n]) )^4 )))));

for(n=1, 30, print1(a(n)-b(n), ", ")) 

