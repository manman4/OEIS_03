my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(5*x) * BesselI(2, 2*x), with offset 2.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(5*x) * my_besseli(2, 2*x)))
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(5*x) * my_besseli(2, 2*x) - exp(5*x) * sum(k=0, N, x^(2*k+2)/(k! * (k+2)!))))
my(N=30, x='x+O('x^N)); serlaplace(exp(5*x) * my_besseli(2, 2*x))