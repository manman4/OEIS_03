a(n) = sumdiv(n, d, eulerphi(d)*numdiv(d^n));
for(n=1, 45,print1(a(n),", "))                 

for(n=1, 100, if(a(n)==n^2, print1(n,", ")))  

b(n) = sum(k=1, n, sigma( (n/gcd(n, k))^n, 0));
for(n=1, 45,print1(a(n)-b(n),", "))

  