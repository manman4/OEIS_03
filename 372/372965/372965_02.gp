a(n) = sumdiv(n, d, d^4 * eulerphi(d));
for(n=1, 30, print1(a(n), ", "))

my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, k^4 * eulerphi(k) * x^k/(1 - x^k) ))


a(n) = sumdiv(n, d, d^3 * eulerphi(d^2));
for(n=1, 30, print1(a(n), ", "))

my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, k^3 * eulerphi(k^2) * x^k/(1 - x^k) ))


a(n) = sumdiv(n, d, d^2 * eulerphi(d^3));
for(n=1, 30, print1(a(n), ", "))

my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, k^2 * eulerphi(k^3) * x^k/(1 - x^k) ))


a(n) = sumdiv(n, d, d * eulerphi(d^4));
for(n=1, 30, print1(a(n), ", "))

my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, k * eulerphi(k^4) * x^k/(1 - x^k) ))


a(n) = sumdiv(n, d, eulerphi(d^5));
for(n=1, 30, print1(a(n), ", "))

my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, eulerphi(k^5) * x^k/(1 - x^k) ))


a(n) = sumdiv(n, d, 1/d * eulerphi(d^6));
for(n=1, 30, print1(a(n), ", "))

my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, 1/k * eulerphi(k^6) * x^k/(1 - x^k) ))