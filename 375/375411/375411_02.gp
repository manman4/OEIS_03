\\ a(n) = (-1)^n * n! * Sum_{k=0..n} binomial(3*k-1,n-k)/k!.
a(n) = (-1)^n * n! * sum(k=0, n, binomial(3*k-1, n-k)/k!);
for(n=0, 20, print1(a(n),", ")) 

