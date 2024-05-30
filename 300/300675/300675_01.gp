b(n, k) = sumdiv(n, d, (gcd(d, k)==1)*(moebius(d)*k^(n/d)))/(k*n);

for(n=1, 500, write("/Users/xxx/Desktop/b300675_1.txt", n, " ", b(n, 16)))
