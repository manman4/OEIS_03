a(n) = n! * sum(k=0, n, 3^(n-k) * binomial(n-2*k/3-1,n-k)/k!);

for(n=0, 18, print1(a(n),", "))          