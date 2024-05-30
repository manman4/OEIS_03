b(n, k) = sumdiv(n, d, (gcd(d, k)==1)*(moebius(d)*k^(n/d)))/(k*n);
a(n, k=3) = sumdiv(n, d, d*b(d, k));

for(n=1, 50, print1(b(n, 3), ", "))
for(n=1, 50, print1(a(n), ", "))
