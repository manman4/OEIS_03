my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f. of column k: exp(2*x) * BesselI(0, 2*sqrt(k^2+1)*x).
a(n,k) = my(x='x+O('x^(n+1))); round(n! * polcoef(exp(2*x) * my_besseli(0, 2*sqrt(k^2+1)*x), n));
for(n=0, 9, for(k=0, n, print1(a(k,n-k),", ")));