my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(4*x) * BesselI(3, 2*x), with offset 3.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(4*x) * my_besseli(3, 2*x)))
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(4*x) * my_besseli(3, 2*x) - exp(4*x) * sum(k=0, N, x^(2*k+3)/(k! * (k+3)!))))
my(N=30, x='x+O('x^N)); serlaplace(exp(4*x) * my_besseli(3, 2*x))