\\ a(n) = (-1)^n * n! * Sum_{k=0..n} binomial(2*k-2,n-k)/k!.
a(n) = (-1)^n * n! * sum(k=0, n, binomial(2*k-2, n-k)/k!);
for(n=0, 21, print1(a(n),", ")) 

