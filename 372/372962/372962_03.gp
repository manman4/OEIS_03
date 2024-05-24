\\ a(n) = Sum_{d|n} phi(n/d) * (n/d)^4 * sigma_4(d^2)/sigma_2(d^2).
a(n) = sumdiv(n, d, eulerphi(n/d) * (n/d)^4 * sigma(d^2, 4)/sigma(d^2, 2));

for(n=1, 100, print1(a(n), ", "))
