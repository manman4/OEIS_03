a(n) = sum(k=0, n, (-9)^k * binomial(-2*(n-k)/3,k));

for(n=0, 20, print1(a(n),", "))          