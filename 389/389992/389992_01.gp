\\ a(n) = n! * Sum_{k=0..n} binomial(n+k+2,n-k)/k!.
a(n) = n! * sum(k=0, n, binomial(n+k+2,n-k)/k!);
for(n=0, 19, print1(a(n),", "));

