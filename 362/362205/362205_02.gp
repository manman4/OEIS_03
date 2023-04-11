a(n) = n! * sum(k=0, n, (-3)^k * binomial(-(n-k)/3,k)/(n-k)!);

for(n=0, 18, print1(a(n),", "))          