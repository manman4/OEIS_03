\\ a(n) = Sum_{k=1..n-1} sigma(k) * sigma_2(n-k).
a(n) = sum(k=1, n-1, sigma(k)*sigma(n-k,2));
for(n=1, 40, print1(a(n),", "))

