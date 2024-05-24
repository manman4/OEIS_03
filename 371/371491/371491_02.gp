a(n) = sumdiv(n, d, eulerphi(n/d) * (n/d)^3 * sigma(d^2, 8)/sigma(d^2, 4));
for(n=1, 24, print1(a(n), ", "))



