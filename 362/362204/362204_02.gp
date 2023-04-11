a(n) = n! * sum(k=0, n, (-2)^k * binomial(-(n-k)/2,k)/(n-k)!);

for(n=0, 19, print1(a(n),", "))          