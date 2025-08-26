my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(5*x) * BesselI(2, 4*x) / 4, with offset 2.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(5*x) * my_besseli(2, 4*x) / 4))
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(5*x) * my_besseli(2, 4*x) / 4 - exp(5*x) * sum(k=0, N, (4*x/2)^(2*k+2)/(k! * (k+2)!) / 4)))
my(N=30, x='x+O('x^N)); serlaplace(exp(5*x) * my_besseli(2, 4*x) / 4)