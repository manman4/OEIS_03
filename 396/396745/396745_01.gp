\\ Expansion of e.g.f. log(1 - W_2(-x)), where W_k(x) is the k-th iterate of LambertW(x).
my(N=20, x='x+O('x^N)); Vec(serlaplace(log(1-lambertw(lambertw(-x)))))
\\ E.g.f.: 1 - exp(-B(x)), where B(x) is the e.g.f. of A396677.
my(N=20, x='x+O('x^N)); Vec(serlaplace(1 - exp(-log(1-lambertw(lambertw(-x)))) ))

\\ Expansion of e.g.f. -W_2(-x)/(1 - W_2(-x)), where W_k(x) is the k-th iterate of LambertW(x).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1 - exp(-log(1-lambertw(lambertw(-x)))) + lambertw(lambertw(-x))/(1-lambertw(lambertw(-x))) ))