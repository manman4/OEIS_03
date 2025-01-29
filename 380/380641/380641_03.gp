\\ E.g.f.: exp( Sum_{k>=1} k*(k+1)/2 * 3^(k-1) * x^k ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( exp(sum(k=1, N, k*(k+1)/2 * 3^(k-1) * x^k)) ))


