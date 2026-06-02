\\ Expansion of e.g.f. -log(1 + W_2(-x)), where W_k(x) is the k-th iterate of LambertW(x).
my(N=20, x='x+O('x^N)); Vec(serlaplace(-log(1 + lambertw(lambertw(-x)))))