a(n) = - n! * sum(k=0, n, (-9)^k * binomial((n-k)/3,k)/(n-k)!);

for(n=0, 17, print1(a(n),", "))          