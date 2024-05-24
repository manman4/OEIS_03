\\ a(n) = Sum_{d|n} phi(n/d) * (n/d)^6 * sigma_6(d^2)/sigma_3(d^2).
a(n) = sumdiv(n, d, eulerphi(n/d) * (n/d)^6 * sigma(d^2, 6)/sigma(d^2, 3));
for(n=1, 100, print1(a(n), ", "))
