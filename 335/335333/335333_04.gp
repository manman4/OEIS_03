my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f. of column k: exp((2*k+1)*x) * BesselI(0, 2*sqrt(k*(k+1))*x).
T(n,k) = my(x='x+O('x^(n+1))); round(n! * polcoef(exp((2*k+1)*x) * my_besseli(0, 2*sqrt(k*(k+1))*x), n));
for(n=0, 10, for(k=0, n, print1(T(k,n-k),", ")));
