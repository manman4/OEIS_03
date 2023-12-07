b(n) = polcoeff( sum( k=0, n, prod( i=1, k, x / (1 - i*x)), x^n * O(x)), n);
a(n) = n! * sum(k=0, n, 2^(n-k) * b(k)/k!);

for(n=0, 20, print1(a(n),", "))   