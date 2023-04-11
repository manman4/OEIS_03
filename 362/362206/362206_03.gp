a(n) = sum(k=0, n, 9^(n-k) * binomial(n-2*k/3-1,n-k));

for(n=0, 21, print1(a(n),", "))          