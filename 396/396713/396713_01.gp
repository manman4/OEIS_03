\\ Expansion of e.g.f. -log(1 + W_3(-x)), where W_k(x) is the k-th iterate of LambertW(x).
my(N=20, x='x+O('x^N)); Vec(serlaplace(-log(1+lambertw(lambertw(lambertw(-x))))))
\\ E.g.f.: exp(B(x)) - 1, where B(x) is the e.g.f. of A396680.
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-log(1+lambertw(lambertw(lambertw(-x))))) - 1))

\\ Expansion of e.g.f. -W_3(-x)/(1 + W_3(-x)), where W_k(x) is the k-th iterate of LambertW(x).
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-log(1+lambertw(lambertw(lambertw(-x))))) - 1 + lambertw(lambertw(lambertw(-x)))/(1+lambertw(lambertw(lambertw(-x)))) ))
