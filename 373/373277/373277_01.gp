a(n) = sumdiv(n, d, (gcd(d, 7)==1)*(moebius(d)*7^(n/d)))/(7*n);
for(n=1, 23, print1(a(n),", "))
