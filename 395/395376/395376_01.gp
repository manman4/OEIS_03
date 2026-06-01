\\ E.g.f.: exp(B(x)), where B(x) is the e.g.f. of A396558.
my(N=20, x='x+O('x^N)); Vec(serlaplace(    lambertw(-lambertw(lambertw(-x)))))
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(lambertw(-lambertw(lambertw(-x))))))
