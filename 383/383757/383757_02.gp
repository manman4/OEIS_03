\\ G.f.: exp( Sum_{k>=1} f(4*k)/f(k) * x^k/k ), where f(k) = 4^k - 3^k.
my(N=30, x='x+O('x^N), f(k) = 4^k - 3^k); Vec( exp( sum(k=1, N, f(4*k)/f(k) * x^k/k) ) )

