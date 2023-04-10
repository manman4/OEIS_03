a(n) = (-1)^n * n! * sum(k=0, n, 2^k * binomial((n-k)/2,k)/(n-k)!);

for(n=0, 20, print1(a(n),", "))          