a(n) = sum(j=1, n, sum(k=1, n, ( n/gcd([j, k, n]) )^3 ));
for(n=1, 30, print1(a(n), ", "))


