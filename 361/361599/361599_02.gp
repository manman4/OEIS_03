M=19;

a(n) = n! * sum(k=0, n, binomial(n+2*k,3*k)/k!);
for(n=0, M, print1(a(n), ", "));
