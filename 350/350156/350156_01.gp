\\ a(n) = Sum_{d|n} phi(n/d) * (n/d)^2 * sigma_2(d^2)/sigma(d^2).
a(n) = sumdiv(n, d, eulerphi(n/d) * (n/d)^2 * sigma(d^2, 2)/sigma(d^2, 1));

for(n=1, 100, print1(a(n), ", "))
