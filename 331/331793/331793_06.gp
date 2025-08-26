my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(5*x) * BesselI(1, 4*x) / 2, with offset 1.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(5*x) * my_besseli(1, 4*x) / 2))
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(5*x) * my_besseli(1, 4*x) / 2 - exp(5*x) * sum(k=0, N, (4*x/2)^(2*k+1)/(k! * (k+1)!)) / 2))
my(N=30, x='x+O('x^N)); serlaplace(exp(5*x) * my_besseli(1, 4*x) / 2)