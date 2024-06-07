\\ a(n) = sum(k=0, n\3, 9^k*binomial(n/3-1, k));
a(n) = sum(k=0, n\3, 9^k * binomial(n/3-1, k));
for(n=0, 33, print1(a(n),", "))

\\ a(3*n) = 10^(n-1) for n > 0.
for(n=1, 50, print1(a(3*n)-10^(n-1), ", "))
