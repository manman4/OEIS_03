my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(7*x) * BesselI(2, 6*x) / 9, with offset 2.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(7*x) * my_besseli(2, 6*x) / 9))
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(7*x) * my_besseli(2, 6*x) / 9 - exp(7*x) * sum(k=0, N, (6*x/2)^(2*k+2)/(k! * (k+2)!) / 9)))
my(N=30, x='x+O('x^N)); serlaplace(exp(7*x) * my_besseli(2, 6*x) / 9)