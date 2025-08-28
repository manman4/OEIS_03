my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(5*x) * BesselI(1, 2*sqrt(6)*x) / sqrt(6), with offset 1.
my(N=30, x='x+O('x^N)); Vec(apply(round, serlaplace(exp(5*x) * my_besseli(1, 2*sqrt(6)*x) / sqrt(6))))
my(N=30, x='x+O('x^N)); Vec(apply(round, serlaplace(exp(5*x) * my_besseli(1, 2*sqrt(6)*x) / sqrt(6) - exp(5*x) * sum(k=0, N, (2*sqrt(6)*x/2)^(2*k+1)/(k! * (k+1)!) / sqrt(6)))))
my(N=30, x='x+O('x^N)); serlaplace(exp(5*x) * my_besseli(1, 2*sqrt(6)*x) / sqrt(6))