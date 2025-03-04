M=19;

\\ a(0) = 1; a(n) = (1/n) * Sum_{k=1..n} sigma_6(k^2)/sigma_3(k^2) * a(n-k).
a(n) = if(n==0, 1, 1/n*sum(k=1, n, sigma(k^2, 6)/sigma(k^2, 3)*a(n-k)));
for(n=0, M, print1(a(n),", "));
 