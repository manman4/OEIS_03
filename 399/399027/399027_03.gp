M=18;
\\ a(0) = 1; a(n) = Sum_{k=0..n-1} binomial(n-1,k) * a(k) * A066532(n-k).
a066532(n) = { if (n%2, 1, 2^(n-1)) };
a(n) = if(n==0, 1, sum(k=0, n-1, binomial(n-1, k) * a(k) * a066532(n-k)));
for(n=0, M, print1(a(n), ", "))

