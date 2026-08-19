\\ a(0) = 1; a(n) = Sum_{k=0..n-1} binomial(n,k) * a(k) * b(n-k), where b(n) = n*A054879((n-1)/2) for odd n and b(n) = -1 for even n.
c(n) = (3^(2*n)+3)/4;
for(n=0, 15, print1(c(n), ", "));
b(n) = if(n%2, n*c((n-1)/2), -1);
a(n) = if(n==0, 1, sum(k=0, n-1, binomial(n,k) * a(k) * b(n-k)));
for(n=0, 15, print1(a(n), ", "));