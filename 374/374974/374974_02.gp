\\ G.f.: ( Sum_{k>=1} k * x^k/(1 - x^k) ) * ( Sum_{k>=1} k^2 * x^k/(1 - x^k) ).
my(N=40, x='x+O('x^N)); concat(0, Vec(sum(k=1, N, k*x^k/(1-x^k)) * sum(k=1, N, k^2*x^k/(1-x^k))))


