M=18;

\\ a(n) = (2*n)!/n! * (1 + n * Sum_{k=1..n} 1/(n+k)).
a(n) = (2*n)!/n! * (1 + n * sum(k=1, n, 1/(n+k)));
for(n=0, M, print1(a(n),", "));


