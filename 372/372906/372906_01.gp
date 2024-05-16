a(n) = sum(j=1, n, sum(k=1, n, gcd(j, n)*gcd(k, n)/gcd(j*k, n)));
for(n=1, 51, print1(a(n), ", ")); 