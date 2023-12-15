M=18;

a(n) = n! * sum(k=0, n, binomial(n+2*k+2,n-k)/k!);
for(n=0, M, print1(a(n), ", "));

