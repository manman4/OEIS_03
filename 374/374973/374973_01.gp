\\ a(n) = Sum_{k=1..n-1} tau(k) * sigma_2(n-k).
a(n) = sum(k=1, n-1, sigma(k,0)*sigma(n-k,2));
for(n=1, 43, print1(a(n),", "))

