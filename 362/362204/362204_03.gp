a(n) = n! * sum(k=0, n, 2^(n-k) * binomial(n-k/2-1,n-k)/k!);

for(n=0, 19, print1(a(n),", "))          