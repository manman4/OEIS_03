a(n) = sumdiv(n, d, eulerphi(d)*numdiv(d^3));
for(n=1, 56,print1(a(n),", "))                 

for(n=1, 100, if(a(n)==4*n-3, print1(n,", ")))  

b(n) = sum(k=1, n, sigma( (n/gcd(n, k))^3, 0));
for(n=1, 45,print1(a(n)-b(n),", "))

  