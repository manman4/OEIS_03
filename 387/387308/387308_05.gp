my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(5*x) * BesselI(3, 4*x) / 8, with offset 3.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(5*x) * my_besseli(3, 4*x) / 8))
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(5*x) * my_besseli(3, 4*x) / 8 - exp(5*x) * sum(k=0, N, (4*x/2)^(2*k+3)/(k! * (k+3)!) / 8)))
my(N=30, x='x+O('x^N)); serlaplace(exp(5*x) * my_besseli(3, 4*x) / 8)