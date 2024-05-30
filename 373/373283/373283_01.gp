b(n, k) = sumdiv(n, d, (gcd(d, k)==1)*(moebius(d)*k^(n/d)))/(k*n);
a(n, k=7) = sumdiv(n, d, d*b(d, k));

for(n=1, 50, print1(b(n, 7), ", "))
for(n=1, 50, print1(a(n), ", "))
