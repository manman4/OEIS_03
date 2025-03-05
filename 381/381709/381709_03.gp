M=19;

\\ a(0) = 1; a(n) = (1/n) * Sum_{k=1..n} sigma_4(k^2) * a(n-k).
a(n) = if(n==0, 1, 1/n*sum(k=1, n, sigma(k^2, 4)*a(n-k)));
for(n=0, M, print1(a(n),", "));
 