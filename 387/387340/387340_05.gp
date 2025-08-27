my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(4*x) * BesselI(3, 2*sqrt(3)*x) / (3*sqrt(3)), with offset 3.
my(N=30, x='x+O('x^N)); Vec(apply(round, serlaplace(exp(4*x) * my_besseli(3, 2*sqrt(3)*x) / (3*sqrt(3)))))
my(N=30, x='x+O('x^N)); Vec(apply(round, serlaplace(exp(4*x) * my_besseli(3, 2*sqrt(3)*x) / (3*sqrt(3)) - exp(4*x) * sum(k=0, N, (2*sqrt(3)*x/2)^(2*k+3)/(k! * (k+3)!) / (3*sqrt(3))))))
my(N=30, x='x+O('x^N)); serlaplace(exp(4*x) * my_besseli(3, 2*sqrt(3)*x) / (3*sqrt(3)))