my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace(sum(k=0, 3, stirling(3, k, 2)*x^k)*exp(x)/(1+x))))

\\ n次式のベル多項式の係数を求める
Bell_poly(n) = sum(k=0, n, stirling(n, k, 2)*x^k);
\\ E.g.f.: B_3(x) * exp(x) / (1+x), where B_n(x) = Bell polynomials.
my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace(Bell_poly(3) * exp(x) / (1+x))))
