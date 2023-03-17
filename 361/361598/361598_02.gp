M=19;

a(n) = n! * sum(k=0, n, binomial(n+k,2*k)/k!);
for(n=0, M, print1(a(n), ", "));
