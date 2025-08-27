my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(3*x) * BesselI(2, 2*sqrt(2)*x) / 2, with offset 2.
my(N=30, x='x+O('x^N)); Vec(apply(round, serlaplace(exp(3*x) * my_besseli(2, 2*sqrt(2)*x) / 2)))
my(N=30, x='x+O('x^N)); Vec(apply(round, serlaplace(exp(3*x) * my_besseli(2, 2*sqrt(2)*x) / 2 - exp(3*x) * sum(k=0, N, (2*sqrt(2)*x/2)^(2*k+2)/(k! * (k+2)!) / 2))))
my(N=30, x='x+O('x^N)); serlaplace(exp(3*x) * my_besseli(2, 2*sqrt(2)*x) / 2)