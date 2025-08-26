my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(7*x) * BesselI(1, 6*x) / 3, with offset 1.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(7*x) * my_besseli(1, 6*x) / 3))
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(7*x) * my_besseli(1, 6*x) / 3 - exp(7*x) * sum(k=0, N, (6*x/2)^(2*k+1)/(k! * (k+1)!) / 3)))
my(N=30, x='x+O('x^N)); serlaplace(exp(7*x) * my_besseli(1, 6*x) / 3)