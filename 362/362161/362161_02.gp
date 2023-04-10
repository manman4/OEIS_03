a(n) = (-1)^n * n! * sum(k=0, n, 4^k * binomial((n-k)/2,k)/(n-k)!);

for(n=0, 18, print1(a(n),", "))          