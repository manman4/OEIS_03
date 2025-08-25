my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(4*x) * BesselI(4, 2*x), with offset 4.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(4*x) * my_besseli(4, 2*x)))
my(N=30, x='x+O('x^N)); serlaplace(exp(4*x) * my_besseli(4, 2*x))