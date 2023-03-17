M=19;

a(n) = n! * sum(k=0, n\3, binomial(n,3*k)/k!);
for(n=0, M, print1(a(n), ", "));
