my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(7*x) * BesselI(3, 6*x) / 27, with offset 3.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(7*x) * my_besseli(3, 6*x) / 27))
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(7*x) * my_besseli(3, 6*x) / 27 - exp(7*x) * sum(k=0, N, (6*x/2)^(2*k+3)/(k! * (k+3)!) / 27)))
my(N=30, x='x+O('x^N)); serlaplace(exp(7*x) * my_besseli(3, 6*x) / 27)