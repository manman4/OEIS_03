a(n) = (-1)^n * sum(k=0, n, 4^k * binomial((n-k)/2,k));

for(n=0, 28, print1(a(n),", "))          