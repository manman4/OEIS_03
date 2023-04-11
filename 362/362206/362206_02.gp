a(n) = sum(k=0, n, (-9)^k * binomial(-(n-k)/3,k));

for(n=0, 21, print1(a(n),", "))          