M=18;

a(n) = n! * sum(k=0, n, (k+1)^(k-1) * binomial(k,n-k)/k!);
for(n=0, M, print1(a(n), ", "))
